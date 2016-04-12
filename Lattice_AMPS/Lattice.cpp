//
//  Lattice.cpp
//  Lattice_AMPS
//
//  Created by Oliver Spiro on 3/15/16.
//  Copyright © 2016 Oliver Spiro. All rights reserved.
//


#include "Lattice.hpp"
#include <iostream>
#include <cmath>
#include <fstream>

#define empty 0
#define parasite 1
#define forb 2
#define grass 3

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */




using namespace std;

int mod(int x, int m) { //got from http://stackoverflow.com/questions/1082917/mod-of-negative-number-is-melting-my-brain need because % doesn't behave well with negative #s
    return (x%m + m)%m;
}

Lattice::Lattice(int setWidth, double prob[4], double setBirthRate[4], int setSeedRadius, double setDeathRate, double dt, double setToxinStrength, int setYearLength, double setAmountDevelopment, int devType, int numberofseeds)
{
//===============================Set base properties==============================================
    this->devType = devType;
    width = setWidth;
    lat.resize(width*width); //technically one dimensional
    for(int i = 0;i<width*width;i++)
    {
        lat[i].toxinStrength = setToxinStrength;
        lat[i].deathRate = setDeathRate;
        lat[i].seedRange[0] = 0;
        lat[i].seedRange[1] = numberofseeds;
    }
    
    
    std::copy(setBirthRate,setBirthRate+4,birthRate);
    seedRadius = setSeedRadius;
    yearLength = setYearLength;
    amountDevelopment = setAmountDevelopment;
    
    //seed the generators and set bounds for the uniform distributions
    event_rand.seed(static_cast<unsigned int>(time(NULL)));
    std::uniform_real_distribution<double>::param_type newParams(0, 1);
    unif.param(newParams);
    unifEvent.param(newParams);
    std::uniform_int_distribution<int>::param_type newRunParams(0, 3); //TODO: change if use numneighbors later
    unifRun.param(newRunParams);
    std::uniform_int_distribution<int>::param_type newLocParams(0, width-1);//TODO: use as few rand num gens as possible
    unifLoc.param(newLocParams);
    this->dt = dt;
    
    
    
    //generate random values used in initialization of site lattice
    
    std::vector<double> randmatrix;
    for(int i = 0; i < width*width; i++)
    {
        randmatrix.push_back(unif(event_rand)); //TODO: use randmatrix.reserve
    }
    
    //initializing sites
    
    for(int i = 0; i<width*width;i++)
    {
        if (randmatrix[i]<prob[empty])
        {
            lat[i].species=empty;
        }
        else if (randmatrix[i]<prob[empty]+prob[parasite])
        {
            lat[i].species=parasite;
        }
        else if (randmatrix[i]<prob[empty]+prob[parasite]+prob[forb])
        {
            lat[i].species=forb;
        }
        else
        {
            lat[i].species=grass;
        }
    }
    
     //set neighbors, iterating through as if 2D using width*row+column indexing
    
    for(int i = 0; i<width;i++)
    {
        for(int j = 0;j<width;j++) //TODO: use mod instead of special cases
        {
            
            //periodic boundaries leads to these special conditions on the four edges
            if (i==0)
            {
                lat[width*i+j].top = &lat[width*(width-1)+j]; //wrap around to bottom
            }
            else
            {
                lat[width*i+j].top = &lat[width*(i-1)+j];
            }
            if (i==width-1)
            {
                lat[width*i+j].bottom = &lat[width*0+j];//to top
            }
            else
            {
                lat[width*i+j].bottom = &lat[width*(i+1)+j];
            }
            if (j==0)
            {
                lat[width*i+j].left = &lat[width*i+(width-1)]; //to right
            }
            else
            {
                lat[width*i+j].left = &lat[width*i + (j-1)];
            }
            if (j==width-1)
            {
                lat[width*i+j].right = &lat[width*i+0]; //to left
            }
            else
            {
                lat[width*i+j].right = &lat[width*i + (j+1)];
            }
            lat[width*i+j].setNeighbors(); //put the above into array
            
        }
    }
if (amountDevelopment > 0)
{
    addDevelopment(amountDevelopment);
}
    

}

