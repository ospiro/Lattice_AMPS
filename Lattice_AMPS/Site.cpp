//
//  Site.cpp
//  Lattice_AMPS
//
//  Created by Oliver Spiro on 3/15/16.
//  Copyright © 2016 Oliver Spiro. All rights reserved.
//
//

#include "Site.hpp"
#include <string>
#include <random>
#include <iostream>
#define empty 0
#define parasite 1
#define forb 2
#define grass 3
using namespace std;

Site::Site()
{
    species = 0;
    deathRate = 1;
    toxinStrength = 4; //TODO: initialize toxinstrength in L constructor
    mt_rand.seed(static_cast<unsigned int>(time(NULL)));
}

//======Calculate death rate based on neighbors================================

double Site::getDeathRate()
{
    double rate = deathRate;
    if (species == grass)
    {
        for(int i = 0;i<4;i++)
        {
            if (neighbors[i]->species==parasite)
            {
                rate+=1/(toxinStrength);
            }
        }
    }
    return rate;
   
}

//=======How many seeds have been planted here===================================
int Site::numSeeds()
{
    
    if (species!=empty)
    {
        
        double range = seedRange[1]-seedRange[0];
        int n = floor(unif(mt_rand)*range+1+seedRange[1]); //TODO: C++ify
    }
}


//=======sprout a species, based on proportion of planted seeds of that type=====
void Site::sproutSeeds()
{
    std::uniform_real_distribution<double>::param_type newParams(0,1);
    unif.param(newParams);
    double rand = unif(mt_rand);
    double numseeds = double(numSeeds());
    
    if (hasSeeds())
    {
        if(rand < (double(seeds[0])/numseeds))
        {
            species = parasite;
        }
        else if(rand<(double(seeds[0]+seeds[1])/numseeds))
        {
            species = forb;
        }
        else
        {
            species = grass;
        }
    }
    
    //seeds now gone
    for(int i = 0; i < 4 ; i++)
    {
        seeds[i]=0;
    }

}

//=========plant a seed here=====================================================

void Site::plant(int speciesType)
{
    seeds[speciesType]+=1;
}