// Part of https://github.com/pothitos/naxos

#include "naxos.h"
#include <algorithm>
#include <cstdlib>

using namespace naxos;
using namespace std;

void ConstrXlessthanY::ArcCons(void)
{
        // while ( VarX->max() >= VarY->max() )   {
        //	if ( !VarX->removeSingle( VarX->max(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(VarY->max(), NsPLUS_INF, this);
        // while ( VarX->min() >= VarY->min() )   {
        //	if ( !VarY->removeSingle( VarY->min(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarY->removeRange(NsMINUS_INF, VarX->min(), this);
}

void ConstrXlessthanY::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                // while ( VarX->min() >= VarY->min() )   {
                //	if ( !VarY->removeSingle( VarY->min(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarY->removeRange(NsMINUS_INF, VarX->min(), this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXlessthanY::LocalArcCons: Wrong getVarFired");
                // while ( VarX->max() >= VarY->max() )   {
                //	if ( !VarX->removeSingle( VarX->max(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarX->removeRange(VarY->max(), NsPLUS_INF, this);
        }
}

void ConstrXlesseqthanY::ArcCons(void)
{
        // while ( VarX->max() > VarY->max() )   {
        //	if ( !VarX->removeSingle( VarX->max(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(VarY->max() + 1, NsPLUS_INF, this);
        // while ( VarX->min() > VarY->min() )   {
        //	if ( !VarY->removeSingle( VarY->min(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarY->removeRange(NsMINUS_INF, VarX->min() - 1, this);
}

void ConstrXlesseqthanY::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                // while ( VarX->min() > VarY->min() )   {
                //	if ( !VarY->removeSingle( VarY->min(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarY->removeRange(NsMINUS_INF, VarX->min() - 1, this);
        } else {
                assert_exc(
                    VarY == Qitem.getVarFired(),
                    "ConstrXlesseqthanY::LocalArcCons: Wrong getVarFired");
                // while ( VarX->max() > VarY->max() )   {
                //	if ( !VarX->removeSingle( VarX->max(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarX->removeRange(VarY->max() + 1, NsPLUS_INF, this);
        }
}

namespace {
void MakeBoolean(NsIntVar* VarX, const NsConstraint* constraint)
{
        // while ( VarX->min() < 0 )   {
        //	if ( !VarX->removeSingle( VarX->min(), constraint ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(NsMINUS_INF, -1, constraint);
        // while ( VarX->max() > 1 )   {
        //	if ( !VarX->removeSingle( VarX->max(), constraint ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(2, NsPLUS_INF, constraint);
}
} // end namespace

void ConstrMetaXeqYlessthanC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->max() < C)
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->min() >= C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarY->min() < C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYlessthanC"
                                                       "::ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->max() >= C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C, NsPLUS_INF, this);
                }
        }
}

void ConstrMetaXeqYlessthanC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarY->min() < C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYlessthanC::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->max() >= C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_exc(
                    VarY == Qitem.getVarFired(),
                    "ConstrMetaXeqYlessthanC::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(0) && VarY->max() < C)
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->min() >= C)
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYlessthanZ::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->max() < VarZ->min())
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->min() >= VarZ->max())
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarZ->max() > VarY->max() )   {
                        //	if ( !VarZ->removeSingle( VarZ->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarZ->removeRange(VarY->max() + 1, NsPLUS_INF, this);
                        // while ( VarZ->min() > VarY->min() )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, VarZ->min() - 1, this);
                } else {
                        // assert_exc(VarX->value() == 1,
                        // "ConstrMetaXeqYlessthanZ::ArcCons: Wrong
                        // `VarX->value()'");  while ( VarY->max() >= VarZ->max()
                        // )   { 	if ( !VarY->removeSingle( VarY->max(),
                        //this ) ) 		return;	// ... to avoid an
                        //infinite loop
                        //}
                        VarY->removeRange(VarZ->max(), NsPLUS_INF, this);
                        // while ( VarY->min() >= VarZ->min() )   {
                        //	if ( !VarZ->removeSingle( VarZ->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarZ->removeRange(NsMINUS_INF, VarY->min(), this);
                }
        }
}

void ConstrMetaXeqYlessthanZ::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarZ->max() > VarY->max() )   {
                                //	if ( !VarZ->removeSingle( VarZ->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarZ->removeRange(VarY->max() + 1, NsPLUS_INF,
                                                  this);
                                // while ( VarZ->min() > VarY->min() )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, VarZ->min() - 1,
                                                  this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYlessthanZ::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->max() >= VarZ->max() )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(VarZ->max(), NsPLUS_INF,
                                                  this);
                                // while ( VarY->min() >= VarZ->min() )   {
                                //	if ( !VarZ->removeSingle( VarZ->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarZ->removeRange(NsMINUS_INF, VarY->min(),
                                                  this);
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        // while ( VarZ->max() > VarY->max() ) {
                                        //	if ( !VarZ->removeSingle(
                                        //VarZ->max(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarZ->removeRange(VarY->max() + 1,
                                                          NsPLUS_INF, this);
                                } else {
                                        assert_exc(VarX->value() == 1,
                                                   "ConstrMetaXeqYlessthanZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        // while ( VarY->min() >= VarZ->min() )
                                        // { 	if ( !VarZ->removeSingle(
                                        //VarZ->min(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarZ->removeRange(NsMINUS_INF,
                                                          VarY->min(), this);
                                }
                        }
                } else {
                        assert_exc(VarZ == Qitem.getVarFired(),
                                   "ConstrMetaXeqYlessthanZ::LocalArcCons: "
                                   "Wrong getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        // while ( VarZ->min() > VarY->min() ) {
                                        //	if ( !VarY->removeSingle(
                                        //VarY->min(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarY->removeRange(
                                            NsMINUS_INF, VarZ->min() - 1, this);
                                } else {
                                        assert_exc(VarX->value() == 1,
                                                   "ConstrMetaXeqYlessthanZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        // while ( VarY->max() >= VarZ->max() )
                                        // { 	if ( !VarY->removeSingle(
                                        //VarY->max(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarY->removeRange(VarZ->max(),
                                                          NsPLUS_INF, this);
                                }
                        }
                }
                if (VarX->contains(0) && VarY->max() < VarZ->min())
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->min() >= VarZ->max())
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYlesseqthanC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->max() <= C)
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->min() > C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarY->min() <= C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYlesseqtha"
                                                       "nC::ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->max() > C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                }
        }
}

void ConstrMetaXeqYlesseqthanC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarY->min() <= C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYlesseqthanC::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->max() > C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_exc(VarY == Qitem.getVarFired(), "ConstrMetaXeqYlesseqth"
                                                        "anC::LocalArcCons: "
                                                        "Wrong getVarFired");
                if (VarX->contains(0) && VarY->max() <= C)
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->min() > C)
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYlesseqthanZ::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->max() <= VarZ->min())
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->min() > VarZ->max())
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarZ->max() >= VarY->max() )   {
                        //	if ( !VarZ->removeSingle( VarZ->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarZ->removeRange(VarY->max(), NsPLUS_INF, this);
                        // while ( VarZ->min() >= VarY->min() )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, VarZ->min(), this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYlesseqtha"
                                                       "nZ::ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->max() > VarZ->max() )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(VarZ->max() + 1, NsPLUS_INF, this);
                        // while ( VarY->min() > VarZ->min() )   {
                        //	if ( !VarZ->removeSingle( VarZ->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarZ->removeRange(NsMINUS_INF, VarY->min() - 1, this);
                }
        }
}

void ConstrMetaXeqYlesseqthanZ::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarZ->max() >= VarY->max() )   {
                                //	if ( !VarZ->removeSingle( VarZ->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarZ->removeRange(VarY->max(), NsPLUS_INF,
                                                  this);
                                // while ( VarZ->min() >= VarY->min() )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, VarZ->min(),
                                                  this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYlesseqthanZ::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->max() > VarZ->max() )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(VarZ->max() + 1, NsPLUS_INF,
                                                  this);
                                // while ( VarY->min() > VarZ->min() )   {
                                //	if ( !VarZ->removeSingle( VarZ->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarZ->removeRange(NsMINUS_INF, VarY->min() - 1,
                                                  this);
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        // while ( VarZ->max() >= VarY->max() )
                                        // { 	if ( !VarZ->removeSingle(
                                        //VarZ->max(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarZ->removeRange(VarY->max(),
                                                          NsPLUS_INF, this);
                                } else {
                                        assert_exc(VarX->value() == 1,
                                                   "ConstrMetaXeqYlesseqthanZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        // while ( VarY->min() > VarZ->min() ) {
                                        //	if ( !VarZ->removeSingle(
                                        //VarZ->min(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarZ->removeRange(
                                            NsMINUS_INF, VarY->min() - 1, this);
                                }
                        }
                } else {
                        assert_exc(VarZ == Qitem.getVarFired(),
                                   "ConstrMetaXeqYlesseqthanZ::LocalArcCons: "
                                   "Wrong getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == 0) {
                                        // while ( VarZ->min() >= VarY->min() )
                                        // { 	if ( !VarY->removeSingle(
                                        //VarY->min(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarY->removeRange(NsMINUS_INF,
                                                          VarZ->min(), this);
                                } else {
                                        assert_exc(VarX->value() == 1,
                                                   "ConstrMetaXeqYlesseqthanZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        // while ( VarY->max() > VarZ->max() ) {
                                        //	if ( !VarY->removeSingle(
                                        //VarY->max(), this ) )
                                        //		return;	// ... to
                                        //avoid an infinite loop
                                        //}
                                        VarY->removeRange(VarZ->max() + 1,
                                                          NsPLUS_INF, this);
                                }
                        }
                }
                if (VarX->contains(0) && VarY->max() <= VarZ->min())
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->min() > VarZ->max())
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYgreaterthanC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->min() > C)
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->max() <= C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarY->max() > C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYgreaterth"
                                                       "anC::ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->min() <= C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C, this);
                }
        }
}

