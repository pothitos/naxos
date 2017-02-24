#ifndef NS_HEURISTICS_H
#define NS_HEURISTICS_H

#include <naxos.h>

namespace naxos {

/* VARIABLE ORDERING HEURISTIC CLASSES */

// Parent class
class VariableHeuristic {
    public:
        virtual int select(const naxos::NsIntVarArray&) = 0;
        virtual ~VariableHeuristic()
        {
        }
};

class VarHeurFirst : public VariableHeuristic {
    public:
        int select(const naxos::NsIntVarArray& Vars);
};

class VarHeurMRV : public VariableHeuristic {
    public:
        int select(const naxos::NsIntVarArray& Vars);
};

class VarHeurRand : public VariableHeuristic {
    public:
        int select(const naxos::NsIntVarArray& Vars);
};

/*******
class VarHeurDegree : public VariableHeuristic{
public:
        int select(const naxos::NsIntVarArray& Vars);
};
*/

/* VALUE ORDERING HEURISTIC CLASSES */

// Parent class
class ValueHeuristic {
    public:
        virtual naxos::NsInt select(const naxos::NsIntVar&) = 0;
        virtual ~ValueHeuristic()
        {
        }

        virtual naxos::NsInt select(const naxos::NsIntVar& V, double& piece)
        {
                piece = 1.0 / V.size();
                return select(V);
        }
};

class ValHeurFirst : public ValueHeuristic {
    public:
        naxos::NsInt select(const naxos::NsIntVar& V);
};

class ValHeurRand : public ValueHeuristic {
    public:
        naxos::NsInt select(const naxos::NsIntVar& V);
};

} // end namespace
#endif // NS_HEURISTICS_H
