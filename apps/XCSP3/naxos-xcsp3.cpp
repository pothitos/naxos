/// @file
/// Links the XCSP3 parser to Naxos solver
///
/// Part of https://github.com/pothitos/naxos

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <naxos.h>

using namespace naxos;
using namespace std;

void signalHandler(int /*signum*/)
{
        exit(0);
}

int main(int argc, char* argv[])
{
        try {
                // Register signal SIGINT and its signal handler
                signal(SIGINT, signalHandler);
                // Parse the input filename command line argument
                if (argc != 2) {
                        cerr << "Usage: " << argv[0] << " BENCHNAME\n";
                        return 1;
                }
                // State the Constraint Satisfaction Problem
                int N = 8;
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
                pm.addGoal(new NsgLabeling(Var));
                while (pm.nextSolution() != false)
                        cout << Var << "\n";
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception\n";
                return 1;
        }
}