void ConstrMetaXeqYgreaterthanC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarY->max() > C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYgreaterthanC::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->min() <= C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C, this);
                        }
                }
        } else {
                assert_exc(VarY == Qitem.getVarFired(), "ConstrMetaXeqYgreatert"
                                                        "hanC::LocalArcCons: "
                                                        "Wrong getVarFired");
                if (VarX->contains(0) && VarY->min() > C)
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->max() <= C)
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYgreatereqthanC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && VarY->min() >= C)
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && VarY->max() < C)
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarY->max() >= C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C, NsPLUS_INF, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYgreatereq"
                                                       "thanC::ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->min() < C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                }
        }
}

void ConstrMetaXeqYgreatereqthanC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarY->max() >= C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C, NsPLUS_INF, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYgreatereqthanC::"
                                           "LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // while ( VarY->min() < C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                        }
                }
        } else {
                assert_exc(VarY == Qitem.getVarFired(), "ConstrMetaXeqYgreatere"
                                                        "qthanC::LocalArcCons: "
                                                        "Wrong getVarFired");
                if (VarX->contains(0) && VarY->min() >= C)
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && VarY->max() < C)
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYeqC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && (VarY->min() == C && VarY->max() == C))
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && !VarY->contains(C))
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        if (VarY->contains(C))
                                VarY->removeSingle(C, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYeqC::"
                                                       "ArcCons: Wrong "
                                                       "`VarX->value()'");
                        // while ( VarY->min() < C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                        // while ( VarY->max() > C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                }
        }
}

void ConstrMetaXeqYeqC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                if (VarY->contains(C))
                                        VarY->removeSingle(C, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYeqC::LocalArcCons: "
                                           "Wrong `VarX->value()'");
                                // while ( VarY->min() < C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                                // while ( VarY->max() > C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        }
                }
        } else {
                assert_exc(
                    VarY == Qitem.getVarFired(),
                    "ConstrMetaXeqYeqC::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(0) && (VarY->min() == C && VarY->max() == C))
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && !VarY->contains(C))
                        VarX->removeSingle(1, this);
        }
}

void ConstrMetaXeqYneqC::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(0) && !VarY->contains(C))
                VarX->removeSingle(0, this);
        if (VarX->contains(1) && (VarY->min() == C && VarY->max() == C))
                VarX->removeSingle(1, this);
        if (VarX->isBound()) {
                if (VarX->value() == 0) {
                        // while ( VarY->min() < C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, C - 1, this);
                        // while ( VarY->max() > C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                } else {
                        assert_exc(VarX->value() == 1, "ConstrMetaXeqYneqC::"
                                                       "ArcCons: Wrong "
                                                       "`VarX->value()'");
                        if (VarY->contains(C))
                                VarY->removeSingle(C, this);
                }
        }
}

void ConstrMetaXeqYneqC::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == 0) {
                                // while ( VarY->min() < C )   {
                                //	if ( !VarY->removeSingle( VarY->min(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(NsMINUS_INF, C - 1, this);
                                // while ( VarY->max() > C )   {
                                //	if ( !VarY->removeSingle( VarY->max(),
                                //this ) ) 		return;	// ... to avoid
                                //an infinite loop
                                //}
                                VarY->removeRange(C + 1, NsPLUS_INF, this);
                        } else {
                                assert_exc(VarX->value() == 1,
                                           "ConstrMetaXeqYneqC::LocalArcCons: "
                                           "Wrong `VarX->value()'");
                                if (VarY->contains(C))
                                        VarY->removeSingle(C, this);
                        }
                }
        } else {
                assert_exc(
                    VarY == Qitem.getVarFired(),
                    "ConstrMetaXeqYneqC::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(0) && !VarY->contains(C))
                        VarX->removeSingle(0, this);
                if (VarX->contains(1) && (VarY->min() == C && VarY->max() == C))
                        VarX->removeSingle(1, this);
        }
}

void ConstrXeqYplusC::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (!VarY->contains(*v - C))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(*v + C))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqYplusC::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                SupportVal = Qitem.getW() - C;
                if (VarY->contains(SupportVal))
                        VarY->removeSingle(SupportVal, this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqYplusC::LocalArcCons: Wrong getVarFired");
                SupportVal = Qitem.getW() + C;
                if (VarX->contains(SupportVal))
                        VarX->removeSingle(SupportVal, this);
        }
}

void ConstrXeqCminusY::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (!VarY->contains(C - *v))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(C - *v))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqCminusY::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                SupportVal = C - Qitem.getW();
                if (VarY->contains(SupportVal))
                        VarY->removeSingle(SupportVal, this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqCminusY::LocalArcCons: Wrong getVarFired");
                SupportVal = C - Qitem.getW();
                if (VarX->contains(SupportVal))
                        VarX->removeSingle(SupportVal, this);
        }
}

void ConstrXeqYtimesC::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (*v % C != 0 || !VarY->contains(*v / C))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(*v * C))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqYtimesC::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                SupportVal = Qitem.getW() / C;
                if (Qitem.getW() % C == 0 && VarY->contains(SupportVal))
                        VarY->removeSingle(SupportVal, this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqCminusY::LocalArcCons: Wrong getVarFired");
                SupportVal = Qitem.getW() * C;
                if (VarX->contains(SupportVal))
                        VarX->removeSingle(SupportVal, this);
        }
}

void ConstrXeqY::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (!VarY->contains(*v))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(*v))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqY::LocalArcCons(QueueItem& Qitem)
{
        // NsInt  SupportVal = Qitem.getW();
        if (VarX == Qitem.getVarFired()) {
                if (VarY->contains(Qitem.getW()))
                        VarY->removeSingle(Qitem.getW(), this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqY::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(Qitem.getW()))
                        VarX->removeSingle(Qitem.getW(), this);
        }
}

void ConstrXeqYplusCZspecial::ArcCons(void)
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

void ConstrXeqYplusCZspecial::LocalArcCons(QueueItem& Qitem)
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
                        if (VarX->contains(SupportVal))
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
                assert_exc(
                    VarZ == Qitem.getVarFired(),
                    "ConstrXeqYplusCZspecial::LocalArcCons: Wrong getVarFired");
                // SupportVal = Qitem.getW() * C -1;
                // while ( ( SupportVal = VarX->next(SupportVal) )  <
                // (Qitem.getW() + 1) * C ) 	VarX->removeSingle( SupportVal,
                //this );
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

void ConstrXeqYdivC::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                if (VarY->next(*v * C - 1) >= (*v + 1) * C)
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(*v / C))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqYdivC::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                // SupportVal = Qitem.getW() * C -1;
                // while ( ( SupportVal = VarY->next(SupportVal) )  <
                // (Qitem.getW() + 1) * C ) 	VarY->removeSingle( SupportVal,
                //this );
                VarY->removeRange(Qitem.getW() * C, (Qitem.getW() + 1) * C - 1,
                                  this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqYdivC::LocalArcCons: Wrong getVarFired");
                SupportVal = Qitem.getW() / C;
                if (VarX->contains(SupportVal)) {
                        if (VarY->next(SupportVal * C - 1) >=
                            (SupportVal + 1) * C)
                                VarX->removeSingle(SupportVal, this);
                }
        }
}

void ConstrXeqCdivY::ArcCons(void)
{
        NsIntVar::const_iterator v;
        for (v = VarX->begin(); v != VarX->end(); ++v) {
                // there should be ( C/(v+1) , C/v ]  in VarY's domain
                // C = C/1
                if ((*v == 0 && VarY->next(C) == NsPLUS_INF)
                    // -C = C/-1
                    || (*v == -1 && NsMINUS_INF == VarY->previous(-C + 1)) ||
                    (*v != 0 && *v != -1 && VarY->next(C / (*v + 1)) > C / *v))
                        VarX->removeSingle(*v, this);
        }
        for (v = VarY->begin(); v != VarY->end(); ++v) {
                if (!VarX->contains(C / *v))
                        VarY->removeSingle(*v, this);
        }
}

void ConstrXeqCdivY::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupportVal;
        if (VarX == Qitem.getVarFired()) {
                if (Qitem.getW() == 0) {
                        // while ( VarY->max() > C )   {
                        //	if ( !VarY->removeSingle( VarY->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(C + 1, NsPLUS_INF, this);
                } else if (Qitem.getW() == -1) {
                        // while ( VarY->min() <= -C )   {
                        //	if ( !VarY->removeSingle( VarY->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarY->removeRange(NsMINUS_INF, -C, this);
                } else {
                        // SupportVal = C / (Qitem.getW()+1);
                        // while ( ( SupportVal = VarY->next(SupportVal) )  <=
                        // C / Qitem.getW() ) 	VarY->removeSingle( SupportVal,
                        //this );
                        VarY->removeRange(C / (Qitem.getW() + 1) + 1,
                                          C / Qitem.getW(), this);
                }
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqCdivY::LocalArcCons: Wrong getVarFired");
                if (Qitem.getW() == 0)
                        return;
                SupportVal = C / Qitem.getW();
                if (VarX->contains(SupportVal)) {
                        // -C = C/-1
                        if ((SupportVal == -1 &&
                             NsMINUS_INF == VarY->previous(-C + 1)) ||
                            (SupportVal != -1 &&
                             VarY->next(C / (SupportVal + 1)) > C / SupportVal))
                                VarX->removeSingle(SupportVal, this);
                }
        }
}

