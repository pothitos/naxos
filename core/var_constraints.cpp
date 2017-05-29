/// @file
/// Algorithms that impose the constraints over variables
///
/// Part of https://github.com/pothitos/naxos

#include "internal.h"
#include "naxos.h"
#include <algorithm>
#include <cstdlib>

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

void Ns_ConstrXlessthanY::ArcCons(void)
{
        VarX->removeRange(VarY->max(), NsPLUS_INF, this);
        VarY->removeRange(NsMINUS_INF, VarX->min(), this);
}

void Ns_ConstrXlessthanY::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                VarY->removeRange(NsMINUS_INF, VarX->min(), this);
        } else {
                assert_Ns(
                    VarY == Qitem.getVarFired(),
                    "Ns_ConstrXlessthanY::LocalArcCons: Wrong getVarFired");
                VarX->removeRange(VarY->max(), NsPLUS_INF, this);
        }
}

void Ns_ConstrXlesseqthanY::ArcCons(void)
{
        VarX->removeRange(VarY->max() + 1, NsPLUS_INF, this);
        VarY->removeRange(NsMINUS_INF, VarX->min() - 1, this);
}

void Ns_ConstrXlesseqthanY::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                VarY->removeRange(NsMINUS_INF, VarX->min() - 1, this);
        } else {
                assert_Ns(
                    VarY == Qitem.getVarFired(),
                    "Ns_ConstrXlesseqthanY::LocalArcCons: Wrong getVarFired");
                VarX->removeRange(VarY->max() + 1, NsPLUS_INF, this);
        }
}

namespace {

inline void makeBoolean(NsIntVar* VarX, const Ns_Constraint* constraint)
{
        VarX->removeRange(NsMINUS_INF, -1, constraint);
        VarX->removeRange(2, NsPLUS_INF, constraint);
}

} // namespace

void Ns_ConstrMetaXeqYlessthanC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->max() < C)
                VarX->removeSingle(0, this);
        if (VarY->min() >= C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                } else {
                        assert_Ns(VarX->value() == 1,
                                  "Ns_ConstrMetaXeqYlessthanC::ArcCons: Wrong "
                                  "'VarX->value()'");
                        VarY->removeRange(C, NsPLUS_INF, this);
                }
        }
}

void Ns_ConstrMetaXeqYlessthanC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYlessthanC::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(C, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrMetaXeqYlessthanC::LocalArcCons: Wrong "
                          "getVarFired");
                if (VarY->max() < C)
                        VarX->removeSingle(0, this);
                if (VarY->min() >= C)
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYlessthanZ::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->max() < VarZ->min())
                VarX->removeSingle(0, this);
        if (VarY->min() >= VarZ->max())
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarZ->removeRange(VarY->max() + 1, NsPLUS_INF, this);
                        VarY->removeRange(NsMINUS_INF, VarZ->min() - 1, this);
                } else {
                        VarY->removeRange(VarZ->max(), NsPLUS_INF, this);
                        VarZ->removeRange(NsMINUS_INF, VarY->min(), this);
                }
        }
}

void Ns_ConstrMetaXeqYlessthanZ::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarZ->removeRange(VarY->max() + 1, NsPLUS_INF,
                                                  this);
                                VarY->removeRange(NsMINUS_INF, VarZ->min() - 1,
                                                  this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYlessthanZ::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(VarZ->max(), NsPLUS_INF,
                                                  this);
                                VarZ->removeRange(NsMINUS_INF, VarY->min(),
                                                  this);
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        VarZ->removeRange(VarY->max() + 1,
                                                          NsPLUS_INF, this);
                                } else {
                                        assert_Ns(VarX->value() == 1,
                                                  "Ns_ConstrMetaXeqYlessthanZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarZ->removeRange(NsMINUS_INF,
                                                          VarY->min(), this);
                                }
                        }
                } else {
                        assert_Ns(VarZ == Qitem.getVarFired(),
                                  "Ns_ConstrMetaXeqYlessthanZ::LocalArcCons: "
                                  "Wrong getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        VarY->removeRange(
                                            NsMINUS_INF, VarZ->min() - 1, this);
                                } else {
                                        assert_Ns(VarX->value() == 1,
                                                  "Ns_ConstrMetaXeqYlessthanZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarY->removeRange(VarZ->max(),
                                                          NsPLUS_INF, this);
                                }
                        }
                }
                if (VarY->max() < VarZ->min())
                        VarX->removeSingle(0, this);
                if (VarY->min() >= VarZ->max())
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYlesseqthanC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->max() <= C)
                VarX->removeSingle(0, this);
        if (VarY->min() > C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeRange(NsMINUS_INF, C, this);
                } else {
                        assert_Ns(VarX->value() == 1,
                                  "Ns_ConstrMetaXeqYlesseqthanC::ArcCons: "
                                  "Wrong 'VarX->value()'");
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                }
        }
}

