#ifndef NS_DFS_H
#define NS_DFS_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmDfsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmDfsLabeling(NsIntVarArray& Vars_init,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmDfsInDomain : public NsGoal {
    private:
        NsIntVar& Var;

        ValueHeuristic* valHeur;

    public:
        AmDfsInDomain(NsIntVar& Var_init,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Var(Var_init), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_DFS_H
