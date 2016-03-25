#include "Lattice.hpp"
#include "Site.hpp"
#include <iostream>


using namespace std;


int main()
{
    
    double birthRate[4] = {0,4,3.4,3};
    int seedRadius = 10;
    double toxinStrength = 4;
    double startingProb[4] = {0.25,0.25,0.25,0.25};
    int setYearLength = 4;
    double setdt = 0.01
    //double deathRate = 1;
    Lattice myLattice(100,startingProb,birthRate,seedRadius, setdt, toxinStrength, setYearLength );
    for(int i = 0;i<10;i++)
    {
    myLattice.advanceYear();
    }
    
    
    

  

}