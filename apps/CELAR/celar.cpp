// Part of https://github.com/pothitos/naxos

#include "heuristics.h"
#include <amorgos.h>
#include <naxos.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                time_t timeBegin = time(0);
                NsProblemManager pm;
                // For normal usage the time limit should be 900 seconds.
                pm.realTimeLimit(60);
                if (argc != 2 && argc != 3 && argc != 6) {
                        cerr << argv[0] << ": correct syntax is: " << argv[0]
                             << " scen_directory [conf|hadoop "
                                "<hadoop_options>]\n";
                        return 1;
                }
                CelarInfo info;
                bool hadoop = false;
                double conf = -1;
                if (argc == 3) {
                        if (string(argv[2]) == "hadoop") {
                                hadoop = true;
                        } else {
                                istringstream conf_argument(argv[2]);
                                if (!(conf_argument >> conf)) {
                                        cerr << argv[0]
                                             << ": Wrong conf number `"
                                             << argv[2] << "'!\n";
                                        return 1;
                                }
                        }
                }
                ifstream file;
                file.open((string(argv[1]) + "/cst.txt").c_str());
                if (!file) {
                        cerr << argv[0] << ": could not open `" << argv[1]
                             << "/cst.txt'!\n";
                        return 1;
                }
                string str;
                while (file >> str && str != "a1")
                        /* VOID */;
                if (str != "a1") {
                        cerr << argv[1] << "/cst.txt: Missing `a1'!\n";
                        return 1;
                }
                NsIndex i, j;
                NsDeque<NsInt> a(8);
                for (i = 0; i < a.size(); ++i) {
                        if ((i != 0 && !(file >> str)) || !(file >> str) ||
                            str != "=" || !(file >> a[i])) {
                                cerr << argv[1]
                                     << "/cst.txt: Missing a_i/b_i!\n";
                                return 1;
                        }
                }
                NsDeque<NsInt> b(4);
                for (i = 0; i < b.size(); ++i)
                        b[i] = a[i + 4];
                a.resize(4);
                file.close();
                file.open((string(argv[1]) + "/dom.txt").c_str());
                if (!file) {
                        cerr << argv[0] << ": could not open `" << argv[1]
                             << "/dom.txt'!\n";
                        return 1;
                }
                NsIndex index;
                NsIndex cardinality;
                NsDeque<NsDeque<NsInt>> domains;
                while (file >> index) {
                        if (!(file >> cardinality)) {
                                cerr << argv[1] << "/dom.txt: " << index
                                     << ": Missing cardinality!\n";
                                return 1;
                        }
                        domains.push_back(NsDeque<NsInt>(cardinality));
                        for (i = 0; i < cardinality; ++i) {
                                if (!(file >> domains[index][i])) {
                                        cerr << argv[1] << "/dom.txt: " << index
                                             << ": Missing domain value!\n";
                                        return 1;
                                }
                        }
                }
                file.close();
                NsDeque<NsDeque<NsInt>> domainsPrevious(domains.size()),
                    domainsNext(domains.size());
                NsIntVarArray AllVars, vObjectiveTerms;
                NsDeque<NsIndex> indexToVar;
                NsIndex varIndex, varIndexY, varDomain, cost;
                NsInt varInitial, difference;
                file.open((string(argv[1]) + "/var.txt").c_str());
                if (!file) {
                        cerr << argv[0] << ": could not open `" << argv[1]
                             << "/var.txt'!\n";
                        return 1;
                }
                while (getline(file, str)) {
                        istringstream line(str);
                        if (!(line >> varIndex >> varDomain)) {
                                cerr << argv[1] << "/var.txt: Syntax error!\n";
                                return 1;
                        }
                        for (i = indexToVar.size(); i < varIndex; ++i)
                                indexToVar.push_back(indexToVar.max_size());
                        indexToVar.push_back(AllVars.size());
                        if (line >> varInitial >> cost) {
                                if (cost == 0) {
                                        if (find(domains[varDomain].begin(),
                                                 domains[varDomain].end(),
                                                 varInitial) ==
                                            domains[varDomain].end()) {
                                                cerr
                                                    << argv[1]
                                                    << "/var.txt: Initial "
                                                       "value not in domain!\n";
                                                return 1;
                                        }
                                        AllVars.push_back(NsIntVar(
                                            pm, varInitial, varInitial));
                                } else {
                                        --cost;
                                        AllVars.push_back(NsInDomain(
                                            pm, domains[varDomain],
                                            domainsPrevious[varDomain],
                                            domainsNext[varDomain]));
                                        vObjectiveTerms.push_back(
                                            (AllVars.back() != varInitial) *
                                            b[cost]);
                                }
                        } else {
                                AllVars.push_back(
                                    NsInDomain(pm, domains[varDomain],
                                               domainsPrevious[varDomain],
                                               domainsNext[varDomain]));
                        }
                }
                file.close();
                info.varsConnected.resize(AllVars.size());
                file.open((string(argv[1]) + "/ctr.txt").c_str());
                if (!file) {
                        cerr << argv[0] << ": could not open `" << argv[1]
                             << "/ctr.txt'!\n";
                        return 1;
                }
                while (file >> varIndex >> varIndexY >> str >> str >>
                       difference >> cost) {
                        if (str != "=" && str != ">") {
                                cerr << argv[1] << "/ctr.txt: Invalid operand `"
                                     << str << "'!\n";
                                return 1;
                        }
                        i = indexToVar[varIndex];
                        j = indexToVar[varIndexY];
                        info.varsConnected[i].push_back(AllVars[j]);
                        if (cost == 0) {
                                if (str == "=") {
                                        pm.add(NsAbs(AllVars[i] - AllVars[j]) ==
                                               difference);
                                } else {
                                        pm.add(NsAbs(AllVars[i] - AllVars[j]) >
                                               difference);
                                }
                        } else {
                                --cost;
                                if (str == "=") {
                                        vObjectiveTerms.push_back(
                                            (NsAbs(AllVars[i] - AllVars[j]) !=
                                             difference) *
                                            a[cost]);
                                } else {
                                        vObjectiveTerms.push_back(
                                            (NsAbs(AllVars[i] - AllVars[j]) <=
                                             difference) *
                                            a[cost]);
                                }
                        }
                }
                file.close();
                NsIntVar vObjective = NsSum(vObjectiveTerms);
                pm.minimize(vObjective);
                VarHeurCelar varHeur(info, conf);
                ValHeurCelar valHeur(AllVars, info, conf);
                NsDeque<NsInt> bestAllVars(AllVars.size());
                if (!hadoop) {
                        pm.addGoal(
                            new AmDfsLabeling(AllVars, &varHeur, &valHeur));
                        NsInt bestObjective = -1;
                        double bestTime = -1;
                        while (pm.nextSolution() != false) {
                                bestTime = difftime(time(0), timeBegin);
                                bestObjective = vObjective.value();
                                for (i = 0; i < bestAllVars.size(); ++i)
                                        bestAllVars[i] = AllVars[i].value();
                        }
                        if (bestObjective != -1) {
                                cout << bestTime << "\t" << bestObjective
                                     << "\t";
                                pm.printCspParameters();
                                ofstream fileSolution(
                                    (string(argv[1]) + "/sol.txt").c_str());
                                for (i = 0; i < indexToVar.size(); ++i)
                                        if (indexToVar[i] !=
                                            indexToVar.max_size())
                                                fileSolution
                                                    << i << "\t"
                                                    << bestAllVars
                                                           [indexToVar[i]]
                                                    << "\n";
                                fileSolution.close();
                        }
                } else {
                        // MapExplore specific code follows //
                        srand(time(0));
                        double maxSplitTime, splitTime, simulationRatio;
                        if (!(argc == 6 &&
                              istringstream(argv[3]) >> maxSplitTime &&
                              istringstream(argv[4]) >> splitTime &&
                              istringstream(argv[5]) >> simulationRatio)) {
                                cerr << "Usage: " << argv[0]
                                     << " <N> <max_split_time> <split_time> "
                                        "<simulation_ratio>\n";
                                return 1;
                        }
                        while (pm.readSplit()) {
                                pm.timeLimit(maxSplitTime);
                                pm.addGoal(new AmDfsLabeling(AllVars, &varHeur,
                                                             &valHeur));
                                NsInt bestObjective = -1;
                                while (pm.nextSolution() != false) {
                                        bestObjective = vObjective.value();
                                        for (i = 0; i < bestAllVars.size(); ++i)
                                                bestAllVars[i] =
                                                    AllVars[i].value();
                                }
                                if (bestObjective != -1) {
                                        cout << bestObjective << "\n";
                                        pm.restart();
                                        // We restart and disable optimization
                                        // in order to have a well-defined
                                        // reference search tree.
                                }
                                pm.simulate(splitTime, simulationRatio);
                        }
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