void Ns_ConstrMetaXeqYlesseqthanC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeRange(NsMINUS_INF, C, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYlesseqthanC::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrMetaXeqYlesseqthanC::LocalArcCons: Wrong "
                          "getVarFired");
                if (VarY->max() <= C)
                        VarX->removeSingle(0, this);
                if (VarY->min() > C)
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYlesseqthanZ::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->max() <= VarZ->min())
                VarX->removeSingle(0, this);
        if (VarY->min() > VarZ->max())
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarZ->removeRange(VarY->max(), NsPLUS_INF, this);
                        VarY->removeRange(NsMINUS_INF, VarZ->min(), this);
                } else {
                        assert_Ns(VarX->value() == 1,
                                  "Ns_ConstrMetaXeqYlesseqthanZ::ArcCons: "
                                  "Wrong 'VarX->value()'");
                        VarY->removeRange(VarZ->max() + 1, NsPLUS_INF, this);
                        VarZ->removeRange(NsMINUS_INF, VarY->min() - 1, this);
                }
        }
}

void Ns_ConstrMetaXeqYlesseqthanZ::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarZ->removeRange(VarY->max(), NsPLUS_INF,
                                                  this);
                                VarY->removeRange(NsMINUS_INF, VarZ->min(),
                                                  this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYlesseqthanZ::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(VarZ->max() + 1, NsPLUS_INF,
                                                  this);
                                VarZ->removeRange(NsMINUS_INF, VarY->min() - 1,
                                                  this);
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        VarZ->removeRange(VarY->max(),
                                                          NsPLUS_INF, this);
                                } else {
                                        assert_Ns(VarX->value() == 1,
                                                  "Ns_"
                                                  "ConstrMetaXeqYlesseqthanZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarZ->removeRange(
                                            NsMINUS_INF, VarY->min() - 1, this);
                                }
                        }
                } else {
                        assert_Ns(VarZ == Qitem.getVarFired(),
                                  "Ns_ConstrMetaXeqYlesseqthanZ::LocalArcCons: "
                                  "Wrong getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        VarY->removeRange(NsMINUS_INF,
                                                          VarZ->min(), this);
                                } else {
                                        assert_Ns(VarX->value() == 1,
                                                  "Ns_"
                                                  "ConstrMetaXeqYlesseqthanZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarY->removeRange(VarZ->max() + 1,
                                                          NsPLUS_INF, this);
                                }
                        }
                }
                if (VarY->max() <= VarZ->min())
                        VarX->removeSingle(0, this);
                if (VarY->min() > VarZ->max())
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYgreaterthanC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->min() > C)
                VarX->removeSingle(0, this);
        if (VarY->max() <= C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                } else {
                        assert_Ns(VarX->value() == 1,
                                  "Ns_ConstrMetaXeqYgreaterthanC::ArcCons: "
                                  "Wrong 'VarX->value()'");
                        VarY->removeRange(NsMINUS_INF, C, this);
                }
        }
}

void Ns_ConstrMetaXeqYgreaterthanC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYgreaterthanC::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(NsMINUS_INF, C, this);
                        }
                }
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrMetaXeqYgreaterthanC::LocalArcCons: Wrong "
                          "getVarFired");
                if (VarY->min() > C)
                        VarX->removeSingle(0, this);
                if (VarY->max() <= C)
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYgreatereqthanC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->min() >= C)
                VarX->removeSingle(0, this);
        if (VarY->max() < C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeRange(C, NsPLUS_INF, this);
                } else {
                        assert_Ns(VarX->value() == 1,
                                  "Ns_ConstrMetaXeqYgreatereqthanC::ArcCons: "
                                  "Wrong 'VarX->value()'");
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                }
        }
}

