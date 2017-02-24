// Part of https://github.com/pothitos/naxos

#include "strtok.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <naxos.h>
#include <string>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                //  Checking input arguments.
                if (argc != 3 && argc != 4) {
                        cerr << "Correct syntax is:\t" << argv[0]
                             << " <input_filename> <N> [precision]\n";
                        return 1;
                }
                //  Passing the number of nodes.
                unsigned N = atoi(argv[2]);
                unsigned precision = ((argc == 4) ? atoi(argv[3]) : 1);
                //  Opening input filename, e.g. `tsp_data.pl'.
                ifstream file(argv[1]);
                if (!file) {
                        cerr << "Could not open `" << argv[1] << "'\n";
                        return 1;
                }
                string input, tok;
                NsDeque<NsDeque<NsInt>> edge_weight;
                //  Reading input file line by line.
                while (getline(file, input)) {
                        StrTokenizer tokens(input, "([,]");
                        tok = tokens.next();
                        if (tok == "costs" || tok == "       ") {
                                edge_weight.push_back(NsDeque<NsInt>());
                                while (!(tok = tokens.next()).empty() &&
                                       tok != ").")
                                        edge_weight.back().push_back(
                                            atof(tok.c_str()) * precision +
                                            0.5);
                        } else {
                                cerr << "Unknown predicate `" << tok << "'\n";
                                return 1;
                        }
                }
                file.close();
                //  Add a row for the last node.
                edge_weight.push_back(NsDeque<NsInt>());
                if (N < 1 || N > edge_weight.size()) {
                        cerr << "Wrong N\n";
                        return 1;
                }
                //  Delete first redundant nodes due to argument N.
                unsigned i, j;
                unsigned delete_first_nodes = edge_weight.size() - N;
                for (i = 0; i < delete_first_nodes; ++i)
                        edge_weight.pop_front();
                //  Make the diagonal array symmetric.
                for (i = 0; i < edge_weight.size(); ++i) {
                        edge_weight[i].push_front(0);
                        for (j = 0; j < i; ++j)
                                edge_weight[i].push_front(
                                    edge_weight[i - j - 1][i]);
                }
                //  Print problem representation.
                /*for (i=0;  i < N;  ++i)   {
                        for (j=0;  j < N;  ++j)   {
                                cout << "edge_weight[" << i << "][" << j << "] =
                "
                                        << edge_weight[i][j] << "\n";
                        }
                        cout << "\n";
                }*/
                /**   ADD YOUR CODE HERE   **/
                NsProblemManager pm;
                NsIntVarArray R;
                R.push_back(NsIntVar(pm, 0, 0));
                for (i = 1; i < N; ++i)
                        R.push_back(NsIntVar(pm, 1, N - 1));
                pm.add(NsAllDiff(R));
                //  The two-dimensional array (edge_weight)
                //   is made one-dimensional (edge_weight_onedim).
                NsDeque<NsInt> edge_weight_onedim;
                for (i = 0; i < N; ++i) {
                        for (j = 0; j < N; ++j)
                                edge_weight_onedim.push_back(edge_weight[i][j]);
                }
                //  The cost for going from the node A to the
                //   node B is edge_weight_onedim[A*N + B].
                //  We impose the `element' constraint using the
                //  edge_weight_onedim.
                NsIntVarArray vWeights;
                for (i = 0; i < N - 1; ++i)
                        vWeights.push_back(
                            edge_weight_onedim[R[i] * N + R[i + 1]]);
                vWeights.push_back(edge_weight_onedim[R[i] * N + R[0]]);
                //  The sum of all the weights is the solution cost.
                NsIntVar vCost = NsSum(vWeights);
                pm.minimize(vCost);
                pm.addGoal(new NsgLabeling(R));
                NsDeque<NsInt> bestR(N);
                while (pm.nextSolution() != false) {
                        //  Recording the (current) best solution.
                        for (i = 0; i < N; ++i)
                                bestR[i] = R[i].value();
                }
                ////  Printing the best solution.
                // if ( bestCost  !=  -1 )    {
                //	cout << "R = [";
                //	for (i=0;  i < N;  ++i)  {
                //		if ( i  >  0 )
                //			cout << ", ";
                //		cout << bestR[i] + 1;
                //	}
                //	cout << "]\n";
                //	cout << "C = " << bestCost << "\n";
                //}
                pm.printCspParameters();
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
