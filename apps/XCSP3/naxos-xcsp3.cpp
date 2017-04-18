/// @file
/// Links the XCSP3 parser to Naxos solver
///
/// Part of https://github.com/pothitos/naxos

#include "translator.h"
#include <XCSP3CoreParser.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace naxos;
using namespace std;

bool searching;
bool interrupted;
NsIntVarArray* VarSolution;

void printSolutionAndExit(void)
{
        if (VarSolution == 0) {
                // No solution found and search hasn't been completed
                cout << "s UNKNOWN\n";
        } else {
                // A solution has been found
                // Status SATISFIABLE or OPTIMUM FOUND has been already printed
                cout << "v <instantiation>\n"
                     << "v   <list> x[] </list>\n"
                     << "v   <values> " << *VarSolution << " </values>\n"
                     << "v </instantiation>\n";
        }
        exit(0);
}

void interruptionHandler(int /*signum*/)
{
        if (searching) {
                // The interruption happened inside pm.nextSolution()
                if (VarSolution != 0)
                        cout << "s SATISFIABLE\n";
                printSolutionAndExit();
        } else {
                // Flag interruption, but continue execution
                interrupted = true;
        }
}

int main(int argc, char* argv[])
{
        try {
                // Check if the verbose argument '-v' exists
                bool verbose = false;
                if (argc == 3 && string(argv[2]) == "-v") {
                        verbose = true;
                } else if (argc != 2) {
                        cerr << "Usage: " << argv[0] << " BENCHNAME\n";
                        return 1;
                }
                // Interface between the parser and the solver
                Xcsp3_to_Naxos callbacks(verbose);
                XCSP3Core::XCSP3CoreParser parser(&callbacks);
                parser.parse(argv[1]);
                // State the Constraint Satisfaction Problem
                NsProblemManager pm;
                NsIntVarArray Var;
                for (int i = 0; i < 3; ++i)
                        Var.push_back(NsIntVar(pm, 1, 3));
                pm.add(NsAllDiff(Var));
                NsIntVar* vObjectivePointer = 0;
                // vObjectivePointer = &Var[2];
                // pm.minimize(*vObjectivePointer);
                pm.addGoal(new NsgLabeling(Var));
                if (verbose) {
                        cout << "c Created " << pm.numVars()
                             << " variables and " << pm.numConstraints()
                             << " constraints, including intermediates\n";
                }
                VarSolution = 0;
                interrupted = false;
                searching = true;
                // Register interruption signal handler
                signal(SIGINT, interruptionHandler);
                while (pm.nextSolution() != false) {
                        searching = false;
                        // Record solution
                        VarSolution = &Var;
                        if (vObjectivePointer == 0) {
                                cout << "s SATISFIABLE\n";
                                printSolutionAndExit();
                                // Non-optimization search finishes here
                        } else {
                                cout << "o " << vObjectivePointer->value()
                                     << endl;
                        }
                        searching = true;
                        // Check if interrupted while in critical area
                        if (interrupted)
                                interruptionHandler(SIGINT); // resume function
                }
                // Search has been completed
                searching = false;
                if (VarSolution == 0) {
                        cout << "s UNSATISFIABLE\n";
                } else {
                        cout << "s OPTIMUM FOUND\n";
                        printSolutionAndExit();
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception\n";
                return 1;
        }
}
