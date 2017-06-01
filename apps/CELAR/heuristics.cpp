// Part of https://github.com/pothitos/naxos

#include "heuristics.h"
#include <cmath>

using namespace naxos;
using namespace std;

unsigned unboundVars(const NsIntVarArray& Vars)
{
        unsigned numUnbound = 0;
        for (NsIndex i = 0; i < Vars.size(); ++i)
                numUnbound += (!Vars[i].isBound());
        return numUnbound;
}

int intervalBinarySearch(const NsDeque<double>& intervalStart, const double x);

NsIndex randomizeHeuristic(NsDeque<double>& heuristic, const double conf)
{
        double sumHeuristic;
        NsIndex i;
        sumHeuristic = 0.0;
        for (i = 1; i < heuristic.size(); ++i)
                sumHeuristic += heuristic[i];
        for (i = 1; i < heuristic.size(); ++i) {
                heuristic[i] = pow(heuristic[i] / sumHeuristic, conf);
                assert_that(heuristic[i] >= 0.0, "Negative `heuristic[i]'");
        }
        // [0,1) is partitioned into intervals corresponding to values.
        sumHeuristic = 0.0;
        for (i = 1; i < heuristic.size(); ++i)
                sumHeuristic += heuristic[i];
        i = 0;
        heuristic[i] = 0.0;
        ++i;
        for (; i < heuristic.size(); ++i) {
                heuristic[i] = heuristic[i] / sumHeuristic + heuristic[i - 1];
                //  `varIndex[i]' is represented by the interval
                //   [heuristic[i-1],heuristic[i]].  The greater the
                //   heuristic function value is, the larger the interval is.
        }
        // A random number in [0,1) multiplied by heuristic[i-1].
        double random = (rand() / (RAND_MAX + 1.0)) * heuristic[i - 1];
        //  To which interval (corresponding to a specific varIndex) does
        //   `random' belong to?  (We search it using binary search.)
        return intervalBinarySearch(heuristic, random);
}

///  Returns the i, with x belonging to [intervalStart[i],intervalStart[i+1]).
int intervalBinarySearch(const NsDeque<double>& intervalStart, const double x)
{
        NsIndex i;
        NsIndex low = 0;
        NsIndex high = intervalStart.size() - 2;
        for (;;) {
                i = (low + high) / 2;
                if (intervalStart[i] <= x && x < intervalStart[i + 1])
                        break;
                if (x < intervalStart[i])
                        high = i;
                else
                        low = i + 1;
                assert_that(low <= high,
                            "intervalBinarySearch: Bad `intervalStart'");
        }
        return i;
}

//  VARIABLE ORDERING HEURISTIC  //
int VarHeurCelar::select(const NsIntVarArray& Vars)
{
        int bestIndex = -1;
        NsIndex i, currentDegree, maxDegree = 0;
        if (conf < 0) {
                for (i = 0; i < Vars.size(); ++i) {
                        if (!Vars[i].isBound() &&
                            (bestIndex == -1 ||
                             (Vars[i].size() < Vars[bestIndex].size()) ||
                             (Vars[i].size() == Vars[bestIndex].size() &&
                              (currentDegree = unboundVars(
                                   info.varsConnected[i])) > maxDegree))) {
                                bestIndex = i;
                                maxDegree = unboundVars(info.varsConnected[i]);
                        }
                }
        } else {
                NsDeque<unsigned> varsDegree(Vars.size());
                unsigned numUnbound = 0, maxDomainSize = 0;
                for (i = 0; i < Vars.size(); ++i) {
                        numUnbound += (!Vars[i].isBound());
                        if (Vars[i].size() > maxDomainSize)
                                maxDomainSize = Vars[i].size();
                        varsDegree[i] = unboundVars(info.varsConnected[i]);
                        if (varsDegree[i] > maxDegree)
                                maxDegree = varsDegree[i];
                }
                if (numUnbound == 0)
                        return -1;
                NsDeque<NsIndex> varIndex(numUnbound);
                NsDeque<double> heuristic(varIndex.size() + 1);
                unsigned valNum = 0;
                for (i = 0; i < Vars.size(); ++i) {
                        if (!Vars[i].isBound()) {
                                varIndex[valNum] = i;
                                //  MRV / DEGREE heuristic  //
                                heuristic[valNum + 1] =
                                    (maxDomainSize - Vars[i].size()) *
                                        (maxDegree + 1) +
                                    varsDegree[i] + 1;
                                ++valNum;
                        }
                }
                bestIndex = varIndex[randomizeHeuristic(heuristic, conf)];
        }
        return bestIndex;
}

//  VALUE ORDERING HEURISTIC  //
NsInt ValHeurCelar::select(const NsIntVar& V)
{
        return V.min();
        NsInt bestValue;
        /*  Searching to find the index of `V' in the array `Vars'.  */
        NsIndex index;
        for (index = 0; &Vars[index] != &V; ++index)
                /*VOID*/;
        NsDeque<NsInt> value;
        NsDeque<double> heuristic;
        if (conf >= 0) {
                value.resize(Vars[index].size());
                heuristic.resize(value.size() + 1);
        }
        /*  Evaluation of all the variable values  */
        NsIndex valNum = 0;
        for (NsIntVar::const_iterator val = Vars[index].begin();
             val != Vars[index].end(); ++val, ++valNum) {
                value[valNum] = *val;
        }
        if (conf >= 0)
                bestValue = value[randomizeHeuristic(heuristic, conf)];
        return bestValue;
}
