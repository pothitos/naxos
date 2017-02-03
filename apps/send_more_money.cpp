// Part of https://github.com/pothitos/naxos

#include <iostream>
#include <naxos.h>

using namespace std;
using namespace naxos;

int main(void)
{
        try {
                NsProblemManager pm;
                NsIntVar S(pm, 1, 9), E(pm, 0, 9), N(pm, 0, 9), D(pm, 0, 9),
                         M(pm, 1, 9), O(pm, 0, 9), R(pm, 0, 9), Y(pm, 0, 9);
                NsIntVar  send =             1000 * S + 100 * E + 10 * N + D;
                NsIntVar  more =             1000 * M + 100 * O + 10 * R + E;
                NsIntVar money = 10000 * M + 1000 * O + 100 * N + 10 * E + Y;
                pm.add(send + more == money);
                NsIntVarArray letters;
                letters.push_back(S);
                letters.push_back(E);
                letters.push_back(N);
                letters.push_back(D);
                letters.push_back(M);
                letters.push_back(O);
                letters.push_back(R);
                letters.push_back(Y);
                pm.add(NsAllDiff(letters));
                pm.addGoal(new NsgLabeling(letters));
                if (pm.nextSolution() != false) {
                        cout << "    " << send.value() << "\n"
                             << " +  " << more.value() << "\n"
                             << " = " << money.value() << "\n";
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception\n";
        }
}
