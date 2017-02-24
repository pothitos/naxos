#ifndef NS_ISAMP_H
#define NS_ISAMP_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmIsampStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int nProbes;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmIsampStepping(NsIntVarArray& Vars_init, int numProbes,
                        VariableHeuristic* varHeuristic = new VarHeurMRV,
                        ValueHeuristic* valHeuristic = new ValHeurRand)
          : Vars(Vars_init),
            nProbes(numProbes),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_ISAMP_H
