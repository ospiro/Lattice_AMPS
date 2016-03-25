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

#define empty 0
#define parasite 1
#define forb 2
#define grass 3
using namespace std;

int mod(int x, int m) { //got from http://stackoverflow.com/questions/1082917/mod-of-negative-number-is-melting-my-brain need because % doesn't behave well with negative #s
    return (x%m + m)%m;
}

Lattice::Lattice(int setWidth, double prob[4], double setBirthRate[4], double setDeathRate, int setSeedRadius, int radius,double dt, double setToxinStrength, int setYearLength)
{
//===============================Set base properties==============================================
    
    width = setWidth;
    lat.resize(width*width); //technically one dimensional
    for(int i = 0;i<width*width;i++)
    {
        lat[i].toxinStrength = setToxinStrength;
        lat[i].deathRate = setDeathRate;
    }
    
    std::copy(setBirthRate,setBirthRate+4,birthRate);
    seedRadius = setSeedRadius;
    yearLength = setYearLength;
    
    //seed the generators and set bounds for the uniform distributions
    matrix_rand.seed(static_cast<unsigned int>(time(NULL)));
    event_rand.seed(static_cast<unsigned int>(time(NULL)));
    loc_rand.seed(static_cast<unsigned int>(time(NULL)));
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
        randmatrix.push_back(unif(matrix_rand)); //TODO: use randmatrix.reserve
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

    

}


//=================================Check if the site's species runs or dies================================


void Lattice::checkEvent(int i, int j) //TODO: make this and getDeathRate take Site params instead of coordinates
{
    double trueDeathRate;
    if (lat[width*i+j].species != empty)
    {
        trueDeathRate = lat[width*i+j].getDeathRate();
    }
    
    
    double rand = unifEvent(event_rand);
    int rand2 = unifRun(run_rand);
    if(rand<trueDeathRate*dt)
    {
        lat[width*i+j].die();//TODO: assign this element to a variable, then use variable in rest of function.( assign to pointer?)
    }
    else if ((lat[width*i+j].neighbors[rand2]->species==empty) && rand<(trueDeathRate+birthRate[lat[width*i+j].species])*dt) //TODO: convoluted, fix using the above assignment
    {
        lat[width*i+j].neighbors[rand2]->species=lat[width*i+j].species;
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
        double thetarand = unif(event_rand);
        double theta = thetarand*2*3.14159;
            
        double distancerand = unif(event_rand);
        double distance = floor(distancerand); //TODO: ask: is the +1 so that radius==0 corresp to running?
        int targI = mod(int(round(double(i)+distance*cos(theta))),width); //TODO: check behavior, possible problem area
        int targJ = mod(int(round(double(j)+distance*sin(theta))),width);
        //TODO: ask why conditions for targI==0 were here in matlab code.
            
        //targI,targJ designate target site for seed dispersal, now we add seeds.
            
            
            lat[width*targI+targJ].plant(lat[width*i+j].species);
            
        }
    
    }
    
    
    
    for(int k = 0; k< width*width;k++)
    {
            lat[k].die(); //TODO: ask if necessary ...//kill all plants
            lat[k].sproutSeeds();
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

void Lattice::population()
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
  
    cout<<"Empty: "<<ratios[empty]<<endl;
    cout<<"Para: "<<ratios[parasite]<<endl;
    cout<<"Forb: "<<ratios[forb]<<endl;
    cout<<"Grass: "<<ratios[grass]<<endl<<endl;
    

    
}



//===================================Self explanatory==========================================


void Lattice::printLattice()
{
    for (int i = 0 ; i<width; i++)
    {
        for (int j = 0 ; j<width ; j++)
        {
            cout<<lat[width*i+j].species;
        }
        cout<<endl;
    }
    cout<<endl<<endl;
}