
//  Naxos Solver:  A Constraint Programming Library         //
//  Copyright © 2007-2015  Nikolaos Pothitos                //
//  See ../../license/LICENSE for the license of the library.  //

#include <naxos.h>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace naxos;

int  main (int argc, char *argv[])
{
	try {
		int  N = (argc > 1) ? atoi(argv[1]) : 8;
		NsProblemManager  pm;
		NsIntVarArray  Var, VarPlus, VarMinus;
		for (int i=0;  i<N;  ++i) {
			Var.push_back( NsIntVar(pm, 0, N-1) );
			VarPlus.push_back(  Var[i] + i );
			VarMinus.push_back( Var[i] - i );
		}
		pm.add( NsAllDiff(Var) );
		pm.add( NsAllDiff(VarPlus) );
		pm.add( NsAllDiff(VarMinus) );
		if ( argc  >  2 ) {
			// SPLIT //
			pm.realTimeLimit(20);
			pm.splitTimeLimit(CLOCKS_PER_SEC/100.0);
			pm.addGoal( new NsgLabeling(Var) );
			while (pm.nextSolution() != false)
				/*VOID*/ ;
			cout << "\n";
		} else {
			// READ //
			while ( pm.readSplit() ) {
				pm.addGoal( new NsgLabeling(Var) );
				while (pm.nextSolution() != false)
					cout << "Solution: " << Var << "\n";
				pm.restart();
			}
		}
	} catch (exception& exc) {
		cerr << exc.what() << "\n";
	} catch (...) {
		cerr << "Unknown exception" << "\n";
	}
}
