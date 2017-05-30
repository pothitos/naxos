/// @file
/// Unnecessary constraints for the XCSP3 Mini-Solver competition
///
/// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace naxos;
using namespace std;

Ns_ConstrXinDomain::Ns_ConstrXinDomain(NsIntVar* X,
                                       const NsDeque<NsInt>& domain,
                                       NsDeque<NsInt>* domainPrevious_init,
                                       NsDeque<NsInt>* domainNext_init)
  : VarX(X), domainPrevious(domainPrevious_init), domainNext(domainNext_init)
{
        if (domainPrevious == 0) {
                assert_Ns(domainNext == 0, "Ns_ConstrXinDomain::Ns_"
                                           "ConstrXinDomain: non-empty "
                                           "'domainNext'");
                domainPrevious = new NsDeque<NsInt>();
                domainNext = new NsDeque<NsInt>();
                arraysAllocated = true;
        } else {
                arraysAllocated = false;
        }
        assert_Ns(domainPrevious->size() == domainNext->size(),
                  "Ns_ConstrXinDomain::Ns_ConstrXinDomain: unequal sizes of "
                  "auxiliary arrays");
        min = domain[0];
        if (domainPrevious->empty()) {
                NsInt max = domain[domain.size() - 1];
                domainPrevious->resize(max - min + 1);
                domainNext->resize(max - min + 1);
                NsInt i, j;
                j = min;
                for (i = 0; i < static_cast<NsInt>(domain.size()); ++i)
                        for (; j <= domain[i]; ++j)
                                (*domainNext)[j - min] = domain[i];
                j = max;
                for (i = domain.size() - 1; i >= 0; --i)
                        for (; j >= domain[i]; --j)
                                (*domainPrevious)[j - min] = domain[i];
        }
}

void Ns_ConstrXinDomain::ArcCons(void)
{
        VarX->removeRange(NsMINUS_INF, (*domainNext)[VarX->min() - min] - 1,
                          this);
        VarX->removeRange((*domainPrevious)[VarX->max() - min] + 1, NsPLUS_INF,
                          this);
}

void Ns_ConstrXinDomain::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}
