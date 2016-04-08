////
////  Lattice.hpp
////  Lattice_AMPS
////
////  Created by Oliver Spiro on 3/15/16.
////  Copyright © 2016 Oliver Spiro. All rights reserved.
////
//
#ifndef Lattice_hpp
#define Lattice_hpp

#include <string>
#include "Site.hpp"
#include <iostream>
#include <vector>
#include <random>
using namespace std;
class Lattice
{
public:
    Lattice(int setWidth, double prob[4], double setBirthRate[4], int setSeedRadius, double setDeathRate, double dt, double setToxinStrength, int setYearLength, double setAmountDevelopment, int devType);
    void advanceYear();
    void endOfYear();
    void advanceTimeStep();
    void addDevelopment(double amountDevelopment);
    double getDeathRate(int i, int j);
    void checkEvent(int i, int j);
    double* population();
    void printLattice();
    bool checkExtinction();
    void printCSVLattice();
    int devType;
    std::uniform_real_distribution<double> unif;
    mt19937 event_rand;
    std::uniform_real_distribution<double> unifEvent;
    std::uniform_int_distribution<int> unifRun;
    std::uniform_int_distribution<int> unifLoc;//TODO:clean up random number generators
    std::vector<Site> lat; //move back to private after testing
    
    
private:
    int width;
    int numSpecies;
    int birthRate[4];
    double seedRange[2];
    double seedRadius;
    double dt;
    int yearLength;
    double amountDevelopment;

};
#endif /* Lattice_hpp */
