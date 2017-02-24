#include "lan.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

// MRV filtered
int VarOrderHeurFiltered(const NsIntVarArray& Vars,
                         /*unsigned**/ deque<unsigned>& assigns,
                         unsigned asLimit)
{
        int index = -1;
        NsUInt minDom = NsUPLUS_INF;
        for (NsIndex i = 0; i < Vars.size(); ++i) {
                if (!Vars[i].isBound() && assigns[i] >= asLimit && index == -1)
                        index = -2;
                if (!Vars[i].isBound() && assigns[i] < asLimit &&
                    Vars[i].size() < minDom) {
                        minDom = Vars[i].size();
                        index = i;
                }
        }
        return index;
}

NsGoal* AmLan::GOAL(void)
{
        unsigned* assigns = new unsigned[Vars.size()];
        for (unsigned i = 0; i < Vars.size(); ++i)
                assigns[i] = 0;
        return 0;
        // return (new NsgOR(new
        // AmLanLabeling(Vars,assigns,assignLimit,varHeur,valHeur),ITC
        //                  new AmLanDestructor(Vars,assigns)));
}

NsGoal* AmLanLabeling::GOAL(void)
{
        int index = VarOrderHeurFiltered(Vars, assigns, assignLimit);
        if (index >= 0) // ITC
                index = varHeur->select(Vars);
        else
                varHeur->select(Vars);
        if (index == -1) // all variables are bound => success
                return 0;
        else if (index == -2) { // there are unbound expired variables and no
                Vars[0].removeAll(); // assignment possible => failure
                return 0;
        }
        return (new NsgAND(
            new AmLanInDomain(Vars, index, assigns, assignLimit, valHeur),
            new AmLanLabeling(Vars, assigns, assignLimit, varHeur, valHeur)));
}

NsGoal* AmLanInDomain::GOAL(void)
{
        NsIntVar& V = Vars[index]; // renaming for readability
        if (V.isBound())
                return 0;
        NsInt value = valHeur->select(V);
        assigns[index]++;
        return (new NsgOR(new NsgSetValue(V, value),
                          new NsgAND(new NsgRemoveValue(V, value),
                                     new AmLanInDomain(Vars, index, assigns,
                                                       assignLimit, valHeur))));
}
