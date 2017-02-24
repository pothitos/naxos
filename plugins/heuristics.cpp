#include "heuristics.h"
#include <iostream>
#include <naxos.h>

using namespace std;
using namespace naxos;

/* VARIABLE ORDERING HEURISTICS */

// first unbound variable
int VarHeurFirst::select(const NsIntVarArray& Vars)
{
        for (NsIndex i = 0; i < Vars.size(); ++i)
                if (!Vars[i].isBound())
                        return i;
        return -1;
}

// minimum remaining values
int VarHeurMRV::select(const NsIntVarArray& Vars)
{
        int index = -1;
        NsUInt minDom = NsUPLUS_INF;
        for (NsIndex i = 0; i < Vars.size(); ++i) {
                if (!Vars[i].isBound() && Vars[i].size() < minDom) {
                        minDom = Vars[i].size();
                        index = i;
                }
        }
        return index;
}

// random heuristic
int VarHeurRand::select(const NsIntVarArray& Vars)
{
        int numUnbound = 0, randChoice, index = 0;
        // Number of unbound variables
        for (NsIndex i = 0; i < Vars.size(); ++i) {
                if (!Vars[i].isBound())
                        numUnbound++;
        }
        if (numUnbound == 0)
                return -1;
        // Pick variable among unbound ones
        randChoice = rand() % numUnbound;
        // Find position of chosen variable
        for (NsIndex i = 0; i < Vars.size(); ++i) {
                if (!Vars[i].isBound()) {
                        if (randChoice == index)
                                return (int)i;
                        else
                                ++index;
                }
        }
        cout << "Something went wrong (heuristics.cpp, VarOrderHeurRand)"
             << endl;
        return 0; // to suppress g++ warning
}

/***************
int VarHeurDegree::select(const NsIntVarArray& Vars)	// Maximum Degree
{
}
*/

/* VALUE ORDERING HEURISTICS */

// minimum available value
NsInt ValHeurFirst::select(const NsIntVar& V)
{
        return V.min();
}

// random heuristic
NsInt ValHeurRand::select(const NsIntVar& V)
{
        int size = (int)V.size();
        int position = rand() % size;
        NsInt value = V.min();
        for (int i = 0; i < position; i++)
                value = V.next(value);
        return value;
}
