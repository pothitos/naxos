#include "pops.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmPopsLabeling::GOAL(void)
{
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (new NsgAND(new AmPopsInDomain(Vars[index], proportion, valHeur),
                           new AmPopsLabeling(*this)));
}

NsGoal* AmPopsInDomain::GOAL(void)
{
        if (Var.isBound())
                return 0;
        double selectedPiece;
        NsInt value = valHeur->select(Var, selectedPiece);
        proportionCovered += selectedPiece * (1.0 - proportionCovered);
        if (proportionCovered >= proportion)
                return (new NsgSetValue(Var, value));
        return (new NsgOR(new NsgSetValue(Var, value),
                          new NsgAND(new NsgRemoveValue(Var, value),
                                     new AmPopsInDomain(*this))));
}

// 0.2 0.1 0.1 0.3 0.1 0.2
// selected 0.1  remains 0.4
// 0.2     0.1 0.3 0.1 0.2
// selected ??
// 	remains (A - selected) / (1 - selected)
// 	selected += newSelected*(1-selected)
