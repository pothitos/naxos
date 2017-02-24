#include "onesamp.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmOnesampLabeling::GOAL(void)
{
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (new NsgAND(new AmOnesampInDomain(Vars[index], valHeur),
                           new AmOnesampLabeling(*this)));
}

NsGoal* AmOnesampInDomain::GOAL(void)
{
        if (V.isBound())
                return 0;
        NsInt value = valHeur->select(V);
        return (new NsgSetValue(V, value));
}
