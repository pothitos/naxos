#ifndef CREDIT_H
#define CREDIT_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmCredit : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int credit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmCredit(NsIntVarArray& Vars_init, int cred,
                 VariableHeuristic* varHeuristic = new VarHeurMRV,
                 ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            credit(cred),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // CREDIT_H