void naxos::YmodC_min_max(const NsIntVar* VarY, const NsInt C, NsInt& min,
                          NsInt& max)
{
        min = NsPLUS_INF;
        max = NsMINUS_INF;
        NsInt SupValY, v;
        assert_exc(C > 0, "YmodC_min_max: Special condition required:  C > 0");
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

void ConstrXeqYmodC::ArcCons(void)
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

void ConstrXeqYmodC::LocalArcCons(QueueItem& Qitem)
{
        NsInt SupValX, SupValY;
        if (VarX == Qitem.getVarFired()) {
                SupValX = Qitem.getW();
                for (SupValY = (VarY->min() / C) * C + SupValX;
                     SupValY <= VarY->max(); SupValY += C) {
                        if (VarY->contains(SupValY))
                                VarY->removeSingle(SupValY, this);
                }
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqYmodC::LocalArcCons: Wrong getVarFired");
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

void ConstrXneqY::ArcCons(void)
{
        if (VarY->isBound() && VarX->contains(VarY->value()))
                VarX->removeSingle(VarY->value(), this);
        if (VarX->isBound() && VarY->contains(VarX->value()))
                VarY->removeSingle(VarX->value(), this);
}

void ConstrXneqY::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound() && VarY->contains(VarX->value()))
                        VarY->removeSingle(VarX->value(), this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXneqY::LocalArcCons: Wrong getVarFired");
                if (VarY->isBound() && VarX->contains(VarY->value()))
                        VarX->removeSingle(VarY->value(), this);
        }
}

void ConstrXeqAbsY::ArcCons(void)
{
        VarX->removeRange(NsMINUS_INF,
                          max(labs(max(VarY->min(), static_cast<NsInt>(0))),
                              labs(min(static_cast<NsInt>(0), VarY->max()))) -
                              1,
                          this);
        VarX->removeRange(max(labs(VarY->min()), labs(VarY->max())) + 1,
                          NsPLUS_INF, this);
        VarY->removeRange(NsMINUS_INF, -(VarX->max() + 1), this);
        VarY->removeRange(+(VarX->max() + 1), NsPLUS_INF, this);
        if (VarX->min() != 0)
                VarY->removeRange(-(VarX->min() - 1), +(VarX->min() - 1), this);
}

void ConstrXeqAbsY::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                VarY->removeRange(NsMINUS_INF, -(VarX->max() + 1), this);
                VarY->removeRange(+(VarX->max() + 1), NsPLUS_INF, this);
                if (VarX->min() != 0)
                        VarY->removeRange(-(VarX->min() - 1),
                                          +(VarX->min() - 1), this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXeqAbsY::LocalArcCons: Wrong getVarFired");
                VarX->removeRange(
                    NsMINUS_INF,
                    max(labs(max(VarY->min(), static_cast<NsInt>(0))),
                        labs(min(static_cast<NsInt>(0), VarY->max()))) -
                        1,
                    this);
                VarX->removeRange(max(labs(VarY->min()), labs(VarY->max())) + 1,
                                  NsPLUS_INF, this);
        }
}

bool naxos::IntersectionEmpty(const NsIntVar* VarY, const NsIntVar* VarZ)
{
        // `valy' plays the role of the max{valy,valz} too
        NsInt valy = NsMINUS_INF, valz;
        for (;;) {
                valz = VarZ->next(valy);
                valy = VarY->next(valy);
                if (valy == NsPLUS_INF || valz == NsPLUS_INF)
                        break;
                if (valy == valz) {
                        return false;
                } else if (valy > valz) {
                        if (VarZ->contains(valy))
                                return false;
                } else {
                        if (VarY->contains(valz))
                                return false;
                        valy = valz;
                }
        }
        return true;
}

// namespace  {
//    	void
//    MetaXeqYeqneqZArcCons (NsIntVar *VarX, NsIntVar *VarY, NsIntVar *VarZ,
//    		const NsConstraint *constraint, const bool neg)
//    {
//    }
//
//
//    	void
//    MetaXeqYeqneqZLocalArcCons (QueueItem& Qitem, NsIntVar *VarX, NsIntVar
//    *VarY, NsIntVar *VarZ,
//    		const NsConstraint *constraint, const bool neg)
//    {
//    }
//}  // end namespace

void ConstrMetaXeqYeqZ::ArcCons(void)
{
        // MetaXeqYeqneqZArcCons(VarX, VarY, VarZ, this, false);
        MakeBoolean(VarX, this);
        if (VarX->contains(neg) && VarY->max() == VarZ->min() &&
            VarY->min() == VarZ->max())
                VarX->removeSingle(neg, this);
        if (VarX->contains(!neg) && IntersectionEmpty(VarY, VarZ))
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        if (VarZ->isBound() && VarY->contains(VarZ->value()))
                                VarY->removeSingle(VarZ->value(), this);
                        if (VarY->isBound() && VarZ->contains(VarY->value()))
                                VarZ->removeSingle(VarY->value(), this);
                } else {
                        assert_exc(
                            VarX->value() == !neg,
                            "MetaXeqYeqneqZArcCons: Wrong `VarX->value()'");
                        NsIntVar::const_iterator v;
                        for (v = VarY->begin(); v != VarY->end(); ++v) {
                                if (!VarZ->contains(*v))
                                        VarY->removeSingle(*v, this);
                        }
                        for (v = VarZ->begin(); v != VarZ->end(); ++v) {
                                if (!VarY->contains(*v))
                                        VarZ->removeSingle(*v, this);
                        }
                }
        }
}

void ConstrMetaXeqYeqZ::LocalArcCons(QueueItem& Qitem)
{
        // MetaXeqYeqneqZLocalArcCons(Qitem, VarX, VarY, VarZ, this, false);
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                if (VarZ->isBound() &&
                                    VarY->contains(VarZ->value()))
                                        VarY->removeSingle(VarZ->value(), this);
                                if (VarY->isBound() &&
                                    VarZ->contains(VarY->value()))
                                        VarZ->removeSingle(VarY->value(), this);
                        } else {
                                assert_exc(VarX->value() == !neg,
                                           "ConstrMetaXeqYeqZ::LocalArcCons: "
                                           "Wrong `VarX->value()'");
                                NsIntVar::const_iterator v;
                                for (v = VarY->begin(); v != VarY->end(); ++v) {
                                        if (!VarZ->contains(*v))
                                                VarY->removeSingle(*v, this);
                                }
                                for (v = VarZ->begin(); v != VarZ->end(); ++v) {
                                        if (!VarY->contains(*v))
                                                VarZ->removeSingle(*v, this);
                                }
                        }
                }
        } else {
                if (VarY == Qitem.getVarFired()) {
                        if (VarX->isBound()) {
                                if (VarX->value() == neg) {
                                        if (VarY->isBound() &&
                                            VarZ->contains(VarY->value()))
                                                VarZ->removeSingle(
                                                    VarY->value(), this);
                                } else {
                                        assert_exc(VarX->value() == !neg,
                                                   "ConstrMetaXeqYeqZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        if (VarZ->contains(Qitem.getW()))
                                                VarZ->removeSingle(Qitem.getW(),
                                                                   this);
                                }
                        }
                } else {
                        assert_exc(VarZ == Qitem.getVarFired(),
                                   "ConstrMetaXeqYeqZ::LocalArcCons: Wrong "
                                   "getVarFired");
                        if (VarX->isBound()) {
                                if (VarX->value() == neg) {
                                        if (VarZ->isBound() &&
                                            VarY->contains(VarZ->value()))
                                                VarY->removeSingle(
                                                    VarZ->value(), this);
                                } else {
                                        assert_exc(VarX->value() == !neg,
                                                   "ConstrMetaXeqYeqZ::"
                                                   "LocalArcCons: Wrong "
                                                   "`VarX->value()'");
                                        if (VarY->contains(Qitem.getW()))
                                                VarY->removeSingle(Qitem.getW(),
                                                                   this);
                                }
                        }
                }
                if (VarX->contains(neg) && VarY->max() == VarZ->min() &&
                    VarY->min() == VarZ->max())
                        VarX->removeSingle(neg, this);
                if (VarX->contains(!neg) && IntersectionEmpty(VarY, VarZ))
                        VarX->removeSingle(!neg, this);
        }
}

//	void
// ConstrMetaXeqYneqZ::ArcCons (void)  const
//{
//	MetaXeqYeqneqZArcCons(VarX, VarY, VarZ, this, true);
//}
//
//
//	void
// ConstrMetaXeqYneqZ::LocalArcCons (QueueItem& Qitem)  const
//{
//	MetaXeqYeqneqZLocalArcCons(Qitem, VarX, VarY, VarZ, this, true);
//}

void ConstrXeqYandZ::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(neg) && (VarY->min() * VarZ->min()))
                VarX->removeSingle(neg, this);
        if (VarX->contains(!neg) && !(VarY->max() * VarZ->max()))
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        // VOID
                } else {
                        assert_exc(VarX->value() == !neg, "ConstrXeqYandZ::"
                                                          "LocalArcCons: Wrong "
                                                          "`VarX->value()'");
                        if (VarY->contains(0))
                                VarY->removeSingle(0, this);
                        if (VarZ->contains(0))
                                VarZ->removeSingle(0, this);
                }
        }
}

void ConstrXeqYandZ::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                // VOID
                        } else {
                                assert_exc(VarX->value() == !neg,
                                           "ConstrXeqYandZ::LocalArcCons: "
                                           "Wrong `VarX->value()'");
                                if (VarY->contains(0))
                                        VarY->removeSingle(0, this);
                                if (VarZ->contains(0))
                                        VarZ->removeSingle(0, this);
                        }
                }
        } else {
                // assert_exc(VarY == Qitem.getVarFired(),
                // "ConstrXeqYandZ::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(neg) && (VarY->min() * VarZ->min()))
                        VarX->removeSingle(neg, this);
                if (VarX->contains(!neg) && !(VarY->max() * VarZ->max()))
                        VarX->removeSingle(!neg, this);
        }
}

void ConstrXeqYorZ::ArcCons(void)
{
        MakeBoolean(VarX, this);
        if (VarX->contains(neg) && (VarY->min() + VarZ->min() != 0))
                VarX->removeSingle(neg, this);
        if (VarX->contains(!neg) && (VarY->max() + VarZ->max() == 0))
                VarX->removeSingle(!neg, this);
        if (VarX->isBound()) {
                if (VarX->value() == neg) {
                        if (VarY->contains(1))
                                VarY->removeSingle(1, this);
                        if (VarZ->contains(1))
                                VarZ->removeSingle(1, this);
                } else {
                        assert_exc(VarX->value() == !neg, "ConstrXeqYorZ::"
                                                          "LocalArcCons: Wrong "
                                                          "`VarX->value()'");
                        // VOID
                }
        }
}

