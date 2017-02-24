#ifndef NS_POPS_H
#define NS_POPS_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmPopsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;

        const double proportion;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmPopsLabeling(NsIntVarArray& Vars_init, const double proportion_init,
                       VariableHeuristic* varHeuristic = new VarHeurMRV,
                       ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            proportion(proportion_init),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
                assert_Ns(0.0 <= proportion && proportion <= 1.0,
                          "AmPopsLabeling::AmPopsLabeling: proportion must be "
                          "between 0 and 1");
        }
        NsGoal* GOAL(void);
};

class AmPopsInDomain : public NsGoal {
    private:
        NsIntVar& Var;

        const double proportion;
        double proportionCovered;

        ValueHeuristic* valHeur;

    public:
        AmPopsInDomain(NsIntVar& Var_init, const double proportion_init,
                       ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Var(Var_init),
            proportion(proportion_init),
            proportionCovered(0.0),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_POPS_H
