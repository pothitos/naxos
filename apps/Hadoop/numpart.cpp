// Part of https://github.com/pothitos/naxos

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <naxos.h>
#include <sstream>

using namespace naxos;
using namespace std;

void ListPrint(NsIntVarArray& List)
{
        NsIntVarArray::iterator it = List.begin();
        if (0 != List.size()) {
                cout << it->value();
                ++it;
                for (; it != List.end(); ++it)
                        cout << " " << it->value();
        }
        cout << "\n";
}

void ListPrintRest(NsIntVarArray& List)
{
        NsIntVarArray::iterator it = List.begin();
        long CurNum = it->value() - 1;
        bool flag = false;
        ++it;
        while (1) {
                while (CurNum != it->value()) {
                        if (false == flag) {
                                cout << CurNum;
                                flag = true;
                        } else {
                                cout << " " << CurNum;
                        }
                        CurNum--;
                }
                ++it;
                CurNum--;
                if (it == List.end()) {
                        while (0 != CurNum) {
                                if (false == flag) {
                                        cout << CurNum;
                                        flag = true;
                                } else {
                                        cout << " " << CurNum;
                                }
                                CurNum--;
                        }
                        break;
                }
        }
        cout << "\n";
}

void Constraint(NsProblemManager& pm, NsIntVarArray& List)
{
        NsIntVarArray::iterator it1 = List.begin();
        NsIntVarArray::iterator it2 = List.begin();
        ++it2;
        while (List.end() != it2) {
                pm.add(*it1 > *it2);
                ++it1;
                ++it2;
        }
}

int main(int argc, char** argv)
{
        try {
                if (argc < 2) {
                        cerr << argv[0] << ": Please provide <N> as argument\n";
                        return 1;
                }
                int N = atoi(argv[1]);
                NsProblemManager pm;
                NsIntVarArray List, ListSquare;
                if (0 != N % 4 || 0 > N)
                        return 0;
                List.push_back(NsIntVar(pm, N, N));
                ListSquare.push_back(NsIntVar(pm, N * N, N * N));
                for (int i = 1; N / 2 > i; ++i) {
                        List.push_back(NsIntVar(pm, N / 2 - i, N - i));
                        ListSquare.push_back(List[i] * List[i]);
                }
                pm.add(NsAllDiff(List));
                Constraint(pm, List);
                NsIntVar Sum = NsSum(List), SquareSum = NsSum(ListSquare);
                pm.add(Sum == N * (N + 1) / 4);
                pm.add(SquareSum == N * (N + 1) * (2 * N + 1) / 12);
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
                        pm.addGoal(new NsgLabeling(List));
                        while (pm.nextSolution() != false)
                                ListPrint(List);
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
