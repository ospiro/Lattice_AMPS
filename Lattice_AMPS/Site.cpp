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

void Site::setNeighbors()
{
    neighbors[0] = left;
    neighbors[1] = right;
    neighbors[2] = top;
    neighbors[3] = bottom;
}
double Site::getDeathRate()
{
    double rate = deathRate;
    if (species == grass)
    {
        for(int i = 0;i<4;i++)
        {
            if (neighbors[i]->species==parasite)
            {
                rate+=toxinStrength/4;//TODO: depends on neighborhood definition
            }
        }
    }
    return rate;
   
}

//=======How many seeds are generated===================================
int Site::numSeeds()
{
    int n = 0;
    if (species!=empty)
    {
        
        double range = seedRange[1]-seedRange[0];
        n = int(floor(unif(mt_rand)*range+1+seedRange[1])); //TODO: C++ify
    }
    return n;
}

int Site::numSeedsHere()
{
    int n = 0;
    for(int i = 0; i < 4;i++)
    {
        n+=seeds[i];
    }
    return n;
}

bool Site::hasSeeds()
{
    return (numSeedsHere()>0);
}

//=======sprout a species, based on proportion of planted seeds of that type=====
void Site::sproutSeeds()
{
    std::uniform_real_distribution<double>::param_type newParams(0,1);
    unif.param(newParams);
    double rand = unif(mt_rand);
    double numseedshere = double(numSeedsHere());
    
    if (hasSeeds())
    {
        if(rand < (double(seeds[parasite])/numseedshere))
        {
            species = parasite;
        }
        else if(rand<(double(seeds[parasite]+seeds[forb])/numseedshere))
        {
            species = forb;
        }
        else if(rand<(double(seeds[parasite]+seeds[forb]+seeds[grass])/numseedshere))
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