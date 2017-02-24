#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

void printSolution(NsIntVarArray* sol, int m, int n, int groupSize)
{
        int i, j, count;
        for (i = 0; i < m; ++i) {
                count = 0;
                for (j = 0; j < n; ++j) {
                        cout << sol[i][j];
                        count++;
                        if (count == groupSize) {
                                cout << " ";
                                count = 0;
                        }
                }
                cout << endl;
        }
}

int main(int argc, char* argv[])
{
        try {
                unsigned int i, j, k, l;
                unsigned int nDays, groupSize, nGroups, nGolfers;
                if (argc < 4) {
                        nDays = 5;
                        groupSize = 4;
                        nGroups = 5;
                } else {
                        nDays = atoi(argv[1]);
                        groupSize = atoi(argv[2]);
                        nGroups = atoi(argv[3]);
                }
                nGolfers = groupSize * nGroups;
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray *varRows, pairsFirst, pairsSecond, pairs;
                varRows = new NsIntVarArray[nDays];
                // Number of pairs:
                // (combinations of groupSize over 2) times nDays times nGroups
                // int nPairs = nDays*nGroups * (groupSize*(groupSize-1)) / 2 ;
                for (i = 0; i < nDays; ++i)
                        for (j = 0; j < nGolfers; ++j)
                                varRows[i].push_back(NsIntVar(pm, 1, nGolfers));
                // Every golfer in same day is different
                for (i = 0; i < nDays; ++i)
                        pm.add(NsAllDiff(varRows[i]));
                for (i = 0; i < nDays; ++i)
                        for (j = 0; j < nGroups; ++j)
                                for (k = 0; k < groupSize - 1; ++k)
                                        for (l = groupSize - 1; l > k; --l) {
                                                // order variables in each group
                                                pm.add(
                                                    varRows[i]
                                                           [j * groupSize + k] <
                                                    varRows[i]
                                                           [j * groupSize + l]);
                                                // save pairs
                                                //						pairsFirst.push_back(
                                                //varRows[i][j*groupSize+k] );
                                                //						pairsSecond.push_back(
                                                //varRows[i][j*groupSize+l] );
                                                pairs.push_back(
                                                    varRows[i]
                                                           [j * groupSize + k] *
                                                        nGolfers +
                                                    varRows[i]
                                                           [j * groupSize + l]);
                                        }
                // Order groups in each day (for symmetry breaking)
                for (i = 0; i < nDays; ++i)
                        for (j = 1; j < nGroups; ++j)
                                pm.add(varRows[i][(j - 1) * groupSize] <
                                       varRows[i][j * groupSize]);
                // Order days
                for (i = 1; i < nDays; ++i)
                        pm.add(varRows[i - 1][0] <= varRows[i][0]);
                /*
                                // No two same pairs
                                for (i=0; i<pairsFirst.size(); ++i)
                                        for (j=0; j<pairsFirst.size(); ++j)
                                                if (i!=j)
                                                        pm.add( pairsFirst[i] !=
                pairsFirst[j] || pairsSecond[i] != pairsSecond[j] );
                //					pm.add(
                pairsFirst[i]*nGolfers + pairsSecond[i] !=
                //
                pairsFirst[j]*nGolfers + pairsSecond[j] );
                */
                pm.add(NsAllDiff(pairs));
                // all variables
                NsIntVarArray Vars;
                for (i = 0; i < nDays; ++i)
                        for (j = 0; j < nGolfers; ++j)
                                Vars.push_back(varRows[i][j]);
                // GOALS //
                pm.addGoal(new AmDfsLabeling(Vars));
                // SOLVING //
                if (pm.nextSolution() != false)
                        printSolution(varRows, nDays, nGolfers, groupSize);
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
