#ifndef NS_RDFS_H
#define NS_RDFS_H

#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmRdfs : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int times;
        int timeoutLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmRdfs(NsIntVarArray& Vars_init, int tim, int tout,
               VariableHeuristic* varHeuristic = new VarHeurMRV,
               ValueHeuristic* valHeuristic = new ValHeurRand)
          : Vars(Vars_init),
            times(tim),
            timeoutLimit(tout),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmRdfsDestructor : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int* timeout;

    public:
        AmRdfsDestructor(NsIntVarArray& Vars_init, int* tout)
          : Vars(Vars_init), timeout(tout)
        {
        }

        NsGoal* GOAL(void)
        {
                Vars[0].removeAll();
                return 0;
        }

        ~AmRdfsDestructor(void)
        {
                delete timeout;
        }
};

class AmRdfsStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int times;
        int* timeout;
        int timeoutLimit;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmRdfsStepping(NsIntVarArray& Vars_init, int tim, int* tout, int limit,
                       VariableHeuristic* varHeuristic,
                       ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            times(tim),
            timeout(tout),
            timeoutLimit(limit),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmRdfsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int* timeout;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmRdfsLabeling(NsIntVarArray& Vars_init, int* tout,
                       VariableHeuristic* varHeuristic,
                       ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            timeout(tout),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmRdfsInDomain : public NsGoal {
    private:
        NsIntVar& Var;
        int* timeout;

        ValueHeuristic* valHeur;

    public:
        AmRdfsInDomain(NsIntVar& Var_init, int* tout,
                       ValueHeuristic* valHeuristic)
          : Var(Var_init), timeout(tout), valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_RDFS_H
