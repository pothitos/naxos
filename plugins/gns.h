#ifndef NS_GNS_H
#define NS_GNS_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmGnsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmGnsLabeling(NsIntVarArray& Vars_init,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_GNS_H