void Ns_ConstrMetaXeqYgreatereqthanC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeRange(C, NsPLUS_INF, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYgreatereqthanC::"
                                          "LocalArcCons: Wrong "
                                          "'VarX->value()'");
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                        }
                }
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrMetaXeqYgreatereqthanC::LocalArcCons: "
                          "Wrong getVarFired");
                if (VarY->min() >= C)
                        VarX->removeSingle(0, this);
                if (VarY->max() < C)
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYeqC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->min() == C && VarY->max() == C)
                VarX->removeSingle(0, this);
        if (!VarY->contains(C))
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeSingle(C, this);
                } else {
                        assert_Ns(VarX->value() == 1, "Ns_ConstrMetaXeqYeqC::"
                                                      "ArcCons: Wrong "
                                                      "'VarX->value()'");
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                }
        }
}

void Ns_ConstrMetaXeqYeqC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeSingle(C, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYeqC::LocalArcCons: "
                                          "Wrong 'VarX->value()'");
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_Ns(
                    VarY == Qitem.getVarFired(),
                    "Ns_ConstrMetaXeqYeqC::LocalArcCons: Wrong getVarFired");
                if (VarY->min() == C && VarY->max() == C)
                        VarX->removeSingle(0, this);
                if (!VarY->contains(C))
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrMetaXeqYneqC::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (!VarY->contains(C))
                VarX->removeSingle(0, this);
        if (VarY->min() == C && VarY->max() == C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                } else {
                        assert_Ns(VarX->value() == 1, "Ns_ConstrMetaXeqYneqC::"
                                                      "ArcCons: Wrong "
                                                      "'VarX->value()'");
                        VarY->removeSingle(C, this);
                }
        }
}

void Ns_ConstrMetaXeqYneqC::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        } else {
                                assert_Ns(VarX->value() == 1,
                                          "Ns_ConstrMetaXeqYneqC::LocalArcCons:"
                                          " Wrong 'VarX->value()'");
                                VarY->removeSingle(C, this);
                        }
                }
        } else {
                assert_Ns(
                    VarY == Qitem.getVarFired(),
                    "Ns_ConstrMetaXeqYneqC::LocalArcCons: Wrong getVarFired");
                if (!VarY->contains(C))
                        VarX->removeSingle(0, this);
                if (VarY->min() == C && VarY->max() == C)
                        VarX->removeSingle(1, this);
        }
}

void Ns_ConstrXeqYplusC::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                VarX->removeRange(NsMINUS_INF, VarY->min() + C - 1, this,
                                  modification);
                VarX->removeRange(VarY->max() + C + 1, NsPLUS_INF, this,
                                  modification);
                VarY->removeRange(NsMINUS_INF, VarX->min() - C - 1, this,
                                  modification);
                VarY->removeRange(VarX->max() - C + 1, NsPLUS_INF, this,
                                  modification);
        } while (modification);
}

void Ns_ConstrXeqYplusC::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void Ns_ConstrXeqCminusY::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                VarX->removeRange(NsMINUS_INF, C - VarY->max() - 1, this,
                                  modification);
                VarX->removeRange(C - VarY->min() + 1, NsPLUS_INF, this,
                                  modification);
                VarY->removeRange(NsMINUS_INF, C - VarX->max() - 1, this,
                                  modification);
                VarY->removeRange(C - VarX->min() + 1, NsPLUS_INF, this,
                                  modification);
        } while (modification);
}

void Ns_ConstrXeqCminusY::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void Ns_ConstrXeqYtimesC::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                if (C >= 0) {
                        VarX->removeRange(NsMINUS_INF, VarY->min() * C - 1,
                                          this, modification);
                        VarX->removeRange(VarY->max() * C + 1, NsPLUS_INF, this,
                                          modification);
                        VarY->removeRange(NsMINUS_INF, VarX->min() / C - 1,
                                          this, modification);
                        VarY->removeRange(VarX->max() / C + 1, NsPLUS_INF, this,
                                          modification);
                } else {
                        VarX->removeRange(NsMINUS_INF, VarY->max() * C - 1,
                                          this, modification);
                        VarX->removeRange(VarY->min() * C + 1, NsPLUS_INF, this,
                                          modification);
                        VarY->removeRange(NsMINUS_INF, VarX->max() / C - 1,
                                          this, modification);
                        VarY->removeRange(VarX->min() / C + 1, NsPLUS_INF, this,
                                          modification);
                }
        } while (modification);
}

void Ns_ConstrXeqYtimesC::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void Ns_ConstrXeqY::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                VarX->removeRange(NsMINUS_INF, VarY->min() - 1, this,
                                  modification);
                VarX->removeRange(VarY->max() + 1, NsPLUS_INF, this,
                                  modification);
                VarY->removeRange(NsMINUS_INF, VarX->min() - 1, this,
                                  modification);
                VarY->removeRange(VarX->max() + 1, NsPLUS_INF, this,
                                  modification);
        } while (modification);
}

