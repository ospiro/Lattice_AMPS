//
//  Cell.hpp
//  Lattice_AMPS
//
//  Created by Oliver Spiro on 3/15/16.
//  Copyright © 2016 Oliver Spiro. All rights reserved.
//
//
#ifndef Site_hpp
#define Site_hpp



#include <iostream>
#include <string>
#include <random>
using namespace std;

class Site
{
public:
    Site();
    int numSeedsHere();
    int numSeeds();
    bool hasSeeds();
    int species;
    int seeds[4] = {0,0,0,0};
    int seedRange[2] = {2,4};
    
    Site* left;
    Site* right;
    Site* top;
    Site* bottom;
    Site* neighbors[4];
    
    void die(){species=0;}
    double getDeathRate();
    void sproutSeeds();
    void plant(int s);
    void setNeighbors();
    double deathRate;
    double toxinStrength;
    mt19937 mt_rand;
    std::uniform_real_distribution<double> unif;
    
      
};




#endif /* Site_hpp */
