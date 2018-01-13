#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

/*************************
 *
 *         ? ? ?
 *       x ? ? ?
 *     ---------
 *         ? ? ?
 *       ? ? ?
 *     ? ? ?
 *     ---------
 *     ? ? ? ? ?
 *
 *************************/

/* where every digit is used twice */

int main()
{
        try {
                NsProblemManager pm;
                NsIntVar x1(pm, 0, 19), x2(pm, 0, 19), x3(pm, 0, 19),
                    y1(pm, 0, 19), y2(pm, 0, 19), y3(pm, 0, 19), a1(pm, 0, 19),
                    a2(pm, 0, 19), a3(pm, 0, 19), b1(pm, 0, 19), b2(pm, 0, 19),
                    b3(pm, 0, 19), c1(pm, 0, 19), c2(pm, 0, 19), c3(pm, 0, 19),
                    o1(pm, 0, 19), o2(pm, 0, 19), o3(pm, 0, 19), o4(pm, 0, 19),
                    o5(pm, 0, 19);
                // each digit can appear exactly twice,
                // either as itself or as itself+10.
                NsIntVar x = 100 * (x1 % 10) + 10 * (x2 % 10) + x3 % 10;
                NsIntVar y = 100 * (y1 % 10) + 10 * (y2 % 10) + y3 % 10;
                NsIntVar a = 100 * (a1 % 10) + 10 * (a2 % 10) + a3 % 10;
                NsIntVar b = 100 * (b1 % 10) + 10 * (b2 % 10) + b3 % 10;
                NsIntVar c = 100 * (c1 % 10) + 10 * (c2 % 10) + c3 % 10;
                NsIntVar o = 10000 * (o1 % 10) + 1000 * (o2 % 10) +
                             100 * (o3 % 10) + 10 * (o4 % 10) + o5 % 10;
                pm.add(x * (y3 % 10) == a);
                pm.add(x * (y2 % 10) == b);
                pm.add(x * (y1 % 10) == c);
                pm.add(a + 10 * b + 100 * c == o);
                pm.add(x1 != 0);
                pm.add(y1 != 0);
                pm.add(a1 != 0);
                pm.add(b1 != 0);
                pm.add(c1 != 0);
                pm.add(o1 != 0);
                NsIntVarArray Vars;
                Vars.push_back(x1);
                Vars.push_back(x2);
                Vars.push_back(x3);
                Vars.push_back(y1);
                Vars.push_back(y2);
                Vars.push_back(y3);
                Vars.push_back(a1);
                Vars.push_back(a2);
                Vars.push_back(a3);
                Vars.push_back(b1);
                Vars.push_back(b2);
                Vars.push_back(b3);
                Vars.push_back(c1);
                Vars.push_back(c2);
                Vars.push_back(c3);
                Vars.push_back(o1);
                Vars.push_back(o2);
                Vars.push_back(o3);
                Vars.push_back(o4);
                Vars.push_back(o5);
                pm.add(NsAllDiff(Vars));
                // GOALS //
                pm.addGoal(new AmDfsLabeling(Vars));
                // SOLVING //
                if (pm.nextSolution() != false) {
                        cout << "Solution: " << endl;
                        cout << "  " << x.value() << endl;
                        cout << " x" << y.value() << endl;
                        cout << "-----" << endl;
                        cout << "  " << a.value() << endl;
                        cout << " " << b.value() << endl;
                        cout << c.value() << endl;
                        cout << "-----" << endl;
                        cout << o.value() << endl;
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