void Ns_ConstrXeqY::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void Ns_ConstrXeqYplusCZspecial::ArcCons(void)
{
        NsIntVar::const_iterator v, vz;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (!VarY->contains(*v % C) ||
                    !VarZ->contains((*v - *v % C) / C))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                for (vz = VarZ->begin(); vz != VarZ->end(); ++vz) {
                        if (VarX->contains(*v + C * (*vz)))
                                break;
                }
                if (vz == VarZ->end())
                        VarY->removeSingle(*v, this);
        }
        for (vz = VarZ->begin(); vz != VarZ->end(); ++vz) {
                for (v = VarY->begin(); v != VarY->end(); ++v) {
                        if (VarX->contains(*v + C * (*vz)))
                                break;
                }
                if (v == VarY->end())
                        VarZ->removeSingle(*vz, this);
        }
}

void Ns_ConstrXeqYplusCZspecial::LocalArcCons(Ns_QueueItem& Qitem)
{
        NsIntVar::const_iterator v, vz;
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                SupportVal = Qitem.getW() / C;
                if (VarZ->contains(SupportVal)) {
                        if (VarX->next(SupportVal * C - 1) >=
                            (SupportVal + 1) * C)
                                VarZ->removeSingle(SupportVal, this);
                }
                SupportVal = Qitem.getW() % C;
                if (VarY->contains(SupportVal)) {
                        for (vz = VarZ->begin(); vz != VarZ->end(); ++vz) {
                                if (VarX->contains(SupportVal + C * (*vz)))
                                        break;
                        }
                        if (vz == VarZ->end())
                                VarY->removeSingle(SupportVal, this);
                }
        } else if (VarY == Qitem.getVarFired()) {
                for (SupportVal = C * (VarX->min() / C) + Qitem.getW();
                     SupportVal <= VarX->max(); SupportVal += C) {
                        VarX->removeSingle(SupportVal, this);
                }
                for (vz = VarZ->begin(); vz != VarZ->end(); ++vz) {
                        for (v = VarY->begin(); v != VarY->end(); ++v) {
                                if (VarX->contains(*v + C * (*vz)))
                                        break;
                        }
                        if (v == VarY->end())
                                VarZ->removeSingle(*vz, this);
                }
        } else {
                assert_Ns(VarZ == Qitem.getVarFired(),
                          "Ns_ConstrXeqYplusCZspecial::LocalArcCons: Wrong "
                          "getVarFired");
                VarX->removeRange(Qitem.getW() * C, (Qitem.getW() + 1) * C - 1,
                                  this);
                for (v = VarY->begin(); v != VarY->end(); ++v) {
                        for (vz = VarZ->begin(); vz != VarZ->end(); ++vz) {
                                if (VarX->contains(*v + C * (*vz)))
                                        break;
                        }
                        if (vz == VarZ->end())
                                VarY->removeSingle(*v, this);
                }
        }
}

void Ns_ConstrXeqYdivC::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                if (C >= 0) {
                        VarX->removeRange(NsMINUS_INF,
                                          xDIVy(VarY->min(), C) - 1, this,
                                          modification);
                        VarX->removeRange(xDIVy(VarY->max(), C) + 1, NsPLUS_INF,
                                          this, modification);
                        VarY->removeRange(NsMINUS_INF, VarX->min() * C - 1,
                                          this, modification);
                        VarY->removeRange((VarX->max() + 1) * C, NsPLUS_INF,
                                          this, modification);
                } else {
                        VarX->removeRange(NsMINUS_INF,
                                          xDIVy(VarY->max(), C) - 1, this,
                                          modification);
                        VarX->removeRange(xDIVy(VarY->min(), C) + 1, NsPLUS_INF,
                                          this, modification);
                        VarY->removeRange(NsMINUS_INF, (VarX->max() + 1) * C,
                                          this, modification);
                        VarY->removeRange(VarX->min() * C + 1, NsPLUS_INF, this,
                                          modification);
                }
        } while (modification);
}

