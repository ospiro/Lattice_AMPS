//
//  main.cpp
//  Lattice_AMPS
//
//  Created by Oliver Spiro on 2/6/16.
//  Copyright © 2016 Oliver Spiro. All rights reserved.
//

#include <iostream>
#include <functional>
#include <random>
using namespace std;

//int *initGrid(int width,double prob_empty, double prob_prod, double prob_res, double prob_susc);

//int *newState(int *lattice, int width, int time_step, double death_rate, double birth_rate_prod, double birth_rate_res, double birth_rate_susc, double col_strength, double move_rate);
int *initGrid(int width,double prob_empty,double prob_prod,double prob_res,double prob_susc)
{
    
    mt19937 mt_rand;
    mt_rand.seed(time(NULL));
    std::uniform_real_distribution<double> realDist(0,1);
    
    double *randmatrix = new double[width*width];
    int *outlattice = new int[width*width];
    
    for(int i = 0;i<width*width;i++)
    {
        randmatrix[i] = realDist(mt_rand);
    }
    for(int i = 0;i<width*width;i++)
    {
        if(randmatrix[i]<prob_empty) ////TODO: rewrite all these as else ifs again
        {
            outlattice[i]=0;
        }
        if(randmatrix[i]>=prob_empty && randmatrix[i]< prob_prod+prob_empty)
        {
            outlattice[i] = 1;
        }
        if(randmatrix[i]>=prob_prod+prob_empty && randmatrix[i]<prob_res+prob_prod+prob_empty)
        {
            outlattice[i]=2;
        }
        if(randmatrix[i]>=prob_res+prob_prod+prob_empty)
        {
            outlattice[i]=3;
        }
    }
    
    return outlattice;
}


