#include "Lattice.hpp"
#include "Site.hpp"
#include <iostream>


using namespace std;


int main()
{
    
    double birthRate[4] = {0,4,3.4,3};
    int seedRange[2] = {2,4};
    int seedRadius = 10;
    double toxinStrength = 4;
    double startingProb[4] = {0.25,0.25,0.25,0.25};
    double deathRate = 1; //
    Lattice myLattice(10,startingProb,birthRate,deathRate,seedRange,seedRadius,toxinStrength,1,0.01);//remove deathRate here, keep in site class
    Lattice(int setWidth, double prob[4], double setBirthRate[4], double setDeathRate, int setSeedRange[2], int setSeedRadius, double setDeathrate, int radius,double dt, double setToxinStrength, int setYearLength)
    //Lattice(int setWidth, double prob[4], double setBirthRate[4], double setDeathRate, int setSeedRange[2], int setSeedRadius, double setStrength, int radius);

    
//        for (int i = 0 ; i<10; i++)
//        {
//            for (int j = 0 ; j<10 ; j++)
//            {
//                cout<<myLattice.point(i,j);
//            }
//            cout<<endl;
//        }
//
    
    myLattice.printLattice();

    
//    typedef Cell* Cellptr;
//    Cellptr *lattice = new Cellptr[100];
//    
//    for(int i = 0; i < 100; i++)
//    {
//        lattice[i] = new Cell();
//    }
//    cout<<lattice[0]->getType();

//        for (int i = 0 ; i<10; i++)
//        {
//            for (int j = 0 ; j<10 ; j++)
//            {
//                cout<<lattice[10*i+j]->getType();
//            }
//        }
//    Lattice lattice(10,0.4,0.2,0.2,0.2);
//    lattice.printLattice();
}