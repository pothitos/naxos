#include <naxos.h>
#include <iostream>
#include "heuristics.h"

using namespace std;
using namespace naxos;


/* VARIABLE ORDERING HEURISTICS */



int VarHeurFirst::select(const NsIntVarArray& Vars)	// first unbound variable
{
    for (NsIndex i = 0; i < Vars.size(); ++i)
        if ( !Vars[i].isBound() )
			return i;

    return -1;
}

int VarHeurMRV::select(const NsIntVarArray& Vars)	// minimum remaining values
{
    int  index = -1;
    NsUInt  minDom = NsUPLUS_INF;
    for (NsIndex i = 0;   i < Vars.size();   ++i)  {
        if ( !Vars[i].isBound()   &&   Vars[i].size() < minDom )   {
            minDom = Vars[i].size();
            index = i;
        }
    }
    return  index;
}


int VarHeurRand::select(const NsIntVarArray& Vars)    // random heuristic
{
	int numUnbound = 0, randChoice, index=0;

	// Number of unbound variables
    for (NsIndex i = 0;   i < Vars.size();   ++i)  {
        if ( !Vars[i].isBound() )
            numUnbound++;
    }
	if (numUnbound == 0)
		return -1;

	// Pick variable among unbound ones
	randChoice = rand() % numUnbound;

	// Find position of chosen variable
    for (NsIndex i = 0;   i < Vars.size();   ++i)  {
        if ( !Vars[i].isBound() ){
			if (randChoice == index)
				return (int)i;
			else
				++index;
		}
    }

	cout << "Something went wrong (heuristics.cpp, VarOrderHeurRand)" << endl;
	return 0;	// to suppress g++ warning
}

/***************
int VarHeurDegree::select(const NsIntVarArray& Vars)	// Maximum Degree
{
}
*/




/* VALUE ORDERING HEURISTICS */



NsInt ValHeurFirst::select(const NsIntVar& V)	// minimum available value
{
    return  V.min();
}

NsInt ValHeurRand::select(const NsIntVar& V)	// random heuristic
{
	int size = (int)V.size();
	int position = rand()%size;

    NsInt value = V.min();
	for(int i=0;i<position;i++)
		value=V.next(value);

	return value;
}