int *newState(int *inlattice, int width, double time_step, double death_rate, double birth_rate_prod, double birth_rate_res, double  birth_rate_susc, double col_strength, double move_rate)
{
    //mersenne twister random num gen, for use later
    int step_i[4] = {0,1,0,-1};
    int step_j[4] = {-1,0,1,0}; //ASK: why are these different arrays?
    mt19937 crand;
    crand.seed(time(NULL));
    std::uniform_real_distribution<double> changeRand(0,1);
    
    mt19937 lrand;
    lrand.seed(time(NULL));
    std::uniform_int_distribution<int> locationRand(0,width-1);
    
    mt19937 mrand;
    mrand.seed(time(NULL));
    std::uniform_int_distribution<int> moveRand(0,2);
    
    int *lattice = new int[width*width];
    
    for(int i=0;i<width*width;i++)
    {
        lattice[i] = inlattice[i];
    }
    delete[] inlattice;
    
    //TODO should I generate two rand? one for selection and one for prob of change.
    for(int count=0;count<width*width-1;count++)
    {
        int frac_emp = 0;
        int frac_prod = 0;
        int frac_res = 0;
        int frac_susc = 0;
        int top;
        int bot;
        int left;
        int right;
        
        int i = locationRand(lrand), j = locationRand(lrand); //pick a random location in the lattice for the change to occur
        
        //now that we have chosen the cell, we calculate the fractions of adjacent cells occupied by Prod, Susc, Res and Emp cells
        
        //NOTE: I use i,j matrix notation. So 0,0 is the top left corner.
        
        
        //////////////////////////////////////////  special case, periodic boundaries
        
        if(i==0)
        {
            top = lattice[width*(width-1)+j];
        }
        else if(i==width-1)
        {
            bot = lattice[j];
        }
        if(j==0)
        {
            left = lattice[width*i+width-1];
        }
        if(j==width-1)
        {
            right = lattice[width*i];
        }
        
        
        
        if( i!=0 && i!=(width-1) && j!=0 && j!= (width-1)) //TODO: set newwidth=width-1 to avoid all the arithmetic, TODO: rewrite if as nested switch/case
        {
            top = lattice[width*(i-1)+j];
            bot = lattice[width*(i+1)+j];
            left = lattice[width*i+(j-1)];
            right = lattice[width*i+(j+1)];
        }
        
        
        int neigh[4] = {top,bot,left,right};
        
        ///create the fractions f_i, TODO: For efficiency, replace with Mark's way if needed.
        
        for(int k = 0;k<4;k++)
        {
            switch(neigh[k])
            {
                case 0:
                    frac_emp+=1/4;
                    break;
                case 1:
                    frac_prod+=1/4;
                case 2:
                    frac_res+=1/4;
                case 3:
                    frac_susc+=1/4;
                    
            }
            
        }
        double r = changeRand(crand);
        
        ///////////////////////  CASE: CELL Empty--->chance cell born    ///////////////////////
        
        if(lattice[width*i+j]==0)
        {
            if( r < birth_rate_prod*frac_prod*time_step )
            {
                lattice[width*i+j]=1;
            }
            else if( r < birth_rate_prod*frac_prod*time_step + birth_rate_res*frac_res*time_step )
            {
                lattice[width*i+j]=2;
            }
            else if( r < birth_rate_prod*frac_prod*time_step + birth_rate_res*frac_res*time_step + birth_rate_susc*frac_susc+time_step )
            {
                lattice[width*i+j]=3;
            }
        }
        
        //////////////////// CASE: Cell is producer, may die or may swap with neighbor. //////////////////
        
        if(lattice[width*i+j]==1)
        {
            if (r < death_rate*time_step)
            {
                lattice[width*i+j]=0;
            }
            else if(r < (death_rate+move_rate)*time_step && i!=0 && i!=(width-1) && j!=0 && j!= (width-1) )
            {
                int randstep = moveRand(mrand);
                int new_i = i+step_i[randstep];
                int new_j = j+step_j[randstep];
                int temp = lattice[width*new_i+new_j];
                lattice[width*new_i+new_j] = lattice[width*i+j];
                lattice[width*i+j] = temp;
            }
        }
        
        //////////////////Case: Cell is resistant, same as above
        if(lattice[width*i+j]==2)
        {
            if (r < death_rate*time_step)
            {
                lattice[width*i+j]=0;
            }
            else if(r < (death_rate+move_rate)*time_step && i!=0 && i!=(width-1) && j!=0 && j!= (width-1))
            {
                int randstep = moveRand(mrand);
                int new_i = i+step_i[randstep];
                int new_j = j+step_j[randstep];
                int temp = lattice[width*new_i+new_j];
                lattice[width*new_i+new_j] = lattice[width*i+j];
                lattice[width*i+j] = temp;
            }
        }
        /////////////////Case: cell is susceptible, same as above with addition of colicin effect//////////////
        if(lattice[width*i+j]==3)
        {
            if (r < (death_rate+col_strength)*time_step)
            {
                lattice[width*i+j]=0;
            }
            else if(r < (death_rate+move_rate)*time_step && i!=0 && i!=(width-1) && j!=0 && j!= (width-1))
            {
                int randstep = moveRand(mrand);
                int new_i = i+step_i[randstep];
                int new_j = j+step_j[randstep];
                int temp = lattice[width*new_i+new_j];
                lattice[width*new_i+new_j] = lattice[width*i+j];
                lattice[width*i+j] = temp;
            }
        }
    }
    return lattice;
}

int main(int argc, char** argv)
{
    //remove these later start
    double time_step = 0.09;
    //int width = 500;
    double birth_rate_prod = 3;
    double birth_rate_res = 3.4;
    double birth_rate_susc = 4;
    double death_rate = 1;
    double move_rate = 5; //make sense of these values
    double col_strength = 4;
    
    double prob_empty = 0.4;
    double prob_prod = 0.2;
    double prob_res = 0.2;
    double prob_susc = 0.2;
    
    
    int arg = atoi(argv[1]);
    int *lat = initGrid(arg,prob_empty, prob_prod,prob_res,prob_susc);
    
    for(int step = 1;step<1000;step++)
    {
        if(step%10==0)
        {
            system("clear");
            for(int i =0;i<arg;i++)
            {
                for(int j=0;j<arg;j++)
                {
                    cout<<lat[arg*i+j];
                }
                cout<<endl;
            }
            cout<<endl<<endl;
        }
        lat = newState(lat,arg,time_step,death_rate,birth_rate_prod,birth_rate_res,birth_rate_susc,col_strength,move_rate);
    }
}//main
