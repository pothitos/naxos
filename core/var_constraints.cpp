/// @file
/// Algorithms that impose the constraints over variables
///
/// Part of https://github.com/pothitos/naxos

#include "internal.h"
#include "naxos-mini.h"
#include <algorithm>
#include <cstdlib>

using namespace naxos;
using namespace std;

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

} // end namespace

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

void Ns_ConstrXeqYdivC::ArcCons(void)
{
        bool modification;
        do {
                modification = false;
                if (C >= 0) {
                        VarX->removeRange(NsMINUS_INF, VarY->min() / C - 1,
                                          this, modification);
                        VarX->removeRange(VarY->max() / C + 1, NsPLUS_INF, this,
                                          modification);
                        VarY->removeRange(NsMINUS_INF, VarX->min() * C - 1,
                                          this, modification);
                        VarY->removeRange((VarX->max() + 1) * C, NsPLUS_INF,
                                          this, modification);
                } else {
                        VarX->removeRange(NsMINUS_INF, VarY->max() / C - 1,
                                          this, modification);
                        VarX->removeRange(VarY->min() / C + 1, NsPLUS_INF, this,
                                          modification);
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
                                                  C / VarY->previous(0) - 1,
                                                  this, modification);
                                VarX->removeRange(C / VarY->next(0) + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        } else {
                                VarX->removeRange(NsMINUS_INF,
                                                  C / VarY->next(0) - 1, this,
                                                  modification);
                                VarX->removeRange(C / VarY->previous(0) + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        }
                } else {
                        if (C >= 0) {
                                VarX->removeRange(NsMINUS_INF,
                                                  C / VarY->max() - 1, this,
                                                  modification);
                                VarX->removeRange(C / VarY->min() + 1,
                                                  NsPLUS_INF, this,
                                                  modification);
                        } else {
                                VarX->removeRange(NsMINUS_INF,
                                                  C / VarY->min() - 1, this,
                                                  modification);
                                VarX->removeRange(C / VarY->max() + 1,
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
                                        VarY->removeRange(NsMINUS_INF,
                                                          C / (VarX->max() + 1),
                                                          this, modification);
                                if (VarX->min() != 0)
                                        VarY->removeRange(C / VarX->min() + 1,
                                                          NsPLUS_INF, this,
                                                          modification);
                        } else {
                                if (VarX->min() == -1)
                                        VarY->removeRange(C + 1, NsPLUS_INF,
                                                          this, modification);
                                else
                                        VarY->removeRange(NsMINUS_INF,
                                                          C / (VarX->min() + 1),
                                                          this, modification);
                                if (VarX->max() != 0)
                                        VarY->removeRange(C / VarX->max() + 1,
                                                          NsPLUS_INF, this,
                                                          modification);
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
        bool modification;
        do {
                modification = false;
                YmodC_min_max(VarY, C, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                for (NsIntVar::const_iterator val = VarY->begin();
                     val != VarY->end(); ++val) {
                        if (VarX->contains(*val % C))
                                break;
                        VarY->removeSingle(*val, this);
                        modification = true;
                }
                for (NsIntVar::const_reverse_iterator val = VarY->rbegin();
                     val != VarY->rend(); ++val) {
                        if (VarX->contains(*val % C))
                                break;
                        VarY->removeSingle(*val, this);
                        modification = true;
                }
        } while (modification);
}

void Ns_ConstrXeqYmodC::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void update_min_max(const NsInt candidate, NsInt& min, NsInt& max)
{
        if (candidate < min)
                min = candidate;
        if (candidate > max)
                max = candidate;
}

void CmodY_min_max(const NsInt C, NsIntVar* VarY, NsInt& min, NsInt& max)
{
        VarY->remove(0);
        assert_Ns(!(VarY->isBound() && VarY->value() == 0),
                  "Zero divisor in modulus constraint");
        min = NsPLUS_INF;
        max = NsMINUS_INF;
        update_min_max(C % VarY->min(), min, max);
        update_min_max(C % VarY->max(), min, max);
        if (VarY->previous(0) != NsMINUS_INF)
                update_min_max(C % VarY->previous(0), min, max);
        if (VarY->next(0) != NsPLUS_INF)
                update_min_max(C % VarY->next(0), min, max);
}

void Ns_ConstrXeqCmodY::ArcCons(void)
{
        NsInt min, max;
        bool modification;
        do {
                modification = false;
                CmodY_min_max(C, VarY, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                for (NsIntVar::const_iterator val = VarY->begin();
                     val != VarY->end(); ++val) {
                        if (VarX->contains(C % *val))
                                break;
                        VarY->removeSingle(*val, this);
                        modification = true;
                }
                for (NsIntVar::const_reverse_iterator val = VarY->rbegin();
                     val != VarY->rend(); ++val) {
                        if (VarX->contains(C % *val))
                                break;
                        VarY->removeSingle(*val, this);
                        modification = true;
                }
        } while (modification);
}

void Ns_ConstrXeqCmodY::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
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
        if ((VarY->min() && VarZ->min()))
                VarX->removeSingle(neg, this);
        if (!(VarY->max() && VarZ->max()))
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
                if ((VarY->min() && VarZ->min()))
                        VarX->removeSingle(neg, this);
                if (!(VarY->max() && VarZ->max()))
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

void product_prune_bounds(NsIntVar* VarX, NsIntVar* VarY, NsIntVar* VarZ,
                          bool& modification, const Ns_Constraint* constraint)
{
        while ((VarY->min() * VarZ->min() < VarX->min() &&
                VarY->min() * VarZ->max() < VarX->min()) ||
               (VarY->min() * VarZ->min() > VarX->max() &&
                VarY->min() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->min(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
        while ((VarY->max() * VarZ->min() < VarX->min() &&
                VarY->max() * VarZ->max() < VarX->min()) ||
               (VarY->max() * VarZ->min() > VarX->max() &&
                VarY->max() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->max(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
}

} // end namespace

void Ns_ConstrXeqYtimesZ::ArcCons(void)
{
        NsInt min, max;
        bool modification;
        do {
                modification = false;
                product_min_max(VarY, VarZ, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                product_prune_bounds(VarX, VarY, VarZ, modification, this);
                product_prune_bounds(VarX, VarZ, VarY, modification, this);
        } while (modification);
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

namespace {

bool dividend_is_out_of_bounds(NsIntVar* VarX, NsInt valueY, NsIntVar* VarZ)
{
        NsInt VarZ_min_positive = VarZ->next(0);
        NsInt VarZ_max_negative = VarZ->previous(0);
        return ((valueY / VarZ->min() < VarX->min() &&
                 valueY / VarZ->max() < VarX->min() &&
                 (VarZ_max_negative == NsMINUS_INF ||
                  valueY / VarZ_max_negative < VarX->min()) &&
                 (VarZ_min_positive == NsPLUS_INF ||
                  valueY / VarZ_min_positive < VarX->min())) ||
                (valueY / VarZ->min() > VarX->max() &&
                 valueY / VarZ->max() > VarX->max() &&
                 (VarZ_max_negative == NsMINUS_INF ||
                  valueY / VarZ_max_negative > VarX->max()) &&
                 (VarZ_min_positive == NsPLUS_INF ||
                  valueY / VarZ_min_positive > VarX->max())));
}

void dividend_prune_bounds(NsIntVar* VarX, NsIntVar* VarY, NsIntVar* VarZ,
                           bool& modification, const Ns_Constraint* constraint)
{
        while (dividend_is_out_of_bounds(VarX, VarY->min(), VarZ)) {
                if (!VarY->removeSingle(VarY->min(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
        while (dividend_is_out_of_bounds(VarX, VarY->max(), VarZ)) {
                if (!VarY->removeSingle(VarY->max(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
}

void divisor_prune_bounds(NsIntVar* VarX, NsIntVar* VarY, NsIntVar* VarZ,
                          bool& modification, const Ns_Constraint* constraint)
{
        while ((VarY->min() / VarZ->min() < VarX->min() &&
                VarY->max() / VarZ->min() < VarX->min()) ||
               (VarY->min() / VarZ->min() > VarX->max() &&
                VarY->max() / VarZ->min() > VarX->max())) {
                if (!VarZ->removeSingle(VarZ->min(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
        while ((VarY->min() / VarZ->max() < VarX->min() &&
                VarY->max() / VarZ->max() < VarX->min()) ||
               (VarY->min() / VarZ->max() > VarX->max() &&
                VarY->max() / VarZ->max() > VarX->max())) {
                if (!VarZ->removeSingle(VarZ->max(), constraint))
                        return; // to avoid an infinite loop
                modification = true;
        }
}

} // end namespace

void Ns_ConstrXeqYdivZ::ArcCons(void)
{
        NsInt min, max;
        bool modification;
        do {
                modification = false;
                quotient_min_max(VarY, VarZ, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                dividend_prune_bounds(VarX, VarY, VarZ, modification, this);
                divisor_prune_bounds(VarX, VarY, VarZ, modification, this);
        } while (modification);
}

void Ns_ConstrXeqYdivZ::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}

void remainder_min_max(const NsIntVar* VarY, NsIntVar* VarZ, NsInt& min_rem,
                       NsInt& max_rem)
{
        // Divisor must not be zero
        VarZ->remove(0);
        assert_Ns(!(VarZ->isBound() && VarZ->value() == 0),
                  "remainder_min_max: zero 'VarZ'");
        // Get the minimum absolute divisor value
        NsInt min_abs_divisor = VarZ->next(0);
        if (VarZ->previous(0) != NsMINUS_INF &&
            -VarZ->previous(0) < min_abs_divisor) {
                min_abs_divisor = -VarZ->previous(0);
        }
        // Get the maximum absolute divisor value
        NsInt max_abs_divisor = max(labs(VarZ->min()), labs(VarZ->max()));
        // Iterate through VarY values and get the min/max VarY % VarZ
        min_rem = NsPLUS_INF;
        max_rem = NsMINUS_INF;
        for (NsIntVar::const_iterator val = VarY->begin(); val != VarY->end();
             ++val) {
                if (*val % min_abs_divisor < min_rem)
                        min_rem = *val % min_abs_divisor;
                if (*val % max_abs_divisor > max_rem)
                        max_rem = *val % max_abs_divisor;
        }
}

namespace {

void dividend_for_mod_prune_bounds(NsIntVar* VarX, NsIntVar* VarY,
                                   NsIntVar* VarZ, bool& modification,
                                   const Ns_Constraint* constraint)
{
        NsIntVar::const_iterator valZ;
        for (NsIntVar::const_iterator valY = VarY->begin(); valY != VarY->end();
             ++valY) {
                for (valZ = VarZ->begin(); valZ != VarZ->end(); ++valZ) {
                        if (VarX->contains(*valY % *valZ))
                                break;
                }
                if (valZ == VarZ->end()) {
                        VarY->removeSingle(*valY, constraint);
                        modification = true;
                }
        }
        for (NsIntVar::const_reverse_iterator valY = VarY->rbegin();
             valY != VarY->rend(); ++valY) {
                for (valZ = VarZ->begin(); valZ != VarZ->end(); ++valZ) {
                        if (VarX->contains(*valY % *valZ))
                                break;
                }
                if (valZ == VarZ->end()) {
                        VarY->removeSingle(*valY, constraint);
                        modification = true;
                }
        }
}

void divisor_for_mod_prune_bounds(NsIntVar* VarX, NsIntVar* VarY,
                                  NsIntVar* VarZ, bool& modification,
                                  const Ns_Constraint* constraint)
{
        NsIntVar::const_iterator valY;
        for (NsIntVar::const_iterator valZ = VarZ->begin(); valZ != VarZ->end();
             ++valZ) {
                for (valY = VarY->begin(); valY != VarY->end(); ++valY) {
                        if (VarX->contains(*valY % *valZ))
                                break;
                }
                if (valY == VarY->end()) {
                        VarZ->removeSingle(*valZ, constraint);
                        modification = true;
                }
        }
        for (NsIntVar::const_reverse_iterator valZ = VarZ->rbegin();
             valZ != VarZ->rend(); ++valZ) {
                for (valY = VarY->begin(); valY != VarY->end(); ++valY) {
                        if (VarX->contains(*valY % *valZ))
                                break;
                }
                if (valY == VarY->end()) {
                        VarZ->removeSingle(*valZ, constraint);
                        modification = true;
                }
        }
}

} // end namespace

void Ns_ConstrXeqYmodZ::ArcCons(void)
{
        NsInt min, max;
        bool modification;
        do {
                modification = false;
                remainder_min_max(VarY, VarZ, min, max);
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                dividend_for_mod_prune_bounds(VarX, VarY, VarZ, modification,
                                              this);
                divisor_for_mod_prune_bounds(VarX, VarY, VarZ, modification,
                                             this);
        } while (modification);
}

void Ns_ConstrXeqYmodZ::LocalArcCons(Ns_QueueItem& /*Qitem*/)
{
        ArcCons();
}