//=================================Add in Node/Spoke Development===========================================

void Lattice::addDevelopment(double amountDevelopment)
{
    int stepI[4] = {-1,0,1,0};
    int stepJ[4] = {0,1,0,-1};
    std::uniform_real_distribution<double>::param_type newParams(0,1);
    unif.param(newParams);
    // Roll how many nodes and how many lines should be used.  Note that it is possible to roll no nodes or no lines.
    int q = int ( floor(unif(event_rand)*3 + 1) + floor(unif(event_rand)*3 + 1) );
    int numNodes = round(unif(event_rand)*q);
    int numLines = q - numNodes;
     //TODO: add six argument: decType, o==> all nodes 1===> all lines
    switch (devType){
            
        case 0:
            break;
        case 1:
            numNodes = 0;
            numLines = q;
            break;
        case 2:
            numNodes = q;
            numLines = 0;
            break;
    }
    
    
   
    
    
    /*  nodesProportion is the percentage of development that will
     bud off of nodes vs. off of lines.  For now, we simply make
    nodesProportion = number of nodes / (number of nodes + number
    of lines); we will probably want to be able to change this
    later.*/
    
    
    double nodesProportion = double(numNodes)/double(q);

    /* If we roll to have nodes, we now need to decide where they
     are. We pick the sites randomly.  We then change the site of
     the node to developed.*/

    int Nodes[numNodes][2];
    int Lines[numLines][4];
    if(numNodes>0)
    {
        
        std::uniform_int_distribution<int>::param_type newLocParams(0,width-1);
        unifLoc.param(newLocParams);
        for(int i = 0;i < numNodes;i++)
        {
            Nodes[i][0] = unifLoc(event_rand);
            Nodes[i][1] = unifLoc(event_rand);
            lat[width*Nodes[i][0]+Nodes[i][1]].develop();
        }
        
     }
    /* Similarly, if we roll to have lines, we roll their location
      randomly.  We roll the two end points randomly, then develop
      all of the squares between the two end points.
     */
    if(numLines>0)
    {
        for (int i =0; i<numLines; i++)
        {
            if (unif(event_rand)<0.5)
            {
                Lines[i][0] = 0;
                Lines[i][1] = unifLoc(event_rand);
            }
            else
            {
                Lines[i][0] = unifLoc(event_rand);
                Lines[i][1] = 0;
            }
            if (unif(event_rand)<0.5)
            {
                Lines[i][2] = width-1;
                Lines[i][3] = unifLoc(event_rand);
            }
            else
            {
                Lines[i][2] = unif(event_rand);
                Lines[i][3] = width-1;
            }
            for( int j = 0;j<width; j++)
            {
                int curJ = int( Lines[i][0] + floor((double(j)/double(width))*(Lines[i][2]-Lines[i][0])) );
                int curI = int( Lines[i][1] + floor((double(j)/double(width))*(Lines[i][3]-Lines[i][1])) );
                lat[width*curI+curJ].develop();
            }
            
        }
        
    }
    
    /* Now we do a for loop, iterating once for every point of
      amountDevelopment. In each iteration, we're going to develop
      one more square, budding off of the nodes or lines.
     */
    std::uniform_int_distribution<int> nodeRand(0,numNodes-1);
    std::uniform_int_distribution<int> lineRand(0,numLines-1);
    for( int i = 0; i < amountDevelopment;i++)
    {
        double r  = unif(event_rand);
        if (numNodes > 0 && (r < nodesProportion)) //if we bud off a node
        {
            int whichNode = nodeRand(event_rand);
            int curJ = Nodes[whichNode][0];
            int curI = Nodes[whichNode][1];
            
            // Then do a random walk from that node until we reach an undeveloped square:
            while(lat[width*curI+curJ].isDeveloped())
                {
                    int s = unif(event_rand)*4 + 1;
                    curJ = mod(curJ+stepJ[s],width); //TODO: ask mark if mod is correct in matlab code
                    curI = mod(curI + stepI[s],width);
                }
            lat[width*curI+curJ].develop();
        }
        else if (r>nodesProportion)
        {
            //If we budd off a line, pick a point in that line:
            double s = unif(event_rand);
            int whichLine = lineRand(event_rand);
            int curJ = int ( Lines[whichLine][0]+floor(s*(Lines[whichLine][2] - Lines[whichLine][0])) );
            int curI = int ( Lines[whichLine][1]+floor(s*(Lines[whichLine][3] - Lines[whichLine][1])) );
            
            //Then random walk until we reach an undeveloped square:
            while ( lat[width*curI+curJ].isDeveloped() )
            {
                int s = int( floor(unif(event_rand)*4+1));
                curJ = mod(curJ+stepJ[s],width);
                curI = mod(curI + stepI[s],width);
                
            }
            //Set the square to be developed:
            
            lat[width*curI+curJ].develop();
            
        }
        
    }


}






