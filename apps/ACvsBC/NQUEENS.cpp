  //  Naxos Solver:  A Constraint Programming Library         //
 //  Copyright 2007-2016 Nikolaos Pothitos                  //
//  See ../license/LICENSE for the license of the library.  //

#include <naxos.h>
#include <iostream>
#include <cstdlib>

using namespace naxos;
using namespace std;

int main(int argc, char *argv[])
{
    try {
      int N = (argc > 1)? atoi(argv[1]) : 8;
      NsProblemManager  pm;

      NsIntVarArray  Var, VarPlus, VarMinus;
      for (int i=0;  i<N;  ++i)   {
          Var.push_back( NsIntVar(pm, 0, N-1) );
          VarPlus.push_back(  Var[i] + i );
          VarMinus.push_back( Var[i] - i );
      }
      pm.add( NsAllDiff(Var) );
      pm.add( NsAllDiff(VarPlus) );
      pm.add( NsAllDiff(VarMinus) );

      pm.addGoal(new NsgLabeling(Var));

      //bool  AcMode = true;//( string(argv[0]).find(".AC") != string::npos );

      unsigned solutions=0;
      //if ( AcMode )
      //    cout << N << "\t" << solutions << "\t";
      //cout << difftime(time(0),timeBegin) << "\n";
      while (pm.nextSolution() != false)
              ++solutions
      //cout << difftime(time(0),timeBegin) << "\tX\t";
      //pm.printCspParameters();
    } catch (exception& exc) {
	cerr << exc.what() << "\n";
        return 1;
    } catch (...) {
	cerr << "Unknown exception\n";
        return 1;
    }
}
