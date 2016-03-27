#include "Lattice.hpp"
#include "Site.hpp"
#include <iostream>
#define empty 0
#define parasite 1
#define forb 2
#define grass 3

using namespace std;


int main()
{
    double startingProb[4] = {0.25,0.25,0.25,0.25};
    
    double birthRate[4] = {0,0,0,0};
    birthRate[parasite] = 3;
    birthRate[forb] = 3.4;
    birthRate[grass] = 4;
    
    double setDeathRate = 1;
    
    int seedRadius = 10;
    double toxinStrength = 4;
    int setYearLength = 4;
    
    double setdt = 0.01;
    
    
    Lattice myLattice(50,startingProb,birthRate,seedRadius, setDeathRate, setdt, toxinStrength, setYearLength );
//    for(int i = 0;i<10;i++)
//    {
//    myLattice.advanceYear();
//    }
    for(int i = 0;i< 50*50;i++)
    {
        myLattice.lat[i].die();
    }
    myLattice.printLattice();
    cout<<endl<<endl;
    myLattice.addDevelopment(20);
    myLattice.printLattice();
    
    
    
    
}