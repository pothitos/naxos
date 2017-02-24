// Part of https://github.com/pothitos/naxos

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <naxos.h>
#include <sstream>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                int N = (argc > 1) ? atoi(argv[1]) : 8;
                NsProblemManager pm;
                NsIntVarArray Var, VarPlus, VarMinus;
                for (int i = 0; i < N; ++i) {
                        Var.push_back(NsIntVar(pm, 0, N - 1));
                        VarPlus.push_back(Var[i] + i);
                        VarMinus.push_back(Var[i] - i);
                }
                pm.add(NsAllDiff(Var));
                pm.add(NsAllDiff(VarPlus));
                pm.add(NsAllDiff(VarMinus));
                // MapExplore specific code follows //
                srand(time(0));
                double maxSplitTime, splitTime, simulationRatio;
                if (!(argc == 5 && istringstream(argv[2]) >> maxSplitTime &&
                      istringstream(argv[3]) >> splitTime &&
                      istringstream(argv[4]) >> simulationRatio)) {
                        cerr << "Usage: " << argv[0] << " <N> <max_split_time> "
                                                        "<split_time> "
                                                        "<simulation_ratio>\n";
                        return 1;
                }
                while (pm.readSplit()) {
                        pm.timeLimit(maxSplitTime);
                        pm.addGoal(new NsgLabeling(Var));
                        while (pm.nextSolution() != false) {
                                for (int i = 0; i < N; ++i) {
                                        cout << Var[i].value();
                                        if (i < N - 1)
                                                cout << " ";
                                }
                                cout << "\n";
                        }
                        pm.simulate(splitTime, simulationRatio);
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
                return 1;
        }
}
