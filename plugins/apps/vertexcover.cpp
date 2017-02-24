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
                int k = 0;
                while (1) { // is there a vertex cover C with |C|=k ?
                        NsProblemManager pm;
                        // PROBLEM STATEMENT //
                        NsIntVarArray Nodes; // 1 if v \in C, 0 otherwise
                        for (i = 0; i < N; ++i)
                                Nodes.push_back(NsIntVar(pm, 0, 1));
                        for (i = 0; i < N; ++i)
                                for (j = 0; j < i; ++j)
                                        if (graph[i][j])
                                                pm.add(Nodes[i] == 1 ||
                                                       Nodes[j] == 1);
                        pm.add(NsSum(Nodes) == k);
                        // GOALS //
                        pm.addGoal(new AmDfsLabeling(Nodes));
                        // SOLVING //
                        if (pm.nextSolution()) {
                                cout << "A vertex cover of cardinality " << k;
                                cout << " was found for the following graph:"
                                     << endl
                                     << endl;
                                for (i = 0; i < N; ++i) {
                                        for (j = 0; j <= i; ++j)
                                                cout << graph[i][j];
                                        cout << endl;
                                }
                                cout << endl
                                     << "It consists of the following nodes:";
                                for (i = 0; i < N; ++i)
                                        if (Nodes[i].value())
                                                cout << " " << i;
                                cout << endl;
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