//=================================Check if the site's species runs or dies================================


void Lattice::checkEvent(int i, int j) //TODO: make this and getDeathRate take Site params instead of coordinates
{
    if(lat[width*i+j].isDeveloped()==false)
    {
        double trueDeathRate;
        if ((lat[width*i+j].species != empty))
        {
            trueDeathRate = lat[width*i+j].getDeathRate();
        }
    
    
        double rand = unifEvent(event_rand);
        int rand2 = unifRun(event_rand);
        if(rand<trueDeathRate*dt)
        {
            lat[width*i+j].die();//TODO: assign this element to a variable, then use variable in rest of function.( assign to pointer?)
        }
        else if ((lat[width*i+j].neighbors[rand2]->species==empty) && rand<(trueDeathRate+birthRate[lat[width*i+j].species])*dt) //TODO: convoluted, fix using the above assignment
        {
            lat[width*i+j].neighbors[rand2]->species=lat[width*i+j].species;
        }
    }
}



    
//=========================Winter: spread seeds, wipe out lattice, sprout seeds================
void Lattice::endOfYear()
{
    
    std::uniform_real_distribution<double>::param_type thetaParams(0, 1);
    std::uniform_real_distribution<double>::param_type distParams(0, seedRadius);

    unif.param(thetaParams);
    unif.param(distParams);
    
    for(int i = 0;i<width;i++)
    {
        for(int j = 0;j<width;j++)
        {
            if(lat[width*i+j].isDeveloped()==false)
            {
                for(int k = 1; k <= lat[width*i+j].numSeeds(event_rand);k++)
                {
                double thetarand = unif(event_rand);
                double theta = thetarand*2*3.14159;
            
                double distancerand = unif(event_rand);
                double distance = floor(distancerand); //TODO: ask: is the +1 so that radius==0 corresp to running?
            
                int targJ = mod(int(round(double(i)+distance*cos(theta))),width); //TODO: check behavior, possible problem area
                int targI = mod(int(round(double(j)+distance*sin(theta))),width);
                
            
                //targI,targJ designate target site for seed dispersal, now we add seeds.
            
            
                lat[width*targI+targJ].plant(lat[width*i+j].species);
                }
            }
        }
    
    }
    
    
    
    for(int k = 0; k< width*width;k++)
    {
        if(lat[k].isDeveloped()==false)
        {
            lat[k].die(); //TODO: ask if necessary ...//kill all plants
            lat[k].sproutSeeds(event_rand); //need to pass rng to this fcn
        }
    }
}