void ConstrXeqYorZ::LocalArcCons(QueueItem& Qitem)
{
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound()) {
                        if (VarX->value() == neg) {
                                if (VarY->contains(1))
                                        VarY->removeSingle(1, this);
                                if (VarZ->contains(1))
                                        VarZ->removeSingle(1, this);
                        } else {
                                assert_exc(VarX->value() == !neg,
                                           "ConstrXeqYorZ::LocalArcCons: Wrong "
                                           "`VarX->value()'");
                                // VOID
                        }
                }
        } else {
                // assert_exc(VarY == Qitem.getVarFired(),
                // "ConstrXeqYorZ::LocalArcCons: Wrong getVarFired");
                if (VarX->contains(neg) && (VarY->min() + VarZ->min() != 0))
                        VarX->removeSingle(neg, this);
                if (VarX->contains(!neg) && (VarY->max() + VarZ->max() == 0))
                        VarX->removeSingle(!neg, this);
        }
}

void ConstrXorY::ArcCons(void)
{
        if (VarY->isBound() && VarY->value() == neg && VarX->contains(neg))
                VarX->removeSingle(neg, this);
        if (VarX->isBound() && VarX->value() == neg && VarY->contains(neg))
                VarY->removeSingle(neg, this);
}

void ConstrXorY::LocalArcCons(QueueItem& Qitem)
{
        // NsInt  SupportVal = Qitem.getW();
        if (VarX == Qitem.getVarFired()) {
                if (VarX->isBound() && VarX->value() == neg &&
                    VarY->contains(neg))
                        VarY->removeSingle(neg, this);
        } else {
                assert_exc(VarY == Qitem.getVarFired(),
                           "ConstrXorY::LocalArcCons: Wrong getVarFired");
                if (VarY->isBound() && VarY->value() == neg &&
                    VarX->contains(neg))
                        VarX->removeSingle(neg, this);
        }
}

void naxos::array_min_minmax(const NsIntVarArray* VarArr, NsInt& min,
                             NsInt& minmax)
{
        NsIntVarArray::const_iterator V = VarArr->begin();
        min = V->min();
        minmax = V->max();
        ++V;
        for (; V != VarArr->end(); ++V) {
                if (V->min() < min)
                        min = V->min();
                if (V->max() < minmax)
                        minmax = V->max();
        }
}

void naxos::array_maxmin_max(const NsIntVarArray* VarArr, NsInt& maxmin,
                             NsInt& max)
{
        NsIntVarArray::const_iterator V = VarArr->begin();
        maxmin = V->min();
        max = V->max();
        ++V;
        for (; V != VarArr->end(); ++V) {
                if (V->min() > maxmin)
                        maxmin = V->min();
                if (V->max() > max)
                        max = V->max();
        }
}

void naxos::array_sum_min_max(const NsIntVarArray* VarArr, const NsIndex start,
                              const NsIndex length, NsInt& summin,
                              NsInt& summax)
{
        summin = summax = 0;
        for (NsIndex i = start; i < start + length; ++i) {
                const NsIntVar& V = (*VarArr)[i];
                summin += V.min();
                summax += V.max();
        }
        // for (NsIntVarArray::iterator V = VarArr->begin();   V !=
        // VarArr->end();   ++V)   { 	summin += V->min(); 	summax +=
        //V->max();
        //}
}

ConstrXeqMin::ConstrXeqMin(NsIntVar* X, NsIntVarArray* VarArr_init)
  : VarX(X), VarArr(VarArr_init)
{
        assert_exc(VarArr->size() >= 1, "ConstrXeqMin::ConstrXeqMin: Condition "
                                        "required:  VarArr.size() >= 1");
        NsProblemManager& pm = VarX->manager();
        for (NsIntVarArray::iterator V = VarArr->begin(); V != VarArr->end();
             ++V) {
                assert_exc(&pm == &V->manager(), "ConstrXeqMin::ConstrXeqMin: "
                                                 "All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
}

ConstrXeqMax::ConstrXeqMax(NsIntVar* X, NsIntVarArray* VarArr_init)
  : VarX(X), VarArr(VarArr_init)
{
        assert_exc(VarArr->size() >= 1, "ConstrXeqMax::ConstrXeqMax: Condition "
                                        "required:  VarArr.size() >= 1");
        NsProblemManager& pm = VarX->manager();
        for (NsIntVarArray::iterator V = VarArr->begin(); V != VarArr->end();
             ++V) {
                assert_exc(&pm == &V->manager(), "ConstrXeqMax::ConstrXeqMax: "
                                                 "All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
}

void ConstrXeqMin::ArcCons(void)
{
        NsInt min, minmax;
        array_min_minmax(VarArr, min, minmax);
        // while ( VarX->min() < min )   {
        //	if ( !VarX->removeSingle( VarX->min(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(NsMINUS_INF, min - 1, this);
        // while ( VarX->max() > minmax )   {
        //	if ( !VarX->removeSingle( VarX->max(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(minmax + 1, NsPLUS_INF, this);
        NsIntVarArray::iterator V;
        NsIntVar::const_iterator v = VarX->begin();
        ++v; // the first value is OK
        for (; v != VarX->end(); ++v) {
                // for (NsInt  val = VarX->min() + 1;   val < VarX->max();
                // ++val)   {  if ( VarX->contains( val ) )  {
                for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                        if (V->contains(*v))
                                break;
                }
                if (V == VarArr->end())
                        VarX->removeSingle(*v, this);
                //} else  {
                //	for (V = VarArr->begin();   V != VarArr->end();   ++V) {
                //		if (V->contains( val ))
                //			V->removeSingle( val, this );
                //	}
                //}
        }
}

void ConstrXeqMax::ArcCons(void)
{
        NsInt maxmin, max;
        array_maxmin_max(VarArr, maxmin, max);
        // while ( VarX->min() < maxmin )   {
        //	if ( !VarX->removeSingle( VarX->min(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(NsMINUS_INF, maxmin - 1, this);
        // while ( VarX->max() > max )   {
        //	if ( !VarX->removeSingle( VarX->max(), this ) )
        //		return;	// ... to avoid an infinite loop
        //}
        VarX->removeRange(max + 1, NsPLUS_INF, this);
        NsIntVarArray::iterator V;
        NsIntVar::const_iterator v = VarX->begin();
        ++v; // the first value is OK
        for (; v != VarX->end(); ++v) {
                // for (NsInt  val = VarX->min() + 1;   val < VarX->max();
                // ++val)   {  if ( VarX->contains( val ) )  {
                for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                        if (V->contains(*v))
                                break;
                }
                if (V == VarArr->end())
                        VarX->removeSingle(*v, this);
                //} else  {
                //	for (V = VarArr->begin();   V != VarArr->end();   ++V) {
                //		if (V->contains( val ))
                //			V->removeSingle( val, this );
                //	}
                //}
        }
}

// namespace  {
//		void
//	MinMaxLocalArcCons (QueueItem& Qitem, NsIntVar *VarX, NsIntVarArray
//*VarArr, const NsConstraint *constraint)
//	{
//		NsIntVarArray::iterator  V;
//
//		if (VarX == Qitem.getVarFired())  {
//
//			//for (V = VarArr->begin();   V != VarArr->end();   ++V)
//{
//			//	if (V->contains( Qitem.getW() ))
//			//		V->removeSingle( Qitem.getW(), constraint
//);
//			//}
//
//		} else  {
//			if ( VarX->contains(Qitem.getW()) )   {
//				for (V = VarArr->begin();   V != VarArr->end();   ++V)
//{ 					if ( V->contains( Qitem.getW() ) )
//						break;
//				}
//				if ( V == VarArr->end() )
//					VarX->removeSingle( Qitem.getW(), constraint
//);
//			}
//		}
//	}
//}  // end namespace

void ConstrXeqMin::LocalArcCons(QueueItem& Qitem)
{
        // MinMaxLocalArcCons(Qitem, VarX, VarArr, this);
        NsIntVarArray::iterator V;
        if (VarX == Qitem.getVarFired()) {
                if (Qitem.getW() < VarX->min()) {
                        for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                                // while ( V->min() < VarX->min() )   {
                                //	if ( !V->removeSingle( V->min(), this )
                                //) 		return;	// ... to avoid an
                                //infinite loop
                                //}
                                V->removeRange(NsMINUS_INF, VarX->min() - 1,
                                               this);
                        }
                }
        } else {
                if (Qitem.getW() > Qitem.getVarFired()->max()) {
                        // while ( VarX->max() > Qitem.getVarFired()->max() ) {
                        //	if ( !VarX->removeSingle( VarX->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarX->removeRange(Qitem.getVarFired()->max() + 1,
                                          NsPLUS_INF, this);
                }
                if (VarX->contains(Qitem.getW())) {
                        for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                                if (V->contains(Qitem.getW()))
                                        break;
                        }
                        if (V == VarArr->end())
                                VarX->removeSingle(Qitem.getW(), this);
                }
        }
}

void ConstrXeqMax::LocalArcCons(QueueItem& Qitem)
{
        // MinMaxLocalArcCons(Qitem, VarX, VarArr, this);
        NsIntVarArray::iterator V;
        if (VarX == Qitem.getVarFired()) {
                if (Qitem.getW() > VarX->max()) {
                        for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                                // while ( V->max() > VarX->max() )   {
                                //	if ( !V->removeSingle( V->max(), this )
                                //) 		return;	// ... to avoid an
                                //infinite loop
                                //}
                                V->removeRange(VarX->max() + 1, NsPLUS_INF,
                                               this);
                        }
                }
        } else {
                if (Qitem.getW() < Qitem.getVarFired()->min()) {
                        // while ( VarX->min() < Qitem.getVarFired()->min() ) {
                        //	if ( !VarX->removeSingle( VarX->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        VarX->removeRange(NsMINUS_INF,
                                          Qitem.getVarFired()->min() - 1, this);
                }
                if (VarX->contains(Qitem.getW())) {
                        for (V = VarArr->begin(); V != VarArr->end(); ++V) {
                                if (V->contains(Qitem.getW()))
                                        break;
                        }
                        if (V == VarArr->end())
                                VarX->removeSingle(Qitem.getW(), this);
                }
        }
}

