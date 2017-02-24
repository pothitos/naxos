#ifndef NS_LDS_H
#define NS_LDS_H

#include "dfs.h"
#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmLds : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int minDiscr;
        int step, maxDiscr, lookAhead;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmLds(NsIntVarArray& Vars_init, int lStep = 1, int lAhead = 0,
              int minDiscrepancy = 0, int maxDiscrepancy = 0,
              VariableHeuristic* varHeuristic = new VarHeurMRV,
              ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            minDiscr(minDiscrepancy),
            step(lStep),
            maxDiscr(maxDiscrepancy),
            lookAhead(lAhead),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmLdsStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int minDiscr;
        NsIntVar& vDiscr;
        int step, maxDiscr, lookAhead;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmLdsStepping(NsIntVarArray& Vars_init, int minDiscrepancy,
                      NsIntVar& vDiscrepancy, int lStep, int maxDiscrep,
                      int lAhead,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            minDiscr(minDiscrepancy),
            vDiscr(vDiscrepancy),
            step(lStep),
            maxDiscr(maxDiscrep),
            lookAhead(lAhead),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmLdsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int minDiscr;
        NsIntVar& vDiscr;
        int step, lookAhead;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmLdsLabeling(NsIntVarArray& Vars_init, int minDiscrepancy,
                      NsIntVar& vDiscrepancy, int lStep = 1, int lAhead = 0,
                      VariableHeuristic* varHeuristic = new VarHeurMRV,
                      ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            minDiscr(minDiscrepancy),
            vDiscr(vDiscrepancy),
            step(lStep),
            lookAhead(lAhead),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmLdsInDomain : public NsGoal {
    private:
        NsIntVarArray& Vars;
        const int index;

        int minDiscr;
        NsIntVar& vDiscr;
        int step, lookAhead;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmLdsInDomain(NsIntVarArray& Vars_init, int index_init,
                      int minDiscrepancy, NsIntVar& vDiscrepancy, int lStep,
                      int lAhead, VariableHeuristic* varHeuristic,
                      ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            index(index_init),
            minDiscr(minDiscrepancy),
            vDiscr(vDiscrepancy),
            step(lStep),
            lookAhead(lAhead),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }

        NsGoal* GOAL(void);
};

class AmLdsRemoveValue : public NsGoal {
    private:
        NsIntVar& V;
        const NsInt value;
        NsIntVar& vDiscr;

    public:
        AmLdsRemoveValue(NsIntVar& V_init, const NsInt value_init,
                         NsIntVar& vDiscrepancy)
          : V(V_init), value(value_init), vDiscr(vDiscrepancy)
        {
        }

        NsGoal* GOAL(void);
};

class AmLdsDestructor : public NsGoal {
    private:
        NsIntVarArray& Vars;
        NsIntVar* vDiscr;

    public:
        AmLdsDestructor(NsIntVarArray& Vars_init, NsIntVar* vDiscrepancy)
          : Vars(Vars_init), vDiscr(vDiscrepancy)
        {
        }

        NsGoal* GOAL(void)
        {
                Vars[0].removeAll();
                return 0;
        }

        ~AmLdsDestructor(void)
        {
                delete vDiscr;
        }
};

int remainingChoices(const NsIntVarArray& VarArr);

} // end namespace
#endif // NS_LDS_H
