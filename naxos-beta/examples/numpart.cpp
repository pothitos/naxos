/*
 * File:   main.cpp
 * Author: peter
 *
 * Created on May 26, 2013, 5:34 PM
 */

#include <cstdlib>
#include "naxos.h"

using namespace naxos;
using namespace std;

/*
 *
 */

void ListPrint(NsIntVarArray& List)
{
        NsIntVarArray::iterator it = List.begin();
        if (0 != List.size()) {
                cout << it->value();
                ++it;
                for (; it != List.end(); ++it) {
                        cout << " " << it->value();
                }
        }
        cout << " / ";
}

void ListPrintRest(NsIntVarArray& List)
{
        NsIntVarArray::iterator it = List.begin();
        long CurNum = it->value()-1;
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

int main(int argc, char **argv)
{
	if (argc != 2) {
		cerr << argv[0] << ": Please provide <N> as argument\n";
		return 1;
	}
        int N = atoi(argv[1]);
        NsProblemManager pm;
        NsIntVarArray List, ListSquare;
        if (0 != N % 4 || 0 > N)
		return 0;
        List.push_back(NsIntVar(pm, N, N));
        ListSquare.push_back(NsIntVar(pm, N*N, N*N));
        for (int i = 1; N/2 > i; ++i) {
                List.push_back(NsIntVar(pm, N/2 - i, N - i));
                ListSquare.push_back(List[i]*List[i]);
        }
        pm.add(NsAllDiff(List));
        Constraint(pm, List);
        NsIntVar Sum = NsSum(List), SquareSum = NsSum(ListSquare);
        pm.add(Sum == N*(N+1)/4);
        pm.add(SquareSum == N*(N+1)*(2*N+1)/12);
        pm.addGoal(new NsgLabeling(List));
        while (pm.nextSolution() != false) {
                ListPrint(List);
                ListPrintRest(List);
        }
        return 0;
}