void Ns_ConstrXeqYdivC::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void Ns_ConstrXeqCdivY::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                if (VarY->min() < 0 && 0 < VarY->max()) {
                        if (C >= 0) {
                                VarX->removeRange(NsMINUS_INF,
                                                  xDIVy(C, VarY->previous(0)) -
                                                      1,
                                                  this, modification);
                                VarX->removeRange(xDIVy(C, VarY->next(0)) + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        } else {
                                VarX->removeRange(NsMINUS_INF,
                                                  xDIVy(C, VarY->next(0)) - 1,
                                                  this, modification);
                                VarX->removeRange(
                                    xDIVy(C, VarY->previous(0)) + 1, NsPLUS_INF,
                                    this, modification);
                        }
                } else {
                        if (C >= 0) {
                                VarX->removeRange(NsMINUS_INF,
                                                  xDIVy(C, VarY->max()) - 1,
                                                  this, modification);
                                VarX->removeRange(xDIVy(C, VarY->min()) + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        } else {
                                VarX->removeRange(NsMINUS_INF,
                                                  xDIVy(C, VarY->min()) - 1,
                                                  this, modification);
                                VarX->removeRange(xDIVy(C, VarY->max()) + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        }
                }
                if (!(VarX->min() < 0 && 0 < VarX->max())) {
                        if (C >= 0) {
                                if (VarX->max() == -1)
                                        VarY->removeRange(NsMINUS_INF, -C - 1,
                                                          this, modification);
                                else
                                        VarY->removeRange(
                                            NsMINUS_INF,
                                            xDIVy(C, (VarX->max() + 1)), this,
                                            modification);
                                if (VarX->min() != 0)
                                        VarY->removeRange(
                                            xDIVy(C, VarX->min()) + 1,
                                            NsPLUS_INF, this, modification);
                        } else {
                                if (VarX->min() == -1)
                                        VarY->removeRange(C + 1, NsPLUS_INF,
                                                          this, modification);
                                else
                                        VarY->removeRange(
                                            NsMINUS_INF,
                                            xDIVy(C, (VarX->min() + 1)), this,
                                            modification);
                                if (VarX->max() != 0)
                                        VarY->removeRange(
                                            xDIVy(C, VarX->max()) + 1,
                                            NsPLUS_INF, this, modification);
                        }
                }
        } while (modification);
}

void Ns_ConstrXeqCdivY::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void YmodC_min_max(const NsIntVar* VarY, const NsInt C, NsInt& min, NsInt& max)
{
        min = NsPLUS_INF;
        max = NsMINUS_INF;
        NsInt SupValY, v;
        assert_Ns(C > 0, "YmodC_min_max: Special condition required: C > 0");
        for (SupValY = (VarY->min() / C) * C; SupValY <= VarY->max();
             SupValY += C) {
                v = VarY->next(SupValY - 1);
                if (v < SupValY + C && min > v % C)
                        min = v % C;
                v = VarY->previous(SupValY + C);
                if (v >= SupValY && max < v % C)
                        max = v % C;
                if (min == 0 && max == C - 1)
                        break; // no alterations are possible
        }
}

void Ns_ConstrXeqYmodC::ArcCons(void)
{
        NsInt min, max;
        YmodC_min_max(VarY, C, min, max);
        VarX->removeRange(NsMINUS_INF, min - 1, this);
        VarX->removeRange(max + 1, NsPLUS_INF, this);
        NsIntVar::const_iterator v;
        NsInt SupValY;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                for (SupValY = (VarY->min() / C) * C + *v;
                     SupValY <= VarY->max(); SupValY += C) {
                        if (VarY->contains(SupValY))
                                break;
                }
                if (SupValY > VarY->max())
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(*v % C))
                        VarY->removeSingle(*v, this);
        }
}

void Ns_ConstrXeqYmodC::LocalArcCons(Ns_QueueItem& Qitem)
{
        NsInt SupValX, SupValY;
        if (VarX == Qitem.getVarFired()) {
                SupValX = Qitem.getW();
                for (SupValY = (VarY->min() / C) * C + SupValX;
                     SupValY <= VarY->max(); SupValY += C) {
                        VarY->removeSingle(SupValY, this);
                }
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrXeqYmodC::LocalArcCons: Wrong getVarFired");
                SupValX = Qitem.getW() % C;
                if (VarX->contains(SupValX)) {
                        for (SupValY = (VarY->min() / C) * C + SupValX;
                             SupValY <= VarY->max(); SupValY += C) {
                                if (VarY->contains(SupValY))
                                        break;
                        }
                        if (SupValY > VarY->max())
                                VarX->removeSingle(SupValX, this);
                }
        }
}

void Ns_ConstrXneqY::ArcCons(void)
{
        if (VarY->isBound())
                VarX->removeSingle(VarY->value(), this);
        if (VarX->isBound())
                VarY->removeSingle(VarX->value(), this);
}

