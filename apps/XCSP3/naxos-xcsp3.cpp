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

NsIntVarArray* VarSolution;

void printSolution(void)
{
        if (VarSolution == 0) {
                cout << "s UNSATISFIABLE\n";
        } else {
                cout << "v <instantiation>\n"
                     << "v   <list> x[] </list>\n"
                     << "v   <values> " << *VarSolution << " </values>\n"
                     << "v </instantiation>\n";
        }
        exit(0);
}

void signalHandler(int /*signum*/)
{
        printSolution();
}

int main(int argc, char* argv[])
{
        try {
                VarSolution = 0;
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
                NsIntVar* vObjectivePointer = 0;
                pm.addGoal(new NsgLabeling(Var));
                cout << "c Created " << pm.numVars() << " variables and "
                     << pm.numConstraints()
                     << " constraints, including intermediates\n";
                while (pm.nextSolution() != false) {
                        VarSolution = &Var;
                        if (vObjectivePointer == 0) {
                                signal(SIGINT, SIG_IGN); // Ignore SIGINT
                                cout << "s SATISFIABLE\n";
                                printSolution();
                        } else {
                                cout << "o " << vObjectivePointer->value()
                                     << endl;
                        }
                }
                signal(SIGINT, SIG_IGN); // Ignore SIGINT
                if (VarSolution != 0)
                        cout << "s OPTIMUM FOUND\n";
                printSolution();
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception\n";
                return 1;
        }
}
