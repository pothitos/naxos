#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                // number of nodes
                int N = (argc > 1) ? atoi(argv[1]) : 9;
                // probability of edge existence between two nodes (%)
                int Pr = (argc > 2) ? atoi(argv[2]) : 25;
                int i, j;
                // construct a random graph
                srand(time(NULL));
                bool** graph = new bool*[N];
                for (i = 0; i < N; ++i)
                        graph[i] = new bool[N];
                for (i = 0; i < N; ++i)
                        for (j = 0; j <= i; ++j)
                                if (i > j && rand() % 100 <= Pr)
                                        graph[i][j] = true;
                                else
                                        graph[i][j] = false;
                int k = 1;
                while (1) { // is the graph k-colorable?
                        NsProblemManager pm;
                        // PROBLEM STATEMENT //
                        NsIntVarArray Nodes;
                        for (i = 0; i < N; ++i)
                                Nodes.push_back(NsIntVar(pm, 0, k));
                        for (i = 0; i < N; ++i)
                                for (j = 0; j < i; ++j)
                                        if (graph[i][j])
                                                pm.add(Nodes[i] != Nodes[j]);
                        // GOALS //
                        pm.addGoal(new AmDfsLabeling(Nodes));
                        // SOLVING //
                        if (pm.nextSolution()) {
                                cout << "The following graph is " << k + 1
                                     << "-colorable:" << endl;
                                for (i = 0; i < N; ++i) {
                                        for (j = 0; j <= i; ++j)
                                                cout << graph[i][j];
                                        cout << endl;
                                }
                                cout << "Colors: " << endl << Nodes << endl;
                                break;
                        }
                        k++;
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