void Ns_ConstrXneqY::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound())
                        VarY->removeSingle(VarX->value(), this);
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrXneqY::LocalArcCons: Wrong getVarFired");
                if (VarY->isBound())
                        VarX->removeSingle(VarY->value(), this);
        }
}

void Ns_ConstrXeqAbsY::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                VarX->removeRange(
                    NsMINUS_INF,
                    max(labs(max(VarY->min(), static_cast<NsInt>(0))),
                        labs(min(static_cast<NsInt>(0), VarY->max()))) -
                        1,
                    this, modification);
                VarX->removeRange(max(labs(VarY->min()), labs(VarY->max())) + 1,
                                  NsPLUS_INF, this, modification);
                VarY->removeRange(NsMINUS_INF, -(VarX->max() + 1), this,
                                  modification);
                VarY->removeRange(+(VarX->max() + 1), NsPLUS_INF, this,
                                  modification);
        } while (modification);
}

void Ns_ConstrXeqAbsY::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

bool intersectionEmpty(const NsIntVar* VarY, const NsIntVar* VarZ)
{
        return (VarY->min() > VarZ->max() || VarY->max() < VarZ->min());
}

void Ns_ConstrMetaXeqYeqZ::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->max() == VarZ->min() && VarY->min() == VarZ->max())
                VarX->removeSingle(neg, this);
        if (intersectionEmpty(VarY, VarZ))
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        if (VarZ->isBound())
                                VarY->removeSingle(VarZ->value(), this);
                        if (VarY->isBound())
                                VarZ->removeSingle(VarY->value(), this);
                } else {
                        assert_Ns(
                            VarX->value() == !neg,
                            "MetaXeqYeqneqZArcCons: Wrong 'VarX->value()'");
                        VarY->removeRange(NsMINUS_INF, VarZ->min() - 1, this);
                        VarY->removeRange(VarZ->max() + 1, NsPLUS_INF, this);
                        VarZ->removeRange(NsMINUS_INF, VarY->min() - 1, this);
                        VarZ->removeRange(VarY->max() + 1, NsPLUS_INF, this);
                }
        }
}

void Ns_ConstrMetaXeqYeqZ::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                if (VarZ->isBound())
                                        VarY->removeSingle(VarZ->value(), this);
                                if (VarY->isBound())
                                        VarZ->removeSingle(VarY->value(), this);
                        } else {
                                assert_Ns(VarX->value() == !neg,
                                          "Ns_ConstrMetaXeqYeqZ::LocalArcCons: "
                                          "Wrong 'VarX->value()'");
                                VarY->removeRange(NsMINUS_INF, VarZ->min() - 1,
                                                  this);
                                VarY->removeRange(VarZ->max() + 1, NsPLUS_INF,
                                                  this);
                                VarZ->removeRange(NsMINUS_INF, VarY->min() - 1,
                                                  this);
                                VarZ->removeRange(VarY->max() + 1, NsPLUS_INF,
                                                  this);
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == neg) {
                                        if (VarY->isBound())
                                                VarZ->removeSingle(
                                                    VarY->value(), this);
                                } else {
                                        assert_Ns(VarX->value() == !neg,
                                                  "Ns_ConstrMetaXeqYeqZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarY->removeRange(
                                            NsMINUS_INF, VarZ->min() - 1, this);
                                        VarY->removeRange(VarZ->max() + 1,
                                                          NsPLUS_INF, this);
                                        VarZ->removeRange(
                                            NsMINUS_INF, VarY->min() - 1, this);
                                        VarZ->removeRange(VarY->max() + 1,
                                                          NsPLUS_INF, this);
                                }
                        }
                } else {
                        assert_Ns(VarZ == Qitem.getVarFired(),
                                  "Ns_ConstrMetaXeqYeqZ::LocalArcCons: Wrong "
                                  "getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == neg) {
                                        if (VarZ->isBound())
                                                VarY->removeSingle(
                                                    VarZ->value(), this);
                                } else {
                                        assert_Ns(VarX->value() == !neg,
                                                  "Ns_ConstrMetaXeqYeqZ::"
                                                  "LocalArcCons: Wrong "
                                                  "'VarX->value()'");
                                        VarY->removeRange(
                                            NsMINUS_INF, VarZ->min() - 1, this);
                                        VarY->removeRange(VarZ->max() + 1,
                                                          NsPLUS_INF, this);
                                        VarZ->removeRange(
                                            NsMINUS_INF, VarY->min() - 1, this);
                                        VarZ->removeRange(VarY->max() + 1,
                                                          NsPLUS_INF, this);
                                }
                        }
                }
                if (VarY->max() == VarZ->min() && VarY->min() == VarZ->max())
                        VarX->removeSingle(neg, this);
                if (intersectionEmpty(VarY, VarZ))
                        VarX->removeSingle(!neg, this);
        }
}