ConstrXeqSum::ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init)
  : VarX(X), VarArr(VarArr_init), start(0), length(VarArr_init->size())
{
        assert_exc(VarArr->size() >= 1, "ConstrXeqSum::ConstrXeqSum: Condition "
                                        "required:  VarArr.size() >= 1");
        NsProblemManager& pm = VarX->manager();
        for (NsIntVarArray::iterator V = VarArr->begin(); V != VarArr->end();
             ++V) {
                assert_exc(&pm == &V->manager(), "ConstrXeqSum::ConstrXeqSum: "
                                                 "All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
}

ConstrXeqSum::ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init,
                           const NsIndex start_init, const NsIndex length_init)
  : VarX(X), VarArr(VarArr_init), start(start_init), length(length_init)
{
        assert_exc(VarArr->size() >= 1, "ConstrXeqSum::ConstrXeqSum: Condition "
                                        "required:  VarArr.size() >= 1");
        NsProblemManager& pm = VarX->manager();
        for (NsIndex i = start; i < start + length; ++i) {
                NsIntVar& V = (*VarArr)[i];
                assert_exc(&pm == &V.manager(), "ConstrXeqSum::ConstrXeqSum: "
                                                "All the variables of a "
                                                "constraint must belong to the "
                                                "same NsProblemManager");
        }
}

// bounds-consistency only
void ConstrXeqSum::ArcCons(void)
{
        NsInt summin, summax;
        array_sum_min_max(VarArr, start, length, summin, summax);
        NsIndex i;
        bool changed_summinmax = true;
        for (;;) {
                do {
                        // while ( VarX->min() < summin )   {
                        //	if ( !VarX->removeSingle( VarX->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        if (!VarX->removeRange(NsMINUS_INF, summin - 1, this))
                                return;
                        for (i = start; i < start + length; ++i) {
                                NsIntVar& V = (*VarArr)[i];
                                if (V.min() + summax - V.max() < VarX->min()) {
                                        summin -= V.min();
                                        // if ( !V.removeSingle( V.min(), this )
                                        // ) 	return;	// ... to avoid an
                                        //infinite loop
                                        if (!V.removeRange(NsMINUS_INF,
                                                           -summax + V.max() +
                                                               VarX->min() - 1,
                                                           this))
                                                return;
                                        summin += V.min();
                                        changed_summinmax = true;
                                }
                        }
                } while (VarX->min() < summin);
                //  Initially `changed_summinmax' was intentionally set true, in
                //  order the
                //   following `if' statement to be ignored, the first time it
                //   is executed.
                if (!changed_summinmax)
                        break;
                changed_summinmax = false;
                do {
                        // while ( VarX->max() > summax )   {
                        //	if ( !VarX->removeSingle( VarX->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        if (!VarX->removeRange(summax + 1, NsPLUS_INF, this))
                                return;
                        for (i = start; i < start + length; ++i) {
                                NsIntVar& V = (*VarArr)[i];
                                if (V.max() + summin - V.min() > VarX->max()) {
                                        summax -= V.max();
                                        // if ( !V.removeSingle( V.max(), this )
                                        // ) 	return;	// ... to avoid an
                                        //infinite loop
                                        if (!V.removeRange(-summin + V.min() +
                                                               VarX->max() + 1,
                                                           NsPLUS_INF, this))
                                                return;
                                        summax += V.max();
                                        changed_summinmax = true;
                                }
                        }
                } while (VarX->max() > summax);
                if (!changed_summinmax)
                        break;
                changed_summinmax = false;
        }
}

// bounds-consistency only
void ConstrXeqSum::LocalArcCons(QueueItem& Qitem)
{
        if (Qitem.getVarFired()->min() < Qitem.getW() &&
            Qitem.getW() < Qitem.getVarFired()->max())
                return; // bounds-consistency does not care
        ArcCons();
        // throw  NsException("ConstrXeqSum::LocalArcCons: unimplemented");
}

// bounds-consistency only
void ConstrXeqYplusZ::ArcCons(void)
{
        bool changed_summinmax = true;
        for (;;) {
                do {
                        // while ( VarX->min() < VarY->min() + VarZ->min() )   {
                        //	if ( !VarX->removeSingle( VarX->min(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        if (!VarX->removeRange(NsMINUS_INF,
                                               VarY->min() + VarZ->min() - 1,
                                               this))
                                return;
                        if (VarY->min() + VarZ->max() < VarX->min()) {
                                // if ( !VarY->removeSingle( VarY->min(), this )
                                // ) 	return;	// ... to avoid an infinite loop
                                if (!VarY->removeRange(
                                        NsMINUS_INF,
                                        -VarZ->max() + VarX->min() - 1, this))
                                        return;
                                changed_summinmax = true;
                        }
                        if (VarZ->min() + VarY->max() < VarX->min()) {
                                // if ( !VarZ->removeSingle( VarZ->min(), this )
                                // ) 	return;	// ... to avoid an infinite loop
                                if (!VarZ->removeRange(
                                        NsMINUS_INF,
                                        -VarY->max() + VarX->min() - 1, this))
                                        return;
                                changed_summinmax = true;
                        }
                } while (VarX->min() < VarY->min() + VarZ->min());
                //  Initially `changed_summinmax' was intentionally set true, in
                //  order the
                //   following `if' statement to be ignored, the first time it
                //   is executed.
                if (!changed_summinmax)
                        break;
                changed_summinmax = false;
                do {
                        // while ( VarX->max() > VarY->max() + VarZ->max() )   {
                        //	if ( !VarX->removeSingle( VarX->max(), this ) )
                        //		return;	// ... to avoid an infinite loop
                        //}
                        if (!VarX->removeRange(VarY->max() + VarZ->max() + 1,
                                               NsPLUS_INF, this))
                                return;
                        if (VarY->max() + VarZ->min() > VarX->max()) {
                                // if ( !VarY->removeSingle( VarY->max(), this )
                                // ) 	return;	// ... to avoid an infinite loop
                                if (!VarY->removeRange(-VarZ->min() +
                                                           VarX->max() + 1,
                                                       NsPLUS_INF, this))
                                        return;
                                changed_summinmax = true;
                        }
                        if (VarZ->max() + VarY->min() > VarX->max()) {
                                // if ( !VarZ->removeSingle( VarZ->max(), this )
                                // ) 	return;	// ... to avoid an infinite loop
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

// bounds-consistency only
void ConstrXeqYplusZ::LocalArcCons(QueueItem& Qitem)
{
        if (Qitem.getVarFired()->min() < Qitem.getW() &&
            Qitem.getW() < Qitem.getVarFired()->max())
                return; // bounds-consistency does not care
        ArcCons();
}

void naxos::product_min_max(const NsIntVar* VarY, const NsIntVar* VarZ,
                            NsInt& min, NsInt& max)
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
                         bool& changed_minmax, const NsConstraint* constraint)
{
        while ((VarY->min() * VarZ->min() < VarX->min() &&
                VarY->min() * VarZ->max() < VarX->min()) ||
               (VarY->min() * VarZ->min() > VarX->max() &&
                VarY->min() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->min(), constraint))
                        return; // ... to avoid an infinite loop
                changed_minmax = true;
        }
        while ((VarY->max() * VarZ->min() < VarX->min() &&
                VarY->max() * VarZ->max() < VarX->min()) ||
               (VarY->max() * VarZ->min() > VarX->max() &&
                VarY->max() * VarZ->max() > VarX->max())) {
                if (!VarY->removeSingle(VarY->max(), constraint))
                        return; // ... to avoid an infinite loop
                changed_minmax = true;
        }
}
} // end namespace

// bounds-consistency only
void ConstrXeqYtimesZ::ArcCons(void)
{
        NsInt min, max;
        bool changed_minmax;
        do {
                changed_minmax = false;
                product_min_max(VarY, VarZ, min, max);
                // while ( VarX->min() < min )   {
                //	if ( !VarX->removeSingle( VarX->min(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarX->removeRange(NsMINUS_INF, min - 1, this);
                // while ( VarX->max() > max )   {
                //	if ( !VarX->removeSingle( VarX->max(), this ) )
                //		return;	// ... to avoid an infinite loop
                //}
                VarX->removeRange(max + 1, NsPLUS_INF, this);
                product_prune_bound(VarX, VarY, VarZ, changed_minmax, this);
                product_prune_bound(VarX, VarZ, VarY, changed_minmax, this);
        } while (changed_minmax);
}

// bounds-consistency only
void ConstrXeqYtimesZ::LocalArcCons(QueueItem& Qitem)
{
        if (Qitem.getVarFired()->min() < Qitem.getW() &&
            Qitem.getW() < Qitem.getVarFired()->max())
                return; // bounds-consistency does not care
        ArcCons();
}

//#ifndef Ns_AllDiff_Test

