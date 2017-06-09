/// @file
/// Unnecessary definitions for the XCSP3 Mini-Solver competition
///
/// Part of https://github.com/pothitos/naxos

#ifndef Ns_NAXOS_H
#define Ns_NAXOS_H

#include "naxos-mini.h"

namespace naxos {

/// This is somehow 'stronger' than the simple 'X == Y + C*Z'
///
/// It requires some special conditions, that allow the
/// efficient application of the pure arc-consistency, i.e. not
/// only bounds consistency.
class Ns_ConstrXeqYplusCZspecial : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        NsInt C;

    public:
        Ns_ConstrXeqYplusCZspecial(NsIntVar* X, NsIntVar* Y, const NsInt C_init,
                                   NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z), C(C_init)
        {
                revisionType = VALUE_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYplusCZspecial::Ns_"
                          "ConstrXeqYplusCZspecial: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(X->min() >= 0, "Ns_ConstrXeqYplusCZspecial::Ns_"
                                         "ConstrXeqYplusCZspecial: Special "
                                         "condition required: X >= 0");
                assert_Ns(0 <= Y->min() && Y->max() < C,
                          "Ns_ConstrXeqYplusCZspecial::Ns_"
                          "ConstrXeqYplusCZspecial: Special condition "
                          "required: 0 <= Y < C");
                assert_Ns(C > 0, "Ns_ConstrXeqYplusCZspecial::Ns_"
                                 "ConstrXeqYplusCZspecial: Condition required: "
                                 "C > 0");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this,
                                                "y+c*z special", false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXinDomain : public Ns_Constraint {

    private:
        NsIntVar* VarX;

        NsInt min;
        NsDeque<NsInt>* domainPrevious;
        NsDeque<NsInt>* domainNext;
        bool arraysAllocated;

    public:
        Ns_ConstrXinDomain(NsIntVar* X, const NsDeque<NsInt>& domain,
                           NsDeque<NsInt>* domainPrevious_init,
                           NsDeque<NsInt>* domainNext_init);

        virtual ~Ns_ConstrXinDomain(void)
        {
                if (arraysAllocated) {
                        delete domainPrevious;
                        delete domainNext;
                }
        }

        virtual int varsInvolvedIn(void) const
        {
                return 1;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\t//Var" << VarX << " -> Dom"
                                     << &domainPrevious
                                     << " [label=\"dom\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrCount : public Ns_Constraint {

    private:
        NsIntVarArray* VarArr;

        typedef Ns_UNORDERED_MAP<Ns_pointer_t, NsIndex> VarIndex_t;

        VarIndex_t VarIndex;

    public:
        typedef Ns_UNORDERED_MAP<NsInt, NsIndex> ValueIndex_t;

    private:
        ValueIndex_t ValueIndex;

    public:
        struct ValueOccurrence_t {

                NsInt value;

                NsInt occurrence;

                const NsDeque<NsIndex>* split_positions;

                NsIntVarArray vCount;

                ValueOccurrence_t(const NsInt value_init) : value(value_init)
                {
                }

                ValueOccurrence_t(const NsInt value_init,
                                  const NsInt occurrence_init,
                                  NsProblemManager& pm)
                  : value(value_init),
                    occurrence(occurrence_init),
                    split_positions(0)
                {
                        assert_Ns(occurrence >= 0,
                                  "Ns_ConstrCount::ValueOccurrence_t::"
                                  "ValueOccurrence_t: negative value in "
                                  "'occurrence'");
                        vCount.push_back(NsIntVar(pm, 0, occurrence));
                }

                ValueOccurrence_t(const NsInt value_init,
                                  const NsInt occurrence_init,
                                  NsProblemManager& pm,
                                  const NsDeque<NsIndex>& split_positions_init,
                                  const NsIndex Split)
                  : value(value_init),
                    occurrence(occurrence_init),
                    split_positions(&split_positions_init)
                {
                        assert_Ns(occurrence >= 0,
                                  "Ns_ConstrCount::ValueOccurrence_t::"
                                  "ValueOccurrence_t: negative value in "
                                  "'occurrence'");
                        for (NsIndex i = 0; i < split_positions->size(); ++i)
                                vCount.push_back(NsIntVar(pm, 0, Split));
                        vCount.push_back(NsIntVar(pm, 0, occurrence % Split));
                        if (split_positions->size() == 0)
                                split_positions = 0;
                }

                NsIndex splitIndexForPosition(NsIndex i)
                {
                        if (split_positions == 0)
                                return 0;
                        NsDeque<NsIndex>::const_iterator cit =
                            lower_bound(split_positions->begin(),
                                        split_positions->end(), i);
                        return (cit - split_positions->begin());
                }

                bool operator<(const ValueOccurrence_t& Y) const
                {
                        return (value < Y.value);
                }
        };

    private:
        NsDeque<ValueOccurrence_t> ValuesOccurrences;
        const NsIndex Split;
        const NsIndex Dwin;

    public:
        Ns_ConstrCount(NsIntVarArray* VarArr_init, const NsDeque<NsInt>& Values,
                       const NsDeque<NsInt>& Occurrences,
                       const NsDeque<NsDeque<NsIndex>>& SplitPositions,
                       const NsIndex Split_init, const NsIndex Dwin_init);

        virtual int varsInvolvedIn(void) const
        {
                return VarArr->size();
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_arrayConstraintToGraphFile(fileConstraintsGraph, VarArr,
                                              this, "count");
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

void Ns_inverseConstraintToGraphFile(std::ofstream& fileConstraintsGraph,
                                     const NsIntVarArray* VarArr,
                                     const NsIntVarArray* VarArrInv,
                                     const Ns_Constraint* constr);

class Ns_ConstrInverse : public Ns_Constraint {

    private:
        NsIntVarArray *VarArrInv, *VarArr;

        NsIntVarArray &VArrInv, &VArr;

        struct ArrayIndex_t {

                const bool InInverse;

                const NsIndex index;

                ArrayIndex_t(const bool InInverse_init,
                             const NsIndex index_init)
                  : InInverse(InInverse_init), index(index_init)
                {
                }
        };

        typedef Ns_UNORDERED_MAP<Ns_pointer_t, ArrayIndex_t> VarArrayIndex_t;

        VarArrayIndex_t VarArrayIndex;

    public:
        Ns_ConstrInverse(NsIntVarArray* VarArrInv_init,
                         NsIntVarArray* VarArr_init);

        virtual int varsInvolvedIn(void) const
        {
                return (VarArrInv->size() + VarArr->size());
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_inverseConstraintToGraphFile(fileConstraintsGraph, VarArr,
                                                VarArrInv, this);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ExprYplusCZspecial : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;
        NsInt C;

    public:
        Ns_ExprYplusCZspecial(NsIntVar& Y, const NsInt C_init, NsIntVar& Z)
          : VarY(Y), VarZ(Z), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYplusCZspecial NsYplusCZspecial(NsIntVar& Y, const NsInt C,
                                              NsIntVar& Z)
{
        return Ns_ExprYplusCZspecial(Y, C, Z);
}

class Ns_ExprInDomain : public Ns_Expression {

    private:
        NsProblemManager& pm;
        NsDeque<NsInt>& domain;
        NsDeque<NsInt>* domainPrevious;
        NsDeque<NsInt>* domainNext;

    public:
        Ns_ExprInDomain(NsProblemManager& pm_init, NsDeque<NsInt>& domain_init,
                        NsDeque<NsInt>* domainPrevious_init,
                        NsDeque<NsInt>* domainNext_init)
          : pm(pm_init),
            domain(domain_init),
            domainPrevious(domainPrevious_init),
            domainNext(domainNext_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprInDomain NsInDomain(NsProblemManager& pm, NsDeque<NsInt>& domain)
{
        return Ns_ExprInDomain(pm, domain, 0, 0);
}

inline Ns_ExprInDomain NsInDomain(NsProblemManager& pm, NsDeque<NsInt>& domain,
                                  NsDeque<NsInt>& domainPrevious,
                                  NsDeque<NsInt>& domainNext)
{
        return Ns_ExprInDomain(pm, domain, &domainPrevious, &domainNext);
}

class Ns_ExprConstrCount : public Ns_ExprConstr {

    private:
        NsIntVarArray& VarArr;
        const NsDeque<NsInt>& Values;
        const NsDeque<NsInt>& Occurrences;
        const NsDeque<NsDeque<NsIndex>>& SplitPositions;
        const NsIndex Split;
        const NsIndex Dwin;

    public:
        Ns_ExprConstrCount(NsIntVarArray& Arr,
                           const NsDeque<NsInt>& Values_init,
                           const NsDeque<NsInt>& Occurrences_init,
                           const NsDeque<NsDeque<NsIndex>>& SplitPositions_init,
                           const NsIndex Split_init, const NsIndex Dwin_init)
          : Ns_ExprConstr(true),
            VarArr(Arr),
            Values(Values_init),
            Occurrences(Occurrences_init),
            SplitPositions(SplitPositions_init),
            Split(Split_init),
            Dwin(Dwin_init)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& /*VarX*/, bool /*positively*/) const
        {
                throw NsException("Ns_ExprConstrCount::postC: NsCount cannot "
                                  "be used as a meta-constraint");
        }
        virtual NsIntVar& postC(bool /*positively*/) const
        {
                throw NsException("Ns_ExprConstrCount::postC: NsCount cannot "
                                  "be used as a meta-constraint");
        }
};

inline Ns_ExprConstrCount
NsCount(NsIntVarArray& Arr, const NsDeque<NsInt>& Values,
        const NsDeque<NsInt>& Occurrences,
        const NsDeque<NsDeque<NsIndex>>& SplitPositions =
            NsDeque<NsDeque<NsIndex>>(),
        const NsIndex Split = 0, const NsIndex Dwin = 1)
{
        return Ns_ExprConstrCount(Arr, Values, Occurrences, SplitPositions,
                                  Split, Dwin);
}

class Ns_ExprInverse : public Ns_ExpressionArray {

    private:
        NsIntVarArray& VarArr;
        NsInt MaxDom;

    public:
        Ns_ExprInverse(NsIntVarArray& VarArr_init, const NsInt MaxDom_init = -1)
          : VarArr(VarArr_init), MaxDom(MaxDom_init)
        {
        }

        virtual void post(NsIntVarArray& VarArrInv) const;
};

inline Ns_ExprInverse NsInverse(NsIntVarArray& Arr)
{
        return Ns_ExprInverse(Arr);
}

inline Ns_ExprInverse NsInverse(NsIntVarArray& Arr, const NsInt MaxDom)
{
        return Ns_ExprInverse(Arr, MaxDom);
}

} // end namespace

#endif // Ns_NAXOS_H
