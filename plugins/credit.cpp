#include "credit.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmCredit::GOAL(void)
{
        if (credit == 0) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0;          // all variables are bound => success
        NsIntVar& V = Vars[index]; // renaming for readability
        if (V.isBound())
                return 0;
        NsInt value = valHeur->select(V);
        //	int   crPortion = (int)ceil((double)credit / (double)V.size());
        int crPortion = 1 + (credit - 1) / V.size();
        int crRemain = credit - crPortion;
        return (new NsgOR(
            new NsgAND(new NsgSetValue(V, value),
                       new AmCredit(Vars, crPortion, varHeur, valHeur)),
            new NsgAND(new NsgRemoveValue(V, value),
                       new AmCredit(Vars, crRemain, varHeur, valHeur))));
}
