/// @file
/// Unnecessary expressions for the XCSP3 Mini-Solver competition
///
/// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace std;
using namespace naxos;

namespace {

void array_sort(NsDeque<NsInt>& domain)
{
        assert_Ns(!domain.empty(), "NsInDomain: empty domain array");
        sort(domain.begin(), domain.end());
}

void exprInDomain_post_constr(NsIntVar& VarX, const NsDeque<NsInt>& domain,
                              NsDeque<NsInt>* domainPrevious,
                              NsDeque<NsInt>* domainNext)
{
        Ns_Constraint* newConstr =
            new Ns_ConstrXinDomain(&VarX, domain, domainPrevious, domainNext);
        VarX.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

} // end namespace

void Ns_ExprInDomain::post(NsIntVar& VarX) const
{
        array_sort(domain);
        VarX = NsIntVar(pm, domain[0], domain[domain.size() - 1]);
        exprInDomain_post_constr(VarX, domain, domainPrevious, domainNext);
}

NsIntVar& Ns_ExprInDomain::post(void) const
{
        array_sort(domain);
        NsIntVar* VarX = new NsIntVar(pm, domain[0], domain[domain.size() - 1]);
        exprInDomain_post_constr(*VarX, domain, domainPrevious, domainNext);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

