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
Xcsp3_to_Naxos* solver;

void printSolutionAndExit(void)
{
        if (solver->solutionIsRecorded()) {
                // A solution has been found
                // Status SATISFIABLE or OPTIMUM FOUND has been already printed
                solver->printSolution();
        } else {
                // No solution found and search hasn't been completed
                cout << "s UNKNOWN\n";
        }
        exit(0);
}

void interruptionHandler(int /*signum*/)
{
        if (searching) {
                // The interruption happened inside pm.nextSolution()
                if (solver->solutionIsRecorded())
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
                        cerr << "Naxos Solver 1.1.0\n";
                        cerr << "Usage: " << argv[0] << " BENCHNAME\n";
                        return 1;
                }
                // Interface between the parser and the solver
                Xcsp3_to_Naxos callbacks(verbose);
                solver = &callbacks;
                XCSP3Core::XCSP3CoreParser parser(solver);
                parser.parse(argv[1]);
                // Initialize variables for search
                interrupted = false;
                searching = true;
                // Register interruption signal handler
                signal(SIGTERM, interruptionHandler);
                while (solver->nextSolution() != false) {
                        searching = false;
                        solver->recordSolution();
                        if (solver->constrainedOptimization()) {
                                cout << "o " << solver->getObjectiveValue()
                                     << endl;
                        } else {
                                if (solver->solutionIsRecorded())
                                        cout << "s SATISFIABLE\n";
                                printSolutionAndExit();
                                // Non-optimization search finishes here
                        }
                        searching = true;
                        // Check if interrupted while in critical area
                        if (interrupted)
                                interruptionHandler(SIGTERM); // resume function
                }
                // Search has been completed
                searching = false;
                if (solver->solutionIsRecorded()) {
                        cout << "s OPTIMUM FOUND\n";
                        printSolutionAndExit();
                } else {
                        cout << "s UNSATISFIABLE\n";
                }
        } catch (exception& exc) {
                cout << "s UNSUPPORTED\n";
                cout << "c " << exc.what() << "\n";
                return 1;
        } catch (...) {
                cout << "s UNSUPPORTED\n";
                cout << "c Unknown exception\n";
                return 1;
        }
}