void Ns_ConstrXeqYandZ::ArcCons(void)
{
        makeBoolean(VarX, this);
        if ((VarY->min() * VarZ->min()))
                VarX->removeSingle(neg, this);
        if (!(VarY->max() * VarZ->max()))
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        // VOID
                } else {
                        assert_Ns(VarX->value() == !neg, "Ns_ConstrXeqYandZ::"
                                                         "LocalArcCons: Wrong "
                                                         "'VarX->value()'");
                        VarY->removeSingle(0, this);
                        VarZ->removeSingle(0, this);
                }
        }
}

void Ns_ConstrXeqYandZ::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                // VOID
                        } else {
                                assert_Ns(VarX->value() == !neg,
                                          "Ns_ConstrXeqYandZ::LocalArcCons: "
                                          "Wrong 'VarX->value()'");
                                VarY->removeSingle(0, this);
                                VarZ->removeSingle(0, this);
                        }
                }
        } else {
                if ((VarY->min() * VarZ->min()))
                        VarX->removeSingle(neg, this);
                if (!(VarY->max() * VarZ->max()))
                        VarX->removeSingle(!neg, this);
        }
}

void Ns_ConstrXeqYorZ::ArcCons(void)
{
        makeBoolean(VarX, this);
        if (VarY->min() + VarZ->min() != 0)
                VarX->removeSingle(neg, this);
        if (VarY->max() + VarZ->max() == 0)
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        VarY->removeSingle(1, this);
                        VarZ->removeSingle(1, this);
                } else {
                        assert_Ns(VarX->value() == !neg, "Ns_ConstrXeqYorZ::"
                                                         "LocalArcCons: Wrong "
                                                         "'VarX->value()'");
                        // VOID
                }
        }
}

void Ns_ConstrXeqYorZ::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                VarY->removeSingle(1, this);
                                VarZ->removeSingle(1, this);
                        } else {
                                assert_Ns(VarX->value() == !neg,
                                          "Ns_ConstrXeqYorZ::LocalArcCons: "
                                          "Wrong 'VarX->value()'");
                                // VOID
                        }
                }
        } else {
                if (VarY->min() + VarZ->min() != 0)
                        VarX->removeSingle(neg, this);
                if (VarY->max() + VarZ->max() == 0)
                        VarX->removeSingle(!neg, this);
        }
}

void Ns_ConstrXorY::ArcCons(void)
{
        if (VarY->isBound() && VarY->value() == neg)
                VarX->removeSingle(neg, this);
        if (VarX->isBound() && VarX->value() == neg)
                VarY->removeSingle(neg, this);
}

void Ns_ConstrXorY::LocalArcCons(Ns_QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound() && VarX->value() == neg)
                        VarY->removeSingle(neg, this);
        } else {
                assert_Ns(VarY == Qitem.getVarFired(),
                          "Ns_ConstrXorY::LocalArcCons: Wrong getVarFired");
                if (VarY->isBound() && VarY->value() == neg)
                        VarX->removeSingle(neg, this);
        }
}

// bounds-consistency only
void Ns_ConstrXeqYplusZ::ArcCons(void)
{
        bool changed_summinmax = true;
        for (;;) {
                do {
                        if (!VarX->removeRange(NsMINUS_INF,
                                               VarY->min() + VarZ->min() - 1,
                                               this))
                                return;
                        if (VarY->min() + VarZ->max() < VarX->min()) {
                                if (!VarY->removeRange(
                                        NsMINUS_INF,
                                        -VarZ->max() + VarX->min() - 1, this))
                                        return;
                                changed_summinmax = true;
                        }
                        if (VarZ->min() + VarY->max() < VarX->min()) {
                                if (!VarZ->removeRange(
                                        NsMINUS_INF,
                                        -VarY->max() + VarX->min() - 1, this))
                                        return;
                                changed_summinmax = true;
                        }
                } while (VarX->min() < VarY->min() + VarZ->min());
                // Initially 'changed_summinmax' was intentionally set true, in
                // order the following 'if' statement to be ignored, the first
                // time it is executed.
                if (!changed_summinmax)
                        break;
                changed_summinmax = false;
                do {
                        if (!VarX->removeRange(VarY->max() + VarZ->max() + 1,
                                               NsPLUS_INF, this))
                                return;
                        if (VarY->max() + VarZ->min() > VarX->max()) {
                                if (!VarY->removeRange(-VarZ->min() +
                                                           VarX->max() + 1,
                                                       NsPLUS_INF, this))
                                        return;
                                changed_summinmax = true;
                        }
                        if (VarZ->max() + VarY->min() > VarX->max()) {
                                if (!VarZ->removeRange(-VarY->min() +
                                                           VarX->max() + 1,
                                                       NsPLUS_INF, this))
                                        return;
                                changed_summinmax = true;
                        }
                } while (VarX->max() > VarY->max() + VarZ->max());
                if (!changed_summinmax)
                        break;
                changed_summinmax = false;
        }
}

