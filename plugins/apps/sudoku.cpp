#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

void printSolution(NsIntVarArray* sol, int dim)
{
        int i, j, k, l;
        for (i = 0; i < dim; ++i) {
                for (j = 0; j < dim; ++j) {
                        for (k = 0; k < dim; ++k) {
                                for (l = 0; l < dim; ++l)
                                        cout << sol[dim * i + j][dim * k + l];
                                cout << " ";
                        }
                        cout << endl;
                }
                cout << endl;
        }
}

int main(int argc, char* argv[])
{
        try {
                int i, j, sqrtn, n;
                sqrtn = (argc > 1) ? atoi(argv[1]) : 3;
                n = sqrtn * sqrtn;
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray *varRows, *varColumns, *varSquares;
                varRows = new NsIntVarArray[n];
                varColumns = new NsIntVarArray[n];
                varSquares = new NsIntVarArray[n];
                for (i = 0; i < n; ++i)
                        for (j = 0; j < n; ++j)
                                varRows[i].push_back(NsIntVar(pm, 1, n));
                for (i = 0; i < n; ++i)
                        for (j = 0; j < n; ++j) {
                                varColumns[i].push_back(varRows[j][i]);
                                varSquares[i].push_back(
                                    varRows[(i / sqrtn) * sqrtn + j / sqrtn]
                                           [(i % sqrtn) * sqrtn + j % sqrtn]);
                        }
                for (i = 0; i < n; ++i) {
                        pm.add(NsAllDiff(varRows[i]));
                        pm.add(NsAllDiff(varColumns[i]));
                        pm.add(NsAllDiff(varSquares[i]));
                }
                NsIntVarArray allVars;
                for (i = 0; i < n; ++i)
                        for (j = 0; j < n; ++j)
                                allVars.push_back(varRows[i][j]);
                // GOALS //
                pm.addGoal(new AmDfsLabeling(allVars));
                // SOLVING //
                while (pm.nextSolution() != false) {
                        printSolution(varRows, sqrtn);
                        getchar();
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
