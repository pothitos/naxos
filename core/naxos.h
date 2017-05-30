/// @file
/// Unnecessary definitions for the XCSP3 Mini-Solver competition
///
/// Part of https://github.com/pothitos/naxos

#ifndef Ns_NAXOS_H
#define Ns_NAXOS_H

#include "naxos-mini.h"

namespace naxos {

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

} // end namespace

#endif // Ns_NAXOS_H