ConstrAllDiff::ConstrAllDiff(NsIntVarArray* VarArr_init) : VarArr(VarArr_init)
{
        assert_exc(VarArr->size() >= 2, "ConstrAllDiff::ConstrAllDiff: "
                                        "Condition required:  VarArr.size() >= "
                                        "2");
        NsIntVarArray::iterator X = VarArr->begin();
        NsProblemManager& pm = X->manager();
        ++X;
        for (; X != VarArr->end(); ++X) {
                assert_exc(&pm == &X->manager(), "ConstrAllDiff::ConstrAllDiff:"
                                                 " All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
}

void ConstrAllDiff::ArcCons(void)
{
        NsIntVarArray::iterator X, Y;
        // Dovecote Theorem (to spot a dead-end earlier) //
        NsInt min_of_arr, max_of_arr;
        X = VarArr->begin();
        min_of_arr = X->min();
        max_of_arr = X->max();
        ++X;
        for (; X != VarArr->end(); ++X) {
                if (X->min() < min_of_arr)
                        min_of_arr = X->min();
                if (X->max() > max_of_arr)
                        max_of_arr = X->max();
        }
        if (static_cast<NsUInt>(max_of_arr - min_of_arr) + 1 < VarArr->size()) {
                // all values are inconsistent
                VarArr->begin()->removeAll();
                return;
        }
        // Classic ArcCons //
        for (X = VarArr->begin(); X != VarArr->end(); ++X) {
                for (Y = VarArr->begin(); Y != VarArr->end(); ++Y) {
                        if (Y != X && Y->isBound() && X->contains(Y->value()))
                                X->removeSingle(Y->value(), 0);
                }
        }
}

void ConstrAllDiff::LocalArcCons(QueueItem& Qitem)
{
        if (Qitem.getVarFired()->isBound()) {
                NsInt Value = Qitem.getVarFired()->value();
                for (NsIntVarArray::iterator X = VarArr->begin();
                     X != VarArr->end(); ++X) {
                        if (&*X != Qitem.getVarFired() && X->contains(Value))
                                X->removeSingle(Value, 0);
                }
        }
}

//#endif  // ! Ns_AllDiff_Test

bool ConstrAllDiffStrong::groupedNsIntVar::removeDomain(
    const IntDomain& V, const NsConstraint* constraint)
{
        if (Var.max() <= V.max()) {
                //  In this case, and when we have bounds-consistency mode (i.e.
                //  a
                //   constrained variable is represented by its minimum and
                //   maximum), we "eat" the values from "right" to "left," in
                //   order not to avoid (if possible) removing a value from the
                //   middle of the domain.
                for (NsIntVar::const_reverse_iterator val = V.rbegin();
                     val != V.rend(); ++val) {
                        if (Var.contains(*val)) {
                                if (!Var.removeSingle(*val, constraint))
                                        return false;
                        }
                }
        } else {
                for (NsIntVar::const_iterator val = V.begin(); val != V.end();
                     ++val) {
                        if (Var.contains(*val)) {
                                if (!Var.removeSingle(*val, constraint))
                                        return false;
                        }
                }
        }
        return true;
}

// struct less_max_mmin_size : public binary_function<NsIntVar, NsIntVar, bool>
// {
//
//	bool  operator ()  (const MaxLopez& X, const MaxLopez& Y)
//	{
//		return  ( X.max() < Y.max()
//				|| ( X.max() == Y.max()  &&  X.min() >  Y.min()
//)
//				|| ( X.max() == Y.max()  &&  X.min() == Y.min()  &&
//X.size() < Y.size() ) );
//	}
//
//};
//
//
// struct less_min           : public binary_function<NsIntVar, NsIntVar, bool>
// {
//
//	bool  operator ()  (const MaxLopez& X, const MaxLopez& Y)
//	{
//		return  ( X.min() >  Y.min() );
//	}
//
//};

//#ifdef Ns_AllDiff_Test

ConstrAllDiffStrong::ConstrAllDiffStrong(NsIntVarArray* VarArr_init,
                                         unsigned long Cap)
  : Capacity(Cap)
{
        assert_exc(VarArr_init->size() > Capacity,
                   "ConstrAllDiffStrong::ConstrAllDiffStrong: Condition "
                   "required:  VarArr.size() >= 2");
        assert_exc(Capacity > 0, "ConstrAllDiffStrong::ConstrAllDiffStrong: "
                                 "Condition required:  Capacity > 0");
        NsIntVarArray::iterator V = VarArr_init->begin();
        NsProblemManager& pm = V->manager();
        for (; V != VarArr_init->end(); ++V) {
                assert_exc(&pm == &V->manager(),
                           "ConstrAllDiffStrong::ConstrAllDiffStrong: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
                VarArr.push_back(groupedNsIntVar(*V));
                VarPointerGroup.insert(
                    make_pair((pointer_t) & *V, &VarArr.back()));
        }
        // for (X = VarArr->begin();   X != VarArr->end();   ++X)
        //	VarArrGroup.push_back( NsIntVar(pm, NsMINUS_INF+1, NsPLUS_INF-1)
        //);
}

namespace {

int less_function_MaxMMinSize(const void* X, const void* Y)
{
        NsIntVar& VarX =
            (*static_cast<ConstrAllDiffStrong::groupedNsIntVar* const*>(X))
                ->Var;
        NsIntVar& VarY =
            (*static_cast<ConstrAllDiffStrong::groupedNsIntVar* const*>(Y))
                ->Var;
        if (VarX.max() < VarY.max() ||
            (VarX.max() == VarY.max() &&
             (-VarX.min() < -VarY.min() ||
              (VarX.min() == VarY.min() && VarX.size() < VarY.size())))) {
                return -1;
        }
        if (VarX.max() == VarY.max() && VarX.min() == VarY.min() &&
            VarX.size() == VarY.size())
                return 0;
        return +1;
}

void AllDiffBoundsConsistency(
    NsDeque<ConstrAllDiffStrong::groupedNsIntVar>& VarArr,
    const unsigned long Capacity,
    const ConstrAllDiffStrong::groupedNsIntVar::group_t group,
    const NsConstraint* constraint)
{
        //	cout << &VarArr << ": ";
        //	for (NsIndex k=0;   k < VarArr.size();   ++k)
        //		cout << "  " << (VarArr[k].group() -
        //groupedNsIntVar::FIRST_GROUP) << ">" << VarArr[k].Var; 	cout <<
        //endl;  for (groupedNsIntVar::group_t group =
        // groupedNsIntVar::FIRST_GROUP;   /* VOID */ ;   ++group)   {
        //  At first, we gather all the variables with group-id
        //   equal to `group' into the array `VarArrSortedList'.
        NsDeque<ConstrAllDiffStrong::groupedNsIntVar*> VarArrSortedList;
        NsIndex i;
        ConstrAllDiffStrong::groupedNsIntVar::group_t nGroups =
            ConstrAllDiffStrong::groupedNsIntVar::FIRST_GROUP;
        for (i = 0; i < VarArr.size(); ++i) {
                if (VarArr[i].group() == group)
                        VarArrSortedList.push_back(&VarArr[i]);
                if (VarArr[i].group() > nGroups)
                        nGroups = VarArr[i].group();
        }
        assert_exc(VarArrSortedList.size() != 0,
                   "AllDiffBoundsConsistency: Empty `group' of variables");
        // break;	// We have processed all the separate groups of
        // variables.
        //  Copying `VarArrSortedList' to `VarArrSorted' which is friendlier
        //   to the `qsort()' function that it is used bellow.
        ConstrAllDiffStrong::groupedNsIntVar** VarArrSorted =
            new ConstrAllDiffStrong::groupedNsIntVar*[VarArrSortedList.size()];
        for (i = 0; i < VarArrSortedList.size(); ++i)
                VarArrSorted[i] = VarArrSortedList[i];
        //  ... Then we sort the variables in `VarArrSorted', by ascending
        //  maximum,
        //   descending minimum, and ascending size.  I.e., if the domain of
        //   VarX is more possible to be a subset of domain of VarY, then VarX
        //   precedes VarY in the array `VarArrSorted'.
        qsort(VarArrSorted, VarArrSortedList.size(),
              sizeof(ConstrAllDiffStrong::groupedNsIntVar*),
              less_function_MaxMMinSize);
        //  We gather all the different maxima into the array `Max' by ascending
        //  order.
        NsDeque<NsInt> Max;
        for (i = 0; i < VarArrSortedList.size(); ++i) {
                if (i == 0 || (i > 0 && VarArrSorted[i]->Var.max() !=
                                            VarArrSorted[i - 1]->Var.max()))
                        Max.push_back(VarArrSorted[i]->Var.max());
        }
        NsDeque<ConstrAllDiffStrong::groupedNsIntVar*> VarLtMax;
        NsDeque<NsInt> VarLtMaxMins;
        i = 0;
        for (NsIndex max_i = 0; max_i < Max.size(); ++max_i) {
                //  `VarEqMax' consists of all the variables of the
                //   array `VarArr' with `VarArr[i].max() == Max[max_i]'.
                NsDeque<ConstrAllDiffStrong::groupedNsIntVar*> VarEqMax;
                for (; i < VarArrSortedList.size() &&
                       VarArrSorted[i]->Var.max() == Max[max_i];
                     ++i)
                        VarEqMax.push_back(VarArrSorted[i]);
                //  `VarLeMax' is constructed by merging its previous contents,
                //   represented by `VarLtMax' (initially empty), and
                //   `VarEqMax'. `VarLeMax' contains the constrained variables
                //   of `VarArr' with `VarArr[i].max() <= Max[max_i]'.
                NsDeque<ConstrAllDiffStrong::groupedNsIntVar*> VarLeMax;
                NsIndex i1, i2;
                for (i1 = i2 = 0;
                     i1 < VarLtMax.size() && i2 < VarEqMax.size();) {
                        if (VarLtMax[i1]->Var.min() >=
                            VarEqMax[i2]->Var.min()) {
                                VarLeMax.push_back(VarLtMax[i1]);
                                ++i1;
                        } else {
                                VarLeMax.push_back(VarEqMax[i2]);
                                ++i2;
                        }
                }
                //  One of the two arrays (`VarLtMax' and `VarEqMax') is
                //   exhausted, so we append the contents of the other
                //   array to `VarLeMax', and the merging is completed.
                for (; i1 < VarLtMax.size(); ++i1)
                        VarLeMax.push_back(VarLtMax[i1]);
                for (; i2 < VarEqMax.size(); ++i2)
                        VarLeMax.push_back(VarEqMax[i2]);
                VarLtMax =
                    VarLeMax; // `VarLtMax' will be used at the next iteration.
                //  `VarLeMaxMins' contains the minima of the constrained
                //  variables
                //   included in `VarLeMax', in descending order (without
                //   duplicates). It is constructed by merging the array
                //   `VarLtMaxMins' (i.e., the previous contents of
                //   `VarLeMaxMins', that have no duplicates, by definition) and
                //   `VarEqMax'.mins (i.e., the minima of the constrained
                //   variables in `VarEqMax' that may contain duplicates, so we
                //   should avoid them).
                NsDeque<NsInt> VarLeMaxMins;
                for (i1 = i2 = 0;
                     i1 < VarLtMaxMins.size() && i2 < VarEqMax.size();) {
                        if (VarLtMaxMins[i1] > VarEqMax[i2]->Var.min()) {
                                VarLeMaxMins.push_back(VarLtMaxMins[i1]);
                                ++i1;
                        } else if (VarLtMaxMins[i1] < VarEqMax[i2]->Var.min()) {
                                VarLeMaxMins.push_back(VarEqMax[i2]->Var.min());
                                do {
                                        ++i2; // avoiding duplicates
                                } while (i2 < VarEqMax.size() &&
                                         VarEqMax[i2]->Var.min() ==
                                             VarEqMax[i2 - 1]->Var.min());
                        } // The two minima are equal.
                        else {
                                VarLeMaxMins.push_back(VarLtMaxMins[i1]);
                                ++i1;
                                do {
                                        ++i2; // avoiding duplicates
                                } while (i2 < VarEqMax.size() &&
                                         VarEqMax[i2]->Var.min() ==
                                             VarEqMax[i2 - 1]->Var.min());
                        }
                }
                //  We complete the merging by appending the contents of the
                //  non-
                //   exhausted array (`VarLtMaxMins' or `VarEqMax') to
                //   `VarLeMaxMins'.
                for (; i1 < VarLtMaxMins.size();) {
                        VarLeMaxMins.push_back(VarLtMaxMins[i1]);
                        ++i1;
                }
                for (; i2 < VarEqMax.size();) {
                        VarLeMaxMins.push_back(VarEqMax[i2]->Var.min());
                        do {
                                ++i2; // avoiding duplicates
                        } while (i2 < VarEqMax.size() &&
                                 VarEqMax[i2]->Var.min() ==
                                     VarEqMax[i2 - 1]->Var.min());
                }
                // `VarLtMaxMins' will be used at the next iteration.
                VarLtMaxMins = VarLeMaxMins;
                //  `vUnion' is a domain representing the union of the
                //   domains that we have examined so far.  Inintially,
                //   it equals to the domain of `VarLeMax[0]'.
                IntDomain* vUnion = new IntDomain(VarLeMax[0]->Var.min(),
                                                  VarLeMax[0]->Var.max());
                NsIntVar::const_gap_iterator gapVal;
                for (gapVal = VarLeMax[0]->Var.gap_begin();
                     gapVal != VarLeMax[0]->Var.gap_end(); ++gapVal) {
                        vUnion->reset(*gapVal);
                        // assert_exc( vUnion->removeSingle(*gapVal,
                        // constraint), 		"ConstrAllDiffStrong::ArcCons:
                        //`vUnion->removeSingle(*gapVal)' failed" );
                }
                //  Iterating through the `VarLeMaxMins', which
                //   has been renamed for readability as `Min'.
                NsDeque<NsInt>& Min = VarLeMaxMins;
                NsIndex nVars = 0;
                NsIndex j = 0;
                for (NsIndex min_i = 0; min_i < Min.size(); ++min_i) {
                        // Iterating through `VarLeMax'...
                        for (; j < VarLeMax.size(); ++j) {
                                if (VarLeMax[j]->Var.min() < Min[min_i])
                                        break; // End of `Min', no propagation.
                                ++nVars;
                                //  We produce the union of the domains `vUnion'
                                //  and `VarLeMax[j]'.  I.e.,
                                //      vUnion  <---  vUnion  UNION
                                //      VarLeMax[j].domain.
                                IntDomain vUnionPrevious = *vUnion;
                                delete vUnion;
                                vUnion =
                                    new IntDomain(min(vUnionPrevious.min(),
                                                      VarLeMax[j]->Var.min()),
                                                  max(vUnionPrevious.max(),
                                                      VarLeMax[j]->Var.max()));
                                //  Removing the values that neither domain
                                //  contains.
                                for (gapVal = vUnionPrevious.gap_begin();
                                     gapVal != vUnionPrevious.gap_end();
                                     ++gapVal) {
                                        if (!VarLeMax[j]->Var.contains(
                                                *gapVal)) {
                                                vUnion->reset(*gapVal);
                                                // assert_exc(
                                                // vUnion->removeSingle(*gapVal,constraint),
                                                // "ConstrAllDiffStrong::ArcCons:
                                                // `vUnion->removeSingle(*gapVal)'
                                                // failed" );
                                        }
                                }
                                for (gapVal = VarLeMax[j]->Var.gap_begin();
                                     gapVal != VarLeMax[j]->Var.gap_end();
                                     ++gapVal) {
                                        if (!vUnionPrevious.contains(*gapVal) &&
                                            vUnion->contains(*gapVal)) {
                                                vUnion->reset(*gapVal);
                                                // assert_exc(
                                                // vUnion->removeSingle(*gapVal,constraint),
                                                // "ConstrAllDiffStrong::ArcCons:
                                                // `vUnion->removeSingle(*gapVal)'
                                                // failed" );
                                        }
                                }
                                NsIndex sizeXcap = vUnion->size() * Capacity;
                                if (sizeXcap < nVars) {
                                        //  The number of the variables is too
                                        //  big to take
                                        //   `sizeXcap' different values.   ==>
                                        //   Failure
                                        VarLeMax[0]->Var.removeAll();
                                        delete vUnion;
                                        delete[] VarArrSorted;
                                        return;
                                }
                                if (sizeXcap == nVars &&
                                    (j < VarLeMax.size() - 1 ||
                                     i < VarArrSortedList.size())) {
                                        //  The number of the variables is the
                                        //  minimum needed to take
                                        //   `sizeXcap' different values
                                        //   (contained in `vUnion'). Each
                                        //   variable has to be assigned a value
                                        //   from `vUnion', and all the values
                                        //   will be used.  So no other variable
                                        //   should be assigned a value from
                                        //   `vUnion'.  Thus, it is possible to
                                        //   subtract this domain from all the
                                        //   other variables, and to separate
                                        //   them from the current group, by
                                        //   changing their group. (This domain
                                        //   is in other words a "Hall
                                        //   Interval.")
                                        ++nGroups;
                                        //						bool
                                        //changed=false;
                                        for (++j; j < VarLeMax.size(); ++j) {
                                                //							changed
                                                //= true;
                                                if (!VarLeMax[j]->removeDomain(
                                                        *vUnion, constraint)) {
                                                        delete vUnion;
                                                        delete[] VarArrSorted;
                                                        return;
                                                }
                                                VarLeMax[j]->setGroup(nGroups);
                                        }
                                        for (; i < VarArrSortedList.size();
                                             ++i) {
                                                //							changed
                                                //= true;
                                                if (!VarArrSorted[i]
                                                         ->removeDomain(
                                                             *vUnion,
                                                             constraint)) {
                                                        delete vUnion;
                                                        delete[] VarArrSorted;
                                                        return;
                                                }
                                                VarArrSorted[i]->setGroup(
                                                    nGroups);
                                        }
                                        AllDiffBoundsConsistency(
                                            VarArr, Capacity, nGroups,
                                            constraint);
                                        max_i = Max.size();
                                        //						if
                                        //( changed )   { 	cout << "
                                        //"; 	for (NsIndex k=0;   k <
                                        //VarArr.size();   ++k)
                                        //		cout << "  " <<
                                        //(VarArr[k].group() -
                                        //groupedNsIntVar::FIRST_GROUP) << ">"
                                        //<< VarArr[k].Var; 	cout << endl;
                                        //						}
                                }
                                if (sizeXcap > VarLeMax.size()) {
                                        //  The number of the values `sizeXcap'
                                        //  to be assigned to be assigned
                                        //   to different variables of the array
                                        //   `VarLeMax' is already greater than
                                        //   the minimum needed, so there is no
                                        //   need to examine the rest of the
                                        //   variables of `VarLeMax'.  Thus, we
                                        //   override them ("early cut-off")
                                        //   using the following statements.
                                        min_i = Min.size();
                                        break;
                                }
                        }
                }
                delete vUnion;
        }
        delete[] VarArrSorted;
        //} // groups iteration
}

} // end namespace

// bounds-consistency only
void ConstrAllDiffStrong::ArcCons(void)
{
        AllDiffBoundsConsistency(VarArr, Capacity, groupedNsIntVar::FIRST_GROUP,
                                 this);
}

// bounds-consistency only
void ConstrAllDiffStrong::LocalArcCons(QueueItem& Qitem)
{
        if (Qitem.getVarFired()->min() < Qitem.getW() &&
            Qitem.getW() < Qitem.getVarFired()->max())
                return; // bounds-consistency does not care
        VarPointerGroup_t::const_iterator cit =
            VarPointerGroup.find((pointer_t)Qitem.getVarFired());
        groupedNsIntVar::group_t groupFired = cit->second->group();
        // ArcCons();
        AllDiffBoundsConsistency(VarArr, Capacity, groupFired, this);
}

Ns_ConstrCount::Ns_ConstrCount(NsIntVarArray* VarArr_init,
                               const NsDeque<NsInt>& Values_init,
                               const NsDeque<NsInt>& Occurrences_init)
  : VarArr(VarArr_init), Values(Values_init), Occurrences(Occurrences_init)
{
        NsProblemManager* pm = 0;
        NsIntVarArray::iterator X = VarArr->begin();
        if (X != VarArr->end())
                pm = &X->manager();
        ++X;
        for (; X != VarArr->end(); ++X) {
                assert_exc(pm == &X->manager(),
                           "Ns_ConstrCount::Ns_ConstrCount: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
        }
        assert_exc(Values.size() == Occurrences.size(),
                   "Ns_ConstrCount::Ns_ConstrCount: `Values' and `Occurrences' "
                   "sizes should match");
        NsIndex occurrencesSum = 0;
        NsIndex i = 0;
        for (NsDeque<NsInt>::const_iterator v = Values.begin();
             v != Values.end(); ++v, ++i) {
                assert_exc(ValueIndex.count(*v) == 0,
                           "Ns_ConstrCount::Ns_ConstrCount: Duplicate value");
                ValueIndex.insert(make_pair(*v, i));
                // assert_exc( *v >= 0 , "Ns_ConstrCount::Ns_ConstrCount:
                // Negative value in `Values'");  vCount.push_back( NsIntVar(*pm,
                // 0, Occurrences[i]) );
                occurrencesSum += Occurrences[i];
        }
        assert_exc(occurrencesSum == VarArr->size(),
                   "Ns_ConstrCount::Ns_ConstrCount: `Occurrences' sum does not "
                   "match `VarArr' size");
}

namespace {

void countArcCons(const NsIntVar& Var, NsIntVarArray& VarArr,
                  const Ns_ConstrCount::ValueIndex_t& ValueIndex,
                  const NsDeque<NsInt>& Occurrences)
{
        if (Var.isBound()) {
                Ns_ConstrCount::ValueIndex_t::const_iterator cit =
                    ValueIndex.find(Var.value());
                if (cit == ValueIndex.end()) {
                        VarArr[0].removeAll();
                        return;
                }
                NsIndex index = cit->second;
                NsInt count = 0;
                for (NsIntVarArray::const_iterator X = VarArr.begin();
                     X != VarArr.end(); ++X) {
                        if (X->isBound() && X->value() == Var.value()) {
                                if (++count > Occurrences[index]) {
                                        VarArr[0].removeAll();
                                        break;
                                }
                        }
                }
        }
}

} // end namespace

void Ns_ConstrCount::ArcCons(void)
{
        for (NsIntVarArray::iterator X = VarArr->begin(); X != VarArr->end();
             ++X) {
                countArcCons(*X, *VarArr, ValueIndex, Occurrences);
        }
}

void Ns_ConstrCount::LocalArcCons(QueueItem& Qitem)
{
        countArcCons(*Qitem.getVarFired(), *VarArr, ValueIndex, Occurrences);
}

// namespace  {
//		void
//	MakeIDs (NsIntVarArray *VarArr)
//	{
//		NsIntVarArray::iterator  X;
//
//		if ( X->IsSetID() )   {
//			X = VarArr->begin();
//			NsIndex  previous = X->getID();
//			++X;
//
//			for ( ;   X != VarArr->end();   ++X)   {
//				assert_exc(X->getID() == previous + 1,  "MakeIDs:
//Non-continuous ids in `VarArr'"); 				previous =
//X->getID();
//			}
//
//		} else  {
//			for (X=VarArr->begin();   X != VarArr->end();   ++X)
//				X->setID();
//		}
//	}
//}  // end namespace

ConstrInverse::ConstrInverse(NsIntVarArray* VarArrInv_init,
                             NsIntVarArray* VarArr_init)
  : VarArrInv(VarArrInv_init),
    VarArr(VarArr_init),
    VArrInv(*VarArrInv),
    VArr(*VarArr)
{
        assert_exc(VarArrInv->size() >= 1 && VarArr->size() >= 1,
                   "ConstrInverse::ConstrInverse: Condition required:  Both "
                   "arrays must have some elements");
        NsIntVarArray::iterator X = VarArr->begin();
        NsProblemManager& pm = X->manager();
        ++X;
        for (; X != VarArr->end(); ++X) {
                assert_exc(&pm == &X->manager(), "ConstrInverse::ConstrInverse:"
                                                 " All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
        for (X = VarArrInv->begin(); X != VarArrInv->end(); ++X) {
                assert_exc(&pm == &X->manager(), "ConstrInverse::ConstrInverse:"
                                                 " All the variables of a "
                                                 "constraint must belong to "
                                                 "the same NsProblemManager");
        }
        NsIndex i;
        for (X = VarArr->begin(), i = 0; X != VarArr->end(); ++X, ++i) {
                assert_exc(VarArrayIndex.count((pointer_t) & *X) == 0,
                           "ConstrInverse::ConstrInverse: Duplicate NsIntVar");
                VarArrayIndex.insert(
                    make_pair((pointer_t) & *X, ArrayIndex_t(false, i)));
        }
        for (X = VarArrInv->begin(), i = 0; X != VarArrInv->end(); ++X, ++i) {
                assert_exc(VarArrayIndex.count((pointer_t) & *X) == 0,
                           "ConstrInverse::ConstrInverse: Duplicate NsIntVar");
                VarArrayIndex.insert(
                    make_pair((pointer_t) & *X, ArrayIndex_t(true, i)));
        }
        // MakeIDs(VarArr);
        // MakeIDs(VarArrInv);
}

void ConstrInverse::ArcCons(void)
{
        NsIndex i;
        NsIntVar::const_iterator val;
        for (i = 0; i < VArrInv.size(); ++i) {
                for (val = VArrInv[i].begin(); val != VArrInv[i].end(); ++val) {
                        if (*val != -1) {
                                if (*val < -1 ||
                                    static_cast<unsigned>(*val) >=
                                        VArr.size() ||
                                    !VArr[*val].contains(i))
                                        VArrInv[i].removeSingle(*val, this);
                                else if (VArr[*val].isBound() &&
                                         VArrInv[i].contains(-1))
                                        VArrInv[i].removeSingle(-1, this);
                        }
                }
        }
        for (i = 0; i < VArr.size(); ++i) {
                for (val = VArr[i].begin(); val != VArr[i].end(); ++val) {
                        if (*val < 0 ||
                            static_cast<unsigned>(*val) >= VArrInv.size() ||
                            !VArrInv[*val].contains(i))
                                VArr[i].removeSingle(*val, this);
                }
        }
}

void ConstrInverse::LocalArcCons(QueueItem& Qitem)
{
        // DomsStore_t::const_iterator  cit;
        // for (cit = DStore.begin();   cit != DStore.end();   ++cit)
        //	( (NsIntVar*)(cit->first) )->setDomain(  cit->second  );
        // NsIndex  VarFiredId = Qitem.getVarFired()->getID();
        NsInt VarFiredW = Qitem.getW();
        if (VarFiredW < 0)
                return;
        VarArrayIndex_t::const_iterator cit =
            VarArrayIndex.find((pointer_t)Qitem.getVarFired());
        NsIndex VarFiredId = cit->second.index;
        // if (VArr[0].getID() <= VarFiredId   &&   VarFiredId < VArr[0].getID()
        // + VArr[0].size())  {
        if (cit->second.InInverse) {
                // assert_exc(VArrInv[0].getID() <= VarFiredId  &&  VarFiredId <
                // VArrInv[0].getID() + VArrInv[0].size(),
                //		"ConstrInverse::LocalArcCons: Wrong
                //`VarFiredId'");  VarFiredId -= VArrInv[0].getID();
                if (static_cast<unsigned>(VarFiredW) >= VArr.size())
                        return;
                if (VArr[VarFiredW].contains(VarFiredId))
                        VArr[VarFiredW].removeSingle(VarFiredId, this);
        } else {
                // VarFiredId -= VArr[0].getID();
                if (Qitem.getVarFired()->isBound()) {
                        NsInt val = Qitem.getVarFired()->value();
                        assert_exc(
                            0 <= val &&
                                static_cast<unsigned>(val) < VArrInv.size(),
                            "ConstrInverse::LocalArcCons: `val' out of range");
                        if (VArrInv[val].contains(-1))
                                VArrInv[val].removeSingle(-1, this);
                }
                if (static_cast<unsigned>(VarFiredW) >= VArrInv.size())
                        return;
                if (VArrInv[VarFiredW].contains(VarFiredId))
                        VArrInv[VarFiredW].removeSingle(VarFiredId, this);
        }
}

void Ns_ConstrElement::ArcCons(void)
{
        if (!VarIndex->removeRange(NsMINUS_INF, -1, this) ||
            !VarIndex->removeRange(intArray.size(), NsPLUS_INF, this)) {
                return;
        }
        NsIntVar::const_iterator index, val;
        for (index = VarIndex->begin(); index != VarIndex->end(); ++index) {
                if (!VarValue->contains(intArray[*index]))
                        VarIndex->removeSingle(*index, this);
        }
        for (val = VarValue->begin(); val != VarValue->end(); ++val) {
                for (index = VarIndex->begin(); index != VarIndex->end();
                     ++index) {
                        if (intArray[*index] == *val)
                                break;
                }
                if (index == VarIndex->end())
                        VarValue->removeSingle(*val, this);
        }
}

void Ns_ConstrElement::LocalArcCons(QueueItem& Qitem)
{
        NsIntVar::const_iterator index;
        if (VarIndex == Qitem.getVarFired()) {
                if (0 <= Qitem.getW() &&
                    static_cast<NsIndex>(Qitem.getW()) < intArray.size()) {
                        NsInt SupportVal = intArray[Qitem.getW()];
                        if (!VarValue->contains(SupportVal))
                                return;
                        for (index = VarIndex->begin();
                             index != VarIndex->end(); ++index) {
                                if (intArray[*index] == SupportVal)
                                        break;
                        }
                        if (index == VarIndex->end())
                                VarValue->removeSingle(SupportVal, this);
                }
        } else {
                assert_exc(VarValue == Qitem.getVarFired(),
                           "Ns_ConstrElement::LocalArcCons: Wrong getVarFired");
                for (index = VarIndex->begin(); index != VarIndex->end();
                     ++index) {
                        if (intArray[*index] == Qitem.getW())
                                VarIndex->removeSingle(*index, this);
                }
        }
}