//================move to next time step======================================================
void Lattice::advanceTimeStep()
{
    std::uniform_real_distribution<double>::param_type newParams(0,width-1);
    unif.param(newParams);
    
    for(int count = 0; count < width*width; count++)
    {
        int i = unif(event_rand);
        int j = unif(event_rand);
        checkEvent(i,j);
    }
//    printLattice();
}

//==============move time forward one year, WINTER============================================
void Lattice::advanceYear()
{
    int numSteps = floor(yearLength)/dt;
    
    for(int i = 0; i<numSteps; i++)
    {
        advanceTimeStep();
        population();
    }
    endOfYear();
    
}
//====================Print population percentages=============================================

double* Lattice::population()
{
    double emp_count = 0;
    double para_count = 0;
    double forb_count = 0;
    double grass_count = 0;
    for(int i =0;i<width*width;i++)
    {
        if(lat[i].species==empty)
        {
            emp_count++;
        }
        
        else if(lat[i].species==parasite)
        {
            para_count++;
        }
        else if(lat[i].species==forb)
        {
            forb_count++;
        }
        else if(lat[i].species==grass)
        {
            grass_count++;
        }
    }
    double *ratios = new double[4];
    ratios[empty] = 100*emp_count/(double(width)*double(width));
    ratios[parasite] = 100*para_count/(double(width)*double(width));
    ratios[forb] = 100*forb_count/(double(width)*double(width));
    ratios[grass] = 100*grass_count/(double(width)*double(width));
    return ratios;
//    cout<<"Empty: "<<ratios[empty]<<endl;
//    cout<<"Para: "<<ratios[parasite]<<endl;
//    cout<<"Forb: "<<ratios[forb]<<endl;
//    cout<<"Grass: "<<ratios[grass]<<endl<<endl;
    

    
}

//===========================Check for die off of any population===============================

bool Lattice::checkExtinction()
{
    int numdead = 0;
    for(int i = 1; i < 4; i++)
    {
      if(population()[i]==0)
      {
          numdead+=1;
      }
    }
    return (numdead>0);
}

//===================================Self explanatory==========================================


void Lattice::printLattice()
{
    for(int i =0;i<width;i++)
                    {
                        for(int j=0;j<width;j++)
                        {
                            if(lat[width*i+j].species==empty)
                            {
                                cout<<WHITE<<"o "<<RESET;
                            }
                            else if(lat[width*i+j].species==parasite)
                            {
                                cout<< RED << "o " << RESET;
                            }
                            else if(lat[width*i+j].species==forb)
                            {
                                cout<< GREEN << "o " << RESET;
                            }
                            else if(lat[width*i+j].species==grass)
                            {
                                cout<< BLUE << "o "<< RESET;
                            }
                            else if(lat[width*i+j].isDeveloped())
                            {
                                cout<< BLACK << "o " <<RESET; 
                            }
                        }
                        cout<<endl;;
                    }
                    cout<<endl<<endl<<endl;
}

void Lattice::printCSVLattice()
{
    ofstream CSVMatrices;
    CSVMatrices.open("CSVMatrices.csv", std::ios_base::app);
    for(int i =0;i<width;i++)
    {
        for(int j=0;j<width;j++)
        {
            if(lat[width*i+j].species==empty)
            {
                CSVMatrices<<lat[width*i+j].species<<",";
            }
            else if(lat[width*i+j].species==parasite)
            {
                CSVMatrices<< lat[width*i+j].species<<",";
            }
            else if(lat[width*i+j].species==forb)
            {
                CSVMatrices<< lat[width*i+j].species<<",";
            }
            else if(lat[width*i+j].species==grass)
            {
                CSVMatrices<< lat[width*i+j].species<<",";
            }
            else if(lat[width*i+j].isDeveloped())
            {
                CSVMatrices<< lat[width*i+j].species<<",";
            }
        }
        CSVMatrices<<endl;;
    }
    CSVMatrices<<endl<<endl<<endl;
    CSVMatrices.close();
    
    
}