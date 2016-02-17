//
//  latticemain.cpp
//  Lattice_AMPS
//
//  Created by Oliver Spiro on 2/6/16.
//  Copyright © 2016 Oliver Spiro. All rights reserved.
//

#include <iostream>
#include <functional>
#include <random>
#include <fstream>
#include <chrono>
#include <thread>

#define _USE_MATH_DEFINES
#include <cmath>


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

#define empty 0
#define producer 1
#define resistant 2
#define susceptible 3
#define wall 4

#define NotOutOfBounds(i,j) ((i > 0) && (i<width) && (j>0) && (j<width))

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;





mt19937 crand;

mt19937 lrand;

mt19937 r_rand;

mt19937 t_rand;

mt19937 p_rand;

const double pi = std::acos(-1);

int *initGrid(int width,double prob_empty,double prob_prod,double prob_res,double prob_susc)
{
    
    mt19937 mt_rand;
    mt_rand.seed(static_cast<unsigned int>(time(NULL)));
    std::uniform_real_distribution<double> realDist(0,1);
    
    double *randmatrix = new double[width*width];
    int *outlattice = new int[width*width];
    
    for(int i = 0;i<width;i++)
    {
        for(int j = 0; j< width; j++)
        {
            randmatrix[width*i+j] = realDist(mt_rand);
    
        }
    }
    
    for(int i = 0;i<width;i++)
    {
        for(int j = 0; j< width; j++)
        {
            if( i==0 || j==0 || i==width-1 ||j==width-1 )
            {
                outlattice[width*i+j]=wall;
            }
            
            else if(randmatrix[width*i+j]<prob_empty) ////TODO: rewrite all these as else ifs again
            {
                outlattice[width*i+j]=empty;
            }
            else if(randmatrix[width*i+j]>=prob_empty && randmatrix[width*i+j]< prob_prod+prob_empty)
            {
                outlattice[width*i+j] = producer;
            }
            else if(randmatrix[width*i+j]>=prob_prod+prob_empty && randmatrix[width*i+j]<prob_res+prob_prod+prob_empty) //superfluous and statement probably
            {
                outlattice[width*i+j]= resistant;
            }
            else if(randmatrix[width*i+j]>=prob_res+prob_prod+prob_empty)
            {
                outlattice[width*i+j]=susceptible;
            }
        }
        
    }
    
    return outlattice;
}

int *newState(int *inlattice, int width, double time_step, double death_rate, double birth_rate[4], double col_strength, double dispersal_radius)
{
    int step_i[4] = {0,1,0,-1};
    int step_j[4] = {-1,0,1,0};
    
    //mersenne twister random num gen, for use later
    
    std::uniform_real_distribution<double> changeRand(0,1); //perhaps generate matrix of random numbers later if will improve performance
    
    std::uniform_int_distribution<double> locationRand(0,width-1);
    
    std::uniform_real_distribution<double> radRand(0,dispersal_radius);
    
    std::uniform_real_distribution<double> thetaRand(0,2*M_PI);
    
    std::uniform_real_distribution<double> unif(0,1);
    
    int *lattice = new int[width*width];
    
    for(int i=0;i<width*width;i++)
    {
        lattice[i] = inlattice[i];
    }
    delete[] inlattice;
    
    //TODO should I generate two rand? one for selection and one for prob of change.
    for(int count=0;count<width*width;count++)
    {
        double r = changeRand(crand); // random num to determine what happens to cell selected in next line
        int i = locationRand(lrand);
        int j = locationRand(lrand); //pick a random location in the lattice for the change to occur
        double radius_rand = radRand(r_rand);
        //double theta_rand = thetaRand(t_rand);
        
        
        if((lattice[width*i+j] != empty) && (lattice[width*i+j] != wall))// if cell is occupied, proceed
        {
            
            double true_deathrate = death_rate; //this is just death_rate, unless the cell at i,j is susceptible, then it is modified by presence of producers
            
            
            if(lattice[width*i+j]==susceptible)
            {
                for(int k = 0;k<4;k++)
                {
                
                    int adj_i = i + step_i[k];
                    int adj_j = j + step_j[k];
                    
                    if(NotOutOfBounds(adj_i, adj_j) && (lattice[width*adj_i+adj_j] == producer))//notoutofbounds--> (adj_i > 0 && adj_i < width && adj_j > 0 && adj_j <width)
                    {
                        true_deathrate = true_deathrate+(col_strength/4);
                    }
                    
                }
                
            }
            
            
            
            
            if(r < true_deathrate*time_step)
            {
                lattice[width*i+j]=empty; //cell death
            }
            if((r>=true_deathrate*time_step) && (r< (true_deathrate+birth_rate[lattice[width*i+j]])*time_step)) //due to the numbers lining up birth_rate[lattice[width*i+j]] is the birth rate of the species in that cell
            {
                double a = unif(p_rand);
                double b = unif(p_rand);
                int targ_i = round(i + b*radius_rand*sin(2*M_PI*(a/b)));
                int targ_j = round(j + b*radius_rand*cos(2*M_PI*(a/b)));
                
                if(NotOutOfBounds(targ_i, targ_j) && (lattice[width*targ_i+targ_j]== empty))
                {
                    lattice[width*targ_i+targ_j] = lattice[width*i+j];
                }
                
            }
            
        
        }
        
        
        
    }//for
    return lattice;
}//newState

double *population(int *lattice,int width)
{
    double emp_count = 0;
    double prod_count = 0;
    double res_count = 0;
    double susc_count = 0;
    for(int i =0;i<width*width;i++)
    {
        if(lattice[i]==empty)
        {
            emp_count++;
        }
            
        else if(lattice[i]==producer)
        {
            prod_count++;
        }
        else if(lattice[i]==resistant)
        {
            res_count++;
        }
        else if(lattice[i]==susceptible)
        {
            susc_count++;
        }
    }
    double *ratios = new double[4];
    ratios[empty] = 100*emp_count/(double(width)*double(width));
    ratios[producer] = 100*prod_count/(double(width)*double(width));
    ratios[resistant] = 100*res_count/(double(width)*double(width));
    ratios[susceptible] = 100*susc_count/(double(width)*double(width));
    return ratios;

}

int main(int argc, char** argv)
{
//    clock_t start;
//    start = clock();
//    double duration;
    
    
    double time_step = 0.09;
    double birth_rate[4];
    birth_rate[producer] = 3;
    birth_rate[resistant] = 3.4;
    birth_rate[susceptible] = 4;
    double death_rate = 0.9;
    
    
    
    double dispersal_radius = atof(argv[1]);
    double col_strength = atof(argv[2]);
    
    double prob_empty = 0.4;
    double prob_prod = 0.2;
    double prob_res = 0.2;
    double prob_susc = 0.2;
    
    
    
    
    int num_timesteps = atoi(argv[3]);
    
    int arg = atoi(argv[4]);
    int *lat = initGrid(arg,prob_empty, prob_prod,prob_res,prob_susc);


    crand.seed(static_cast<unsigned int>(time(NULL)));
    
    lrand.seed(static_cast<unsigned int>(time(NULL)));
    
    r_rand.seed(static_cast<unsigned int>(time(NULL)));
    
    t_rand.seed(static_cast<unsigned int>(time(NULL)));
    
    p_rand.seed(static_cast<unsigned int>(time(NULL)));
    
    
    
    
    

    
    for(int step = 0;step<num_timesteps+1;step++)
    {
        
//        //sleep_for(std::chrono::microseconds(100));
//        if(step%10==0)
//        {
//        
//        sleep_for(std::chrono::microseconds(100));
//        system("clear");
//            for(int i =0;i<arg;i++)
//            {
//                for(int j=0;j<arg;j++)
//                {
//                    if(lat[arg*i+j]==empty)
//                    {
//                        cout<<WHITE<<"o "<<RESET;
//                    }
//                    else if(lat[arg*i+j]==producer)
//                    {
//                        cout<< RED << "o " << RESET;
//                    }
//                    else if(lat[arg*i+j]==resistant)
//                    {
//                        cout<< GREEN << "o " << RESET;
//                    }
//                    else if(lat[arg*i+j]==susceptible)
//                    {
//                        cout<< BLUE << "o "<< RESET;
//                    }
//                    else
//                    {
//                        cout<<"o ";
//                    }
//                }
//                cout<<endl;;
//            }
//            cout<<endl<<endl<<endl;
//            duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
            
//            cout<<"printf: "<< duration <<'\n';

//         }
//
//        double *pop_count = population(lat,arg); //uncomment this and the next 5 lines to get population count. Best to comment out the output loop above too.
//        cout<<step<<endl;
//        cout<<"Empty: "<<pop_count[empty]<<endl;
//        cout<<"Producer: "<<pop_count[producer]<<endl;
//        cout<<"Resistant: "<<pop_count[resistant]<<endl;
//        cout<<"Susceptible: "<<pop_count[susceptible]<<endl<<endl;
        lat = newState(lat,arg,time_step,death_rate,birth_rate, col_strength,dispersal_radius);
    }
    double *pop_count = population(lat,arg);
    ofstream record; //uncomment this and the next three lines to get record.csv
    record.open("record.csv", std::ios_base::app);
    record<<dispersal_radius<<","<<col_strength<<","<<pop_count[empty]<<","<<pop_count[producer]<<","<<pop_count[resistant]<<","<<pop_count[susceptible]<<endl;
    record.close();

    return 0;

}//main
