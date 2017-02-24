#ifndef NS_LAN_H
#define NS_LAN_H

#include "heuristics.h"
#include <naxos.h>

int VarOrderHeurFiltered(const naxos::NsIntVarArray& Vars,
                         /*unsigned **/ std::deque<unsigned>& assigns,
                         unsigned asLimit);

namespace naxos {

class AmLan : public NsGoal {
    private:
        NsIntVarArray& Vars;
        unsigned assignLimit;

        // VariableHeuristic *varHeur;//ITC
        ValueHeuristic* valHeur;

    public:
        AmLan(NsIntVarArray& Vars_init, int aLimit,
              // VariableHeuristic *varHeuristic = new VarHeurMRV,//ITC
              ValueHeuristic* valHeuristic = new ValHeurFirst)
          : Vars(Vars_init),
            assignLimit(aLimit),
            /*varHeur(varHeuristic),ITC*/ valHeur(valHeuristic)
        {
        }
        NsGoal* GOAL(void);
};

class AmLanDestructor : public NsGoal {
    private:
        NsIntVarArray& Vars;
        unsigned* assigns;

    public:
        AmLanDestructor(NsIntVarArray& Vars_init, unsigned* asgns)
          : Vars(Vars_init), assigns(asgns)
        {
        }

        NsGoal* GOAL(void)
        {
                Vars[0].removeAll();
                return 0;
        }

        ~AmLanDestructor(void)
        {
                delete assigns;
        }
};

class AmLanLabeling : public NsGoal {
    private:
        NsIntVarArray& Vars;
        unsigned assignLimit;
        // unsigned *assigns;ITC

        // ITC
        VariableHeuristic* varHeur;
        ValueHeuristic* valHeur;
        std::deque<unsigned>& assigns;

    public:
        AmLanLabeling(NsIntVarArray& Vars_init,
                      /*unsigned**/ std::deque<unsigned>& asgns,
                      unsigned aLimit,
                      // ITC
                      VariableHeuristic* varHeuristic,
                      ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            assignLimit(aLimit),
            varHeur(varHeuristic),
            /*ITC*/ valHeur(valHeuristic),
            assigns(asgns)
        {
        }

        NsGoal* GOAL(void);
};

class AmLanInDomain : public NsGoal {
    private:
        NsIntVarArray& Vars;
        const int index;
        unsigned assignLimit;
        // unsigned *assigns;ITC

        ValueHeuristic* valHeur;
        std::deque<unsigned>& assigns;

    public:
        AmLanInDomain(NsIntVarArray& Vars_init, int index_init,
                      /*unsigned **/ std::deque<unsigned>& asgns,
                      unsigned aLimit, ValueHeuristic* valHeuristic)
          : Vars(Vars_init),
            index(index_init),
            assignLimit(aLimit),
            valHeur(valHeuristic),
            assigns(asgns)
        {
        }
        NsGoal* GOAL(void);
};

} // end namespace
#endif // NS_LAN_H
