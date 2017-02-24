#include "dfs.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmDfsLabeling::GOAL(void)
{
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (new NsgAND(new AmDfsInDomain(Vars[index], valHeur),
                           new AmDfsLabeling(*this)));
}

NsGoal* AmDfsInDomain::GOAL(void)
{
        if (Var.isBound())
                return 0;
        NsInt value = valHeur->select(Var);
        return (new NsgOR(new NsgSetValue(Var, value),
                          new NsgAND(new NsgRemoveValue(Var, value),
                                     new AmDfsInDomain(*this))));
}
