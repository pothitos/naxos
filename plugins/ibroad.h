#ifndef NS_IBROAD_H
#define NS_IBROAD_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmIbroad : public NsGoal {
    private:
        NsIntVarArray& Vars;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmIbroad(NsIntVarArray& Vars_init,
                 VariableHeuristic* varHeuristic = new VarHeurMRV,
                 ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmIbroadStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        unsigned currBreadthLimit, maxBreadthLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmIbroadStepping(NsIntVarArray& Vars_init, unsigned currBLimit,
                         unsigned maxBLimit,
                         VariableHeuristic* varHeuristic = new VarHeurMRV,
                         ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            currBreadthLimit(currBLimit),
            maxBreadthLimit(maxBLimit),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmIbroadLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        unsigned breadthLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmIbroadLabeling(NsIntVarArray& Vars_init, unsigned bLimit,
                         VariableHeuristic* varHeuristic = new VarHeurMRV,
                         ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            breadthLimit(bLimit),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmIbroadInDomain : public NsGoal {
    private:
        NsIntVar& V;
        unsigned breadthLimit;

        ValueHeuristic* valHeur;

    public:
        AmIbroadInDomain(NsIntVar& V_init, unsigned bLimit,
                         ValueHeuristic* valHeuristic = new ValHeurFirst)
          : V(V_init), breadthLimit(bLimit), valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_IBROAD_H
