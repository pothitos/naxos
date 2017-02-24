#ifndef NS_BBS_H
#define NS_BBS_H

#include "dfs.h"
#include "heuristics.h"
#include <naxos.h>

namespace naxos {

class AmBbs : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int lookaheadLimit;
        int times;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmBbs(NsIntVarArray& Vars_init, int tim, int lahead,
              VariableHeuristic* varHeuristic = new VarHeurFirst,
              ValueHeuristic* valHeuristic = new ValHeurRand)
          : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
                lookaheadLimit = lahead;
                times = tim;
        }
        NsGoal* GOAL(void);
};

class AmBbsDestructor : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int* maxLevel;

    public:
        AmBbsDestructor(NsIntVarArray& Vars_init, int* ml)
          : Vars(Vars_init), maxLevel(ml)
        {
        }

        NsGoal* GOAL(void)
        {
                Vars[0].removeAll();
                return 0;
        }

        ~AmBbsDestructor(void)
        {
                delete maxLevel;
        }
};

class AmBbsStepping : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int lookaheadLimit;
        int times;
        int& maxLevel;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmBbsStepping(NsIntVarArray& Vars_init, int lahead, int tim,
                      int& mLevel, VariableHeuristic* varHeuristic,
                      ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            maxLevel(mLevel),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
                lookaheadLimit = lahead;
                times = tim;
        }
        NsGoal* GOAL(void);
};

class AmBbsLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        int lookahead;
        int& maxLevel;

        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;

    public:
        AmBbsLabeling(NsIntVarArray& Vars_init, int lAhead, int& maxLev,
                      VariableHeuristic* varHeuristic,
                      ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            maxLevel(maxLev),
            varHeur(varHeuristic),
            valHeur(valHeuristic)
        {
                lookahead = lAhead;
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_BBS_H
