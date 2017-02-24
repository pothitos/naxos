#ifndef DOMAIN_SPLITTING_H
#define DOMAIN_SPLITTING_H

#include "heuristics.h"
#include <naxos.h>

#include <cmath>

namespace naxos {

class SplitValHeuristic {

    public:
        virtual double select(const NsIntVar& V) = 0;

        virtual ~SplitValHeuristic(void)
        {
        }
};

class ValHeurMiddle : public SplitValHeuristic {

    public:
        double select(const NsIntVar& V)
        {
                return ((static_cast<double>(V.min()) + V.max()) / 2);
        }
};

///  Constrains the domain of the variable to be a subset of [\a start..\a end].

class Am_SetInterval : public NsGoal {

    private:
        ///  The domain of this variable will be limited.
        NsIntVar& Var;

        ///  Interval start.
        const NsInt start;

        ///  Interval end.
        const NsInt end;

    public:
        ///  Constructor.
        Am_SetInterval(NsIntVar& Var_init, const NsInt start_init,
                       const NsInt end_init)
          : Var(Var_init), start(start_init), end(end_init)
        {
                assert_Ns(NsMINUS_INF <= start && start <= end &&
                              end <= NsPLUS_INF,
                          "Am_SetInterval::Am_SetInterval: Invalid interval");
        }

        ///  Goal execution.
        NsGoal* GOAL(void)
        {
                if (start != NsMINUS_INF)
                        Var.remove(NsMINUS_INF, start - 1);
                if (end != NsPLUS_INF)
                        Var.remove(end + 1, NsPLUS_INF);
                return 0;
        }
};

///  Goal that selects a value to separate the domain of a variable during
///  domain-splitting.

class Am_DomainSplittingInDomain : public NsGoal {

    private:
        ///  Variable that will be `separated.'
        NsIntVar& Var;

        ///  Value ordering heuristic.
        SplitValHeuristic* valHeur;

    public:
        ///  Constructor.
        Am_DomainSplittingInDomain(
            NsIntVar& Var_init,
            SplitValHeuristic* valHeuristic = new ValHeurMiddle)
          : Var(Var_init), valHeur(valHeuristic)
        {
        }

        ///  Somehow similar to NsgInDomain::GOAL().
        NsGoal* GOAL(void)
        {
                if (Var.isBound())
                        return 0;
                // NsInt  value  =  static_cast<NsInt>( round( (
                //		static_cast<double>(Var.min()) + Var.max() )  /  2
                //) );
                double value = valHeur->select(Var);
                NsInt intValue = static_cast<NsInt>(floor(value));
                // std::cout << Var << "\tSplit = " << value << "\n";
                // system("pause");
                // cout << "Var=" << Var << "\tMiddleValue= " << value << ".\n";
                NsGoal* goalA = new Am_SetInterval(Var, NsMINUS_INF, intValue);
                NsGoal* goalB =
                    new Am_SetInterval(Var, intValue + 1, NsPLUS_INF);
                if (value - Var.min() <= Var.max() - value)
                        return (new NsgOR(goalA, goalB));
                else
                        return (new NsgOR(goalB, goalA));
        }
};

///  Domain-splitting method.

class AmDomainSplittingLabeling : public NsGoal {

    private:
        ///  The variables of this array should be instantiated.
        NsIntVarArray& VarArr;

        ///  Variable ordering heuristic.
        VariableHeuristic* varHeur;

        ///  Value ordering heuristic.
        SplitValHeuristic* valHeur;

    public:
        ///  Constructor.
        AmDomainSplittingLabeling(
            NsIntVarArray& VarArr_init,
            VariableHeuristic* varHeuristic = new VarHeurRand,
            SplitValHeuristic* valHeuristic = new ValHeurMiddle)
          : VarArr(VarArr_init), varHeur(varHeuristic), valHeur(valHeuristic)
        {
        }

        ///  Selects a variable and generates the goals to `separate' it.
        NsGoal* GOAL(void)
        {
                // double  random = rand() / (RAND_MAX + 1.0);	// A random
                // number in [0,1).
                NsInt index = varHeur->select(VarArr);
                if (index == -1)
                        return 0; // all variables are bound => success
                return (new NsgAND(
                    new Am_DomainSplittingInDomain(VarArr[index], valHeur),
                    new AmDomainSplittingLabeling(*this)));
        }
};

double splitValueConstrLess(const NsIntVar X, const NsIntVar Y,
                            const bool greaterThan);
double splitValueConstrNeq(const NsIntVar X, const NsIntVar Y);

} // end namespace
#endif // DOMAIN_SPLITTING_H
