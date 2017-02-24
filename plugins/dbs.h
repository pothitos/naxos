#ifndef NS_DBS_H
#define NS_DBS_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmDbsStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int depthLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmDbsStepping(NsIntVarArray& Vars_init, int limit = 0,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
                depthLimit = limit;
        }
        NsGoal* GOAL(void);
};

class AmDbsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int depthLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmDbsLabeling(NsIntVarArray& Vars_init, int limit,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            depthLimit(limit),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_DBS_H
