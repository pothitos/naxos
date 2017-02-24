#ifndef NS_FNS_H
#define NS_FNS_H

#include <cmath>

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

int fSample(int nChildren, int depth, int treeHeight);
int fSampleMiddle(int nChildren, int depth, int treeHeight);
int fSampleExp(int nChildren, int depth, int treeHeight);

class AmFnsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int (*f)(int, int, int);

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmFnsLabeling(NsIntVarArray& Vars_init, int (*function)(int, int, int),
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            f(function),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_FNS_H
