#ifndef NS_ONESAMP_H
#define NS_ONESAMP_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmOnesampLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmOnesampLabeling(NsIntVarArray& Vars_init,
                          VariableHeuristic* varHeuristic = new VarHeurFirst,
                          ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmOnesampInDomain : public NsGoal {
    private:
        NsIntVar& V;

        ValueHeuristic* valHeur;

    public:
        AmOnesampInDomain(NsIntVar& Var,
                          ValueHeuristic* valHeuristic = new ValHeurFirst)
          : V(Var), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_ONESAMP_H
