#include "Lattice.hpp"
#include "Site.hpp"
#include <iostream>
#include <fstream>
#include <random>
#define empty 0
#define parasite 1
#define forb 2
#define grass 3

using namespace std;


int main(int argc, char** argv)
{
    
    int width  = atoi(argv[1]); // first arg
    
    double startingProb[4] = {0.25,0.25,0.25,0.25};
    
    double birthRate[4] = {0,0,0,0};
    birthRate[parasite] = 3;
    birthRate[forb] = 3.4;
    birthRate[grass] = 4;
    
    double setDeathRate = 1;
    
    int seedRadius = atoi(argv[2]); //second arg
    double toxinStrength = atof(argv[3]);//third arg
    double amountDevelopment = atof(argv[4]);//fourth arg
    
    int numYearsRun = atoi(argv[5]); //fifth arg
    int setYearLength = 4;
    
    double setdt = 0.01;
    
    int devType = atoi(argv[6]);
    
    int numberofseeds = atoi(argv[7]);
    
    if( amountDevelopment >= 0.8*width*width)
    {
        cout<<"amountDevelopment too high, process terminated."<<endl;
        exit(1);
    }
    
    Lattice myLattice(width,startingProb,birthRate,seedRadius, setDeathRate, setdt, toxinStrength, setYearLength, amountDevelopment, devType,numberofseeds);


    int yearOfDeath = 0;
    for( int i = 1; i <= numYearsRun;i++)
    {
//        myLattice.printLattice(); //comment out this line and uncomment lines 55-58 (inclusive) to return to normal tracking behavior
        myLattice.advanceYear();
        if (myLattice.checkExtinction()==true)
        {
            yearOfDeath = i;
            break;
        }
    }
    ofstream record; //uncomment this and the next three lines to get csv record
    record.open(strcat(argv[8],".csv"));//, std::ios_base::app);
    record<<width<<","<<seedRadius<<","<<toxinStrength<<","<<amountDevelopment<<","<<numberofseeds<<","<<yearOfDeath<<endl;
    record.close();

    
}