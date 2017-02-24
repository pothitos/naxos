#include "ibroad.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmIbroad::GOAL(void)
{
        unsigned i, numVars, currSize, maxDomainSize;
        // compute maximum domain size
        maxDomainSize = 0;
        numVars = Vars.size();
        for (i = 0; i < numVars; ++i) {
                currSize = Vars[i].size();
                if (maxDomainSize < currSize)
                        maxDomainSize = currSize;
        }
        return new AmIbroadStepping(Vars, 1, maxDomainSize, varHeur, valHeur);
}

NsGoal* AmIbroadStepping::GOAL(void)
{
        if (currBreadthLimit > maxBreadthLimit) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        return new NsgOR(
            new AmIbroadLabeling(Vars, currBreadthLimit, varHeur, valHeur),
            new AmIbroadStepping(Vars, currBreadthLimit + 1, maxBreadthLimit,
                                 varHeur, valHeur));
}

NsGoal* AmIbroadLabeling::GOAL(void)
{
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (new NsgAND(
            new AmIbroadInDomain(Vars[index], breadthLimit, valHeur),
            new AmIbroadLabeling(Vars, breadthLimit, varHeur, valHeur)));
}

NsGoal* AmIbroadInDomain::GOAL(void)
{
        if (breadthLimit == 0) {
                V.removeAll(); // fail
                return 0;
        }
        if (V.isBound())
                return 0;
        NsInt value = valHeur->select(V);
        return (new NsgOR(
            new NsgSetValue(V, value),
            new NsgAND(new NsgRemoveValue(V, value),
                       new AmIbroadInDomain(V, breadthLimit - 1, valHeur))));
}