void Ns_ConstrXeqYplusZ::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void product_min_max(const NsIntVar* VarY, const NsIntVar* VarZ, NsInt& min,
                     NsInt& max)
{
        NsInt prod;
        prod = VarY->min() * VarZ->min();
        min = max = prod;
        prod = VarY->max() * VarZ->min();
        if (prod < min)
                min = prod;
        if (prod > max)
                max = prod;
        prod = VarY->min() * VarZ->max();
        if (prod < min)
                min = prod;
        if (prod > max)
                max = prod;
        prod = VarY->max() * VarZ->max();
        if (prod < min)
                min = prod;
        if (prod > max)
                max = prod;
}

namespace {

void product_prune_bound(NsIntVar* VarX, NsIntVar* VarY, NsIntVar* VarZ,
                         bool& changed_minmax, const Ns_Constraint* constraint)
{
        while ((VarY->min() * VarZ->min() < VarX->min() &&
                VarY->min() * VarZ->max() < VarX->min()) ||
               (VarY->min() * VarZ->min() > VarX->max() &&
                VarY->min() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->min(), constraint))
                        return; // to avoid an infinite loop
                changed_minmax = true;
        }
        while ((VarY->max() * VarZ->min() < VarX->min() &&
                VarY->max() * VarZ->max() < VarX->min()) ||
               (VarY->max() * VarZ->min() > VarX->max() &&
                VarY->max() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->max(), constraint))
                        return; // to avoid an infinite loop
                changed_minmax = true;
        }
}

} // namespace

void Ns_ConstrXeqYtimesZ::ArcCons(void)
{
        NsInt min, max;
        bool changed_minmax;
        do {
                changed_minmax = false;
                product_min_max(VarY, VarZ, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                product_prune_bound(VarX, VarY, VarZ, changed_minmax, this);
                product_prune_bound(VarX, VarZ, VarY, changed_minmax, this);
        } while (changed_minmax);
}

void Ns_ConstrXeqYtimesZ::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void quotient_min_max(const NsIntVar* VarY, NsIntVar* VarZ, NsInt& min,
                      NsInt& max)
{
        VarZ->remove(0);
        assert_Ns(!(VarZ->isBound() && VarZ->value() == 0),
                  "quotient_min_max: zero 'VarZ'");
        NsInt quotient;
        quotient = VarY->min() / VarZ->min();
        min = max = quotient;
        quotient = VarY->max() / VarZ->min();
        if (quotient < min)
                min = quotient;
        if (quotient > max)
                max = quotient;
        quotient = VarY->min() / VarZ->max();
        if (quotient < min)
                min = quotient;
        if (quotient > max)
                max = quotient;
        quotient = VarY->max() / VarZ->max();
        if (quotient < min)
                min = quotient;
        if (quotient > max)
                max = quotient;
        NsInt divisor;
        if ((divisor = VarZ->previous(0)) != NsMINUS_INF) {
                quotient = VarY->min() / divisor;
                if (quotient < min)
                        min = quotient;
                if (quotient > max)
                        max = quotient;
                quotient = VarY->max() / divisor;
                if (quotient < min)
                        min = quotient;
                if (quotient > max)
                        max = quotient;
        }
        if ((divisor = VarZ->next(0)) != NsPLUS_INF) {
                quotient = VarY->min() / divisor;
                if (quotient < min)
                        min = quotient;
                if (quotient > max)
                        max = quotient;
                quotient = VarY->max() / divisor;
                if (quotient < min)
                        min = quotient;
                if (quotient > max)
                        max = quotient;
        }
}
