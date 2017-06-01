// Part of https://github.com/pothitos/naxos

#include "naxos.h"
#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace naxos;

namespace {
enum op_type {
        opPlus = 1,
        opMinus,
        opTimes,
        opDiv,
        opCDivY,
        opMod,
        opAnd,
        opNand,
        opOr,
        opNor,
        opAbs
};

void ExprYopC_post_constr(NsIntVar& VarX, NsIntVar& VarY, const NsInt C,
                          const op_type opType)
{
        NsConstraint* NewConstr;
        switch (opType) {
        case opPlus:
                NewConstr = new ConstrXeqYplusC(&VarX, &VarY, C);
                break;
        case opMinus:
                NewConstr = new ConstrXeqCminusY(&VarX, C, &VarY);
                break;
        case opTimes:
                NewConstr = new ConstrXeqYtimesC(&VarX, &VarY, C);
                break;
        case opDiv:
                NewConstr = new ConstrXeqYdivC(&VarX, &VarY, C);
                break;
        case opCDivY:
                NewConstr = new ConstrXeqCdivY(&VarX, C, &VarY);
                break;
        case opMod:
                NewConstr = new ConstrXeqYmodC(&VarX, &VarY, C);
                break;
        case opAbs:
                NewConstr = new ConstrXeqAbsY(&VarX, &VarY);
                break;
        default:
                throw NsException("ExprYopC_post_constr: Wrong `opType'");
                break;
        }
        VarX.addConstraint(NewConstr);
        VarY.addConstraint(NewConstr);
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}
} // end namespace

void ExprYplusC::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() + C, VarY.max() + C);
        ExprYopC_post_constr(VarX, VarY, C, opPlus);
}

NsIntVar& ExprYplusC::post(void) const
{
        if (C == 0)
                return VarY;
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), VarY.min() + C, VarY.max() + C);
        ExprYopC_post_constr(*VarX, VarY, C, opPlus);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprCminusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), C - VarY.max(), C - VarY.min());
        ExprYopC_post_constr(VarX, VarY, C, opMinus);
}

NsIntVar& ExprCminusZ::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), C - VarY.max(), C - VarY.min());
        ExprYopC_post_constr(*VarX, VarY, C, opMinus);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprYtimesC::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), min(VarY.min() * C, VarY.max() * C),
                        max(VarY.min() * C, VarY.max() * C));
        if (C != 0) // if C==0 there is no constraint
                ExprYopC_post_constr(VarX, VarY, C, opTimes);
}

NsIntVar& ExprYtimesC::post(void) const
{
        if (C == 1)
                return VarY;
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), min(VarY.min() * C, VarY.max() * C),
                         max(VarY.min() * C, VarY.max() * C));
        if (C != 0) // if C==0 there is no constraint
                ExprYopC_post_constr(*VarX, VarY, C, opTimes);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprYdivC::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() / C, VarY.max() / C);
        ExprYopC_post_constr(VarX, VarY, C, opDiv);
}

NsIntVar& ExprYdivC::post(void) const
{
        if (C == 1)
                return VarY;
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), VarY.min() / C, VarY.max() / C);
        ExprYopC_post_constr(*VarX, VarY, C, opDiv);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprCdivZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarZ.manager(), C / VarZ.max(), C / VarZ.min());
        ExprYopC_post_constr(VarX, VarZ, C, opCDivY);
}

NsIntVar& ExprCdivZ::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarZ.manager(), C / VarZ.max(), C / VarZ.min());
        ExprYopC_post_constr(*VarX, VarZ, C, opCDivY);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprYmodC::post(NsIntVar& VarX) const
{
        NsInt min, max;
        YmodC_min_max(&VarY, C, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        ExprYopC_post_constr(VarX, VarY, C, opMod);
}

NsIntVar& ExprYmodC::post(void) const
{
        if (0 <= VarY.min() && VarY.max() < C)
                return VarY;
        NsInt min, max;
        YmodC_min_max(&VarY, C, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        ExprYopC_post_constr(*VarX, VarY, C, opMod);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprAbsY::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(),
                        max(labs(max(VarY.min(), static_cast<NsInt>(0))),
                            labs(min(static_cast<NsInt>(0), VarY.max()))),
                        max(labs(VarY.min()), labs(VarY.max())));
        ExprYopC_post_constr(VarX, VarY, 0, opAbs);
}

NsIntVar& ExprAbsY::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(),
                         max(labs(max(VarY.min(), static_cast<NsInt>(0))),
                             labs(min(static_cast<NsInt>(0), VarY.max()))),
                         max(labs(VarY.min()), labs(VarY.max())));
        ExprYopC_post_constr(*VarX, VarY, 0, opAbs);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

namespace {

void array_sort(NsDeque<NsInt>& domain)
{
        assert_Ns(!domain.empty(), "NsInDomain: empty domain array");
        sort(domain.begin(), domain.end());
}

void exprInDomain_post_constr(NsIntVar& VarX, const NsDeque<NsInt>& domain)
{
        for (NsIndex i = 0; i < domain.size() - 1; ++i)
                VarX.remove(domain[i] + 1, domain[i + 1] - 1);
}

} // end namespace

void ExprInDomain::post(NsIntVar& VarX) const
{
        array_sort(domain);
        VarX = NsIntVar(pm, domain[0], domain[domain.size() - 1]);
        exprInDomain_post_constr(VarX, domain);
}

NsIntVar& ExprInDomain::post(void) const
{
        array_sort(domain);
        NsIntVar* VarX = new NsIntVar(pm, domain[0], domain[domain.size() - 1]);
        exprInDomain_post_constr(*VarX, domain);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

namespace {
void ExprYplusCZspecial_post_constr(NsIntVar& VarX, NsIntVar& VarY,
                                    const NsInt C, NsIntVar& VarZ)
{
        NsConstraint* NewConstr =
            new ConstrXeqYplusCZspecial(&VarX, &VarY, C, &VarZ);
        VarX.addConstraint(NewConstr);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}
} // end namespace

void ExprYplusCZspecial::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() + C * VarZ.min(),
                        VarY.max() + C * VarZ.max());
        ExprYplusCZspecial_post_constr(VarX, VarY, C, VarZ);
}

NsIntVar& ExprYplusCZspecial::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), VarY.min() + C * VarZ.min(),
                         VarY.max() + C * VarZ.max());
        ExprYplusCZspecial_post_constr(*VarX, VarY, C, VarZ);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

namespace {
// enum op_type {opplus=1, opminus, optimes, opdiv, opmod, opand, opNand, opor,
// opNor};

void ExprYopZ_post_constr(NsIntVar& VarX, NsIntVar& VarY, NsIntVar& VarZ,
                          const op_type opType)
{
        NsConstraint* NewConstr;
        switch (opType) {
        case opPlus:
                NewConstr = new ConstrXeqYplusZ(&VarX, &VarY, &VarZ);
                break;
        case opMinus:
                NewConstr = new ConstrXeqYplusZ(&VarY, &VarX, &VarZ);
                break;
        case opTimes:
                NewConstr = new ConstrXeqYtimesZ(&VarX, &VarY, &VarZ);
                break;
        case opDiv:
                if (VarZ.contains(0))
                        VarZ.removeSingle(0, 0);
                NewConstr = new ConstrXeqYtimesZ(&VarY, &VarX, &VarZ);
                break;
        // case  opMod:
        //	NewConstr = new ConstrXeqYmodZ(&VarX, &VarY, &VarZ);
        //	break;
        case opAnd:
                NewConstr = new ConstrXeqYandZ(&VarX, &VarY, &VarZ, true);
                break;
        case opNand:
                NewConstr = new ConstrXeqYandZ(&VarX, &VarY, &VarZ, false);
                break;
        case opOr:
                NewConstr = new ConstrXeqYorZ(&VarX, &VarY, &VarZ, true);
                break;
        case opNor:
                NewConstr = new ConstrXeqYorZ(&VarX, &VarY, &VarZ, false);
                break;
        default:
                throw NsException("ExprYopZ_post_constr: Wrong `opType'");
                break;
        }
        VarX.addConstraint(NewConstr);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}
} // end namespace

void ExprYplusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() + VarZ.min(),
                        VarY.max() + VarZ.max());
        ExprYopZ_post_constr(VarX, VarY, VarZ, opPlus);
}

NsIntVar& ExprYplusZ::post(void) const
{
        NsIntVar* VarX = new NsIntVar(VarY.manager(), VarY.min() + VarZ.min(),
                                      VarY.max() + VarZ.max());
        ExprYopZ_post_constr(*VarX, VarY, VarZ, opPlus);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprYminusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() - VarZ.max(),
                        VarY.max() - VarZ.min());
        ExprYopZ_post_constr(VarX, VarY, VarZ, opMinus);
}

NsIntVar& ExprYminusZ::post(void) const
{
        NsIntVar* VarX = new NsIntVar(VarY.manager(), VarY.min() - VarZ.max(),
                                      VarY.max() - VarZ.min());
        ExprYopZ_post_constr(*VarX, VarY, VarZ, opMinus);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprYtimesZ::post(NsIntVar& VarX) const
{
        NsInt min, max;
        product_min_max(&VarY, &VarZ, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        ExprYopZ_post_constr(VarX, VarY, VarZ, opTimes);
}

NsIntVar& ExprYtimesZ::post(void) const
{
        NsInt min, max;
        product_min_max(&VarY, &VarZ, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        ExprYopZ_post_constr(*VarX, VarY, VarZ, opTimes);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprConstrYandZ::postC(NsIntVar& VarX, bool positively) const
{
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.min() * VarZ.min(),
                                VarY.max() * VarZ.max());
                ExprYopZ_post_constr(VarX, VarY, VarZ, opAnd);
        } else {
                VarX = NsIntVar(VarY.manager(), !(VarY.max() * VarZ.max()),
                                !(VarY.min() * VarZ.min()));
                ExprYopZ_post_constr(VarX, VarY, VarZ, opNand);
        }
}

NsIntVar& ExprConstrYandZ::postC(bool positively) const
{
        NsIntVar* VarX;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.min() * VarZ.min(),
                                    VarY.max() * VarZ.max());
                ExprYopZ_post_constr(*VarX, VarY, VarZ, opAnd);
        } else {
                VarX = new NsIntVar(VarY.manager(), !(VarY.max() * VarZ.max()),
                                    !(VarY.min() * VarZ.min()));
                ExprYopZ_post_constr(*VarX, VarY, VarZ, opNand);
        }
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprConstrYorZ::postC(NsIntVar& VarX, bool positively) const
{
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), (VarY.min() + VarZ.min() != 0),
                                (VarY.max() + VarZ.max() != 0));
                ExprYopZ_post_constr(VarX, VarY, VarZ, opOr);
        } else {
                VarX = NsIntVar(VarY.manager(), !(VarY.max() + VarZ.max() != 0),
                                !(VarY.min() + VarZ.min() != 0));
                ExprYopZ_post_constr(VarX, VarY, VarZ, opNor);
        }
}

NsIntVar& ExprConstrYorZ::postC(bool positively) const
{
        NsIntVar* VarX;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX =
                    new NsIntVar(VarY.manager(), (VarY.min() + VarZ.min() != 0),
                                 (VarY.max() + VarZ.max() != 0));
                ExprYopZ_post_constr(*VarX, VarY, VarZ, opOr);
        } else {
                VarX = new NsIntVar(VarY.manager(),
                                    !(VarY.max() + VarZ.max() != 0),
                                    !(VarY.min() + VarZ.min() != 0));
                ExprYopZ_post_constr(*VarX, VarY, VarZ, opNor);
        }
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

NsConstraint* ExprConstrYorZ::postConstraint(bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively)
                NewConstr = new ConstrXorY(&VarY, &VarZ, true);
        else
                NewConstr = new ConstrXorY(&VarZ, &VarY, false);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        return NewConstr;
}

namespace {

enum constr_type { //, csum};
        cmin = 1,
        cmax
};

void ExprGlob_post_constr(NsIntVar& VarX, NsIntVarArray& VarArr,
                          const constr_type ctype)
{
        NsConstraint* NewConstr;
        switch (ctype) {
        case cmin:
                NewConstr = new ConstrXeqMin(&VarX, &VarArr);
                break;
        case cmax:
                NewConstr = new ConstrXeqMax(&VarX, &VarArr);
                break;
        // case  csum:
        //	NewConstr = new ConstrXeqSum(&VarX, &VarArr);
        //	break;
        default:
                throw NsException("ExprGlob_post_constr: Wrong `ctype'");
                break;
        }
        VarX.addConstraint(NewConstr);
        for (NsIntVarArray::iterator V = VarArr.begin(); V != VarArr.end(); ++V)
                V->addConstraint(NewConstr);
        VarArr.addConstraint();
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}

void ExprGlobSum_post_constr(NsIntVar& VarX, NsIntVarArray& VarArr,
                             const NsIndex start, const NsIndex length)
{
        NsConstraint* NewConstr =
            new ConstrXeqSum(&VarX, &VarArr, start, length);
        VarX.addConstraint(NewConstr);
        for (NsIndex i = start; i < start + length; ++i)
                VarArr[i].addConstraint(NewConstr);
        VarArr.addConstraint();
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}

} // end namespace

void ExprMin::post(NsIntVar& VarX) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the min of an empty array");
        NsInt min, minmax;
        array_min_minmax(&VarArr, min, minmax);
        VarX = NsIntVar(VarArr[0].manager(), min, minmax);
        ExprGlob_post_constr(VarX, VarArr, cmin);
}

NsIntVar& ExprMin::post(void) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the min of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt min, minmax;
        array_min_minmax(&VarArr, min, minmax);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), min, minmax);
        ExprGlob_post_constr(*VarX, VarArr, cmin);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprMax::post(NsIntVar& VarX) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the max of an empty array");
        NsInt maxmin, max;
        array_maxmin_max(&VarArr, maxmin, max);
        VarX = NsIntVar(VarArr[0].manager(), maxmin, max);
        ExprGlob_post_constr(VarX, VarArr, cmax);
}

NsIntVar& ExprMax::post(void) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the max of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt maxmin, max;
        array_maxmin_max(&VarArr, maxmin, max);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), maxmin, max);
        ExprGlob_post_constr(*VarX, VarArr, cmax);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprSum::post(NsIntVar& VarX) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the sum of an empty array");
        NsInt summin, summax;
        array_sum_min_max(&VarArr, start, length, summin, summax);
        VarX = NsIntVar(VarArr[0].manager(), summin, summax);
        // if (VarArr.size() != 0)	// if equal to zero, there is no
        // constraint
        ExprGlobSum_post_constr(VarX, VarArr, start, length);
}

NsIntVar& ExprSum::post(void) const
{
        assert_exc(VarArr.size() > 0, "Cannot get the sum of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt summin, summax;
        array_sum_min_max(&VarArr, start, length, summin, summax);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), summin, summax);
        // if (VarArr.size() != 0)	// if equal to zero, there is no
        // constraint
        ExprGlobSum_post_constr(*VarX, VarArr, start, length);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

namespace {

void array_elements_min_max(const NsDeque<NsInt>& intArray, NsIntVar& VarIndex,
                            NsInt& minElement, NsInt& maxElement)
{
        minElement = NsPLUS_INF;
        maxElement = NsMINUS_INF;
        for (NsIntVar::const_iterator index = VarIndex.begin();
             index != VarIndex.end(); ++index) {
                if (0 <= *index &&
                    static_cast<NsIndex>(*index) < intArray.size()) {
                        if (intArray[*index] < minElement)
                                minElement = intArray[*index];
                        if (intArray[*index] > maxElement)
                                maxElement = intArray[*index];
                }
        }
        assert_exc(minElement != NsPLUS_INF,
                   "Failed to index the integer array");
}

void exprElement_post_constr(NsIntVar& VarIndex, const NsDeque<NsInt>& intArray,
                             NsIntVar& VarX)
{
        NsConstraint* newConstr =
            new Ns_ConstrElement(&VarIndex, intArray, &VarX);
        VarX.addConstraint(newConstr);
        VarIndex.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().RecordConstraint(newConstr);
}

} // end namespace

void Ns_ExprElement::post(NsIntVar& VarX) const
{
        NsInt minElement, maxElement;
        array_elements_min_max(intArray, VarIndex, minElement, maxElement);
        VarX = NsIntVar(VarIndex.manager(), minElement, maxElement);
        exprElement_post_constr(VarIndex, intArray, VarX);
}

NsIntVar& Ns_ExprElement::post(void) const
{
        NsInt minElement, maxElement;
        array_elements_min_max(intArray, VarIndex, minElement, maxElement);
        NsIntVar* VarX =
            new NsIntVar(VarIndex.manager(), minElement, maxElement);
        exprElement_post_constr(VarIndex, intArray, *VarX);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

void ExprInverse::post(NsIntVarArray& VarArrInv) const
{
        assert_exc(VarArr.size() > 0, "Cannot inverse an empty array");
        assert_exc(VarArrInv.size() == 0,
                   "ExprInverse::post: `VarArrInv' non-empty");
        NsIntVarArray::iterator V;
        V = VarArr.begin();
        NsInt min = V->min();
        NsInt max = V->max();
        ++V;
        for (; V != VarArr.end(); ++V) {
                if (V->min() < min)
                        min = V->min();
                if (V->max() > max)
                        max = V->max();
        }
        assert_exc(min >= 0, "ExprInverse::post: `VarArr' min must be >= 0");
        if (MaxDom != -1) {
                assert_exc(
                    MaxDom >= max,
                    "ExprInverse::post: `MaxDom' is less than `VarArr' max");
                max = MaxDom;
        }
        for (NsIndex i = 0; i <= static_cast<NsIndex>(max); ++i)
                VarArrInv.push_back(
                    NsIntVar(VarArr[0].manager(), -1, VarArr.size() - 1));
        NsConstraint* NewConstr = new ConstrInverse(&VarArrInv, &VarArr);
        for (V = VarArr.begin(); V != VarArr.end(); ++V)
                V->addConstraint(NewConstr);
        VarArr.addConstraint();
        for (V = VarArrInv.begin(); V != VarArrInv.end(); ++V)
                V->addConstraint(NewConstr);
        VarArrInv.addConstraint();
        NewConstr->ArcCons();
        VarArr[0].manager().RecordConstraint(NewConstr);
}

namespace {
void ExprConstrYopZ_post_constr(NsConstraint* NewConstr, NsIntVar& VarX,
                                NsIntVar& VarY)
{
        VarX.addConstraint(NewConstr);
        VarY.addConstraint(NewConstr);
        NewConstr->ArcCons();
        VarX.manager().RecordConstraint(NewConstr);
}

void ExprConstrYopZ_post_constr(NsConstraint* NewConstr, NsIntVar& VarX,
                                NsIntVar& VarY, NsIntVar& VarZ)
{
        VarZ.addConstraint(NewConstr);
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY);
}
} // end namespace

NsConstraint* ExprConstrYlessthanC::postConstraint(bool positively) const
{
        if (!is_positive)
                positively = !positively;
        if (positively) {
                // while ( VarY.max() >= C )   {
                //	if ( !VarY.removeSingle( VarY.max(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                VarY.removeRange(C, NsPLUS_INF, 0);
        } else {
                // while ( VarY.min() < C )   {
                //	if ( !VarY.removeSingle( VarY.min(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                VarY.removeRange(NsMINUS_INF, C - 1, 0);
        }
        return 0; // unary constraint
}

void ExprConstrYlessthanC::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() < C, VarY.min() < C);
                NewConstr = new ConstrMetaXeqYlessthanC(&VarX, &VarY, C);
        } else {
                VarX =
                    NsIntVar(VarY.manager(), VarY.min() >= C, VarY.max() >= C);
                NewConstr = new ConstrMetaXeqYgreatereqthanC(&VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY);
}

NsIntVar& ExprConstrYlessthanC::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() < C,
                                    VarY.min() < C);
                NewConstr = new ConstrMetaXeqYlessthanC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarY.min() >= C,
                                    VarY.max() >= C);
                NewConstr = new ConstrMetaXeqYgreatereqthanC(VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

NsConstraint* ExprConstrYlesseqthanC::postConstraint(bool positively) const
{
        if (!is_positive)
                positively = !positively;
        if (positively) {
                // while ( VarY.max() > C )   {
                //	if ( !VarY.removeSingle( VarY.max(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                VarY.removeRange(C + 1, NsPLUS_INF, 0);
        } else {
                // while ( VarY.min() <= C )   {
                //	if ( !VarY.removeSingle( VarY.min(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                VarY.removeRange(NsMINUS_INF, C, 0);
        }
        return 0; // unary constraint
}

void ExprConstrYlesseqthanC::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX =
                    NsIntVar(VarY.manager(), VarY.max() <= C, VarY.min() <= C);
                NewConstr = new ConstrMetaXeqYlesseqthanC(&VarX, &VarY, C);
        } else {
                VarX = NsIntVar(VarY.manager(), VarY.min() > C, VarY.max() > C);
                NewConstr = new ConstrMetaXeqYgreaterthanC(&VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY);
}

NsIntVar& ExprConstrYlesseqthanC::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() <= C,
                                    VarY.min() <= C);
                NewConstr = new ConstrMetaXeqYlesseqthanC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarY.min() > C,
                                    VarY.max() > C);
                NewConstr = new ConstrMetaXeqYgreaterthanC(VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

//	NsConstraint*
// ExprConstrYgreaterthanC::postConstraint (void) const
//{
//	while ( VarY.min() <= C )   {
//		if ( !VarY.removeSingle( VarY.min(), 0 ) )
//			return  0;	// ... to avoid an infinite loop
//	}
//	return  0;  // unary constraint
//}
//
//
//
//	void
// ExprConstrYgreaterthanC::post (NsIntVar *VarX) const
//{
//	*VarX = NsIntVar(*VarY.manager(), VarY.min() > C, VarY.max() > C);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYgreaterthanC(VarX, &VarY,
//C); 	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//}
//
//
//	NsIntVar&
// ExprConstrYgreaterthanC::post (void) const
//{
//	NsIntVar  *VarX = new NsIntVar(*VarY.manager(), VarY.min() > C,
//VarY.max() > C);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYgreaterthanC(VarX, &VarY,
//C); 	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//
//	VarX->manager()->RecordIntermediateVar( VarX );
//	return  *VarX;
//}

//	NsConstraint*
// ExprConstrYgreatereqthanC::postConstraint (void) const
//{
//	while ( VarY.min() < C )   {
//		if ( !VarY.removeSingle( VarY.min(), 0 ) )
//			return  0;	// ... to avoid an infinite loop
//	}
//	return  0;  // unary constraint
//}
//
//
//	void
// ExprConstrYgreatereqthanC::post (NsIntVar *VarX) const
//{
//	*VarX = NsIntVar(*VarY.manager(), VarY.min() >= C, VarY.max() >= C);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYgreatereqthanC(VarX, &VarY,
//C); 	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//}
//
//
//	NsIntVar&
// ExprConstrYgreatereqthanC::post (void) const
//{
//	NsIntVar  *VarX = new NsIntVar(*VarY.manager(), VarY.min() >= C,
//VarY.max() >= C);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYgreatereqthanC(VarX, &VarY,
//C); 	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//
//	VarX->manager()->RecordIntermediateVar( VarX );
//	return  *VarX;
//}

NsConstraint* ExprConstrYeqC::postConstraint(bool positively) const
{
        if (!is_positive)
                positively = !positively;
        if (positively) {
                // while ( VarY.min() < C )   {
                //	if ( !VarY.removeSingle( VarY.min(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                // while ( VarY.max() > C )   {
                //	if ( !VarY.removeSingle( VarY.max(), 0 ) )
                //		return  0;	// ... to avoid an infinite loop
                //}
                VarY.removeRange(NsMINUS_INF, C - 1, 0);
                VarY.removeRange(C + 1, NsPLUS_INF, 0);
        } else {
                if (VarY.contains(C))
                        VarY.remove(C);
        }
        return 0; // unary constraint
}

void ExprConstrYeqC::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(),
                                (VarY.min() == C && VarY.max() == C),
                                VarY.contains(C));
                NewConstr = new ConstrMetaXeqYeqC(&VarX, &VarY, C);
        } else {
                VarX = NsIntVar(VarY.manager(), !VarY.contains(C),
                                (VarY.min() != C || VarY.max() != C));
                NewConstr = new ConstrMetaXeqYneqC(&VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY);
}

NsIntVar& ExprConstrYeqC::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(),
                                    (VarY.min() == C && VarY.max() == C),
                                    VarY.contains(C));
                NewConstr = new ConstrMetaXeqYeqC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), !VarY.contains(C),
                                    (VarY.min() != C || VarY.max() != C));
                NewConstr = new ConstrMetaXeqYneqC(VarX, &VarY, C);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

//	NsConstraint*
// ExprConstrYneqC::postConstraint (void) const
//{
//	if (VarY.contains( C ))
//		VarY.remove( C );
//	return  0;  // unary constraint
//}
//
//
//	void
// ExprConstrYneqC::post (NsIntVar *VarX) const
//{
//	*VarX = NsIntVar(*VarY.manager(), !VarY.contains(C), (VarY.min()!=C ||
//VarY.max()!=C));
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYneqC(VarX, &VarY, C);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//}
//
//
//	NsIntVar&
// ExprConstrYneqC::post (void) const
//{
//	NsIntVar  *VarX = new NsIntVar(*VarY.manager(), !VarY.contains(C),
//(VarY.min()!=C || VarY.max()!=C));
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYneqC(VarX, &VarY, C);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//
//	VarX->manager()->RecordIntermediateVar( VarX );
//	return  *VarX;
//}

NsConstraint* ExprConstrYlessthanZ::postConstraint(bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively)
                NewConstr = new ConstrXlessthanY(&VarY, &VarZ);
        else
                NewConstr = new ConstrXlesseqthanY(&VarZ, &VarY);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        return NewConstr;
}

void ExprConstrYlessthanZ::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() < VarZ.min(),
                                VarY.min() < VarZ.max());
                NewConstr = new ConstrMetaXeqYlessthanZ(&VarX, &VarY, &VarZ);
        } else {
                VarX = NsIntVar(VarY.manager(), VarZ.max() <= VarY.min(),
                                VarZ.min() <= VarY.max());
                NewConstr = new ConstrMetaXeqYlesseqthanZ(&VarX, &VarZ, &VarY);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY, VarZ);
}

NsIntVar& ExprConstrYlessthanZ::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() < VarZ.min(),
                                    VarY.min() < VarZ.max());
                NewConstr = new ConstrMetaXeqYlessthanZ(VarX, &VarY, &VarZ);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarZ.max() <= VarY.min(),
                                    VarZ.min() <= VarY.max());
                NewConstr = new ConstrMetaXeqYlesseqthanZ(VarX, &VarZ, &VarY);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY, VarZ);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

NsConstraint* ExprConstrYlesseqthanZ::postConstraint(bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively)
                NewConstr = new ConstrXlesseqthanY(&VarY, &VarZ);
        else
                NewConstr = new ConstrXlessthanY(&VarZ, &VarY);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        return NewConstr;
}

void ExprConstrYlesseqthanZ::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() <= VarZ.min(),
                                VarY.min() <= VarZ.max());
                NewConstr = new ConstrMetaXeqYlesseqthanZ(&VarX, &VarY, &VarZ);
        } else {
                VarX = NsIntVar(VarY.manager(), VarZ.max() < VarY.min(),
                                VarZ.min() < VarY.max());
                NewConstr = new ConstrMetaXeqYlessthanZ(&VarX, &VarZ, &VarY);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY, VarZ);
}

NsIntVar& ExprConstrYlesseqthanZ::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() <= VarZ.min(),
                                    VarY.min() <= VarZ.max());
                NewConstr = new ConstrMetaXeqYlesseqthanZ(VarX, &VarY, &VarZ);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarZ.max() < VarY.min(),
                                    VarZ.min() < VarY.max());
                NewConstr = new ConstrMetaXeqYlessthanZ(VarX, &VarZ, &VarY);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY, VarZ);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

NsConstraint* ExprConstrYeqZ::postConstraint(bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively)
                NewConstr = new ConstrXeqY(&VarY, &VarZ);
        else
                NewConstr = new ConstrXneqY(&VarY, &VarZ);
        VarY.addConstraint(NewConstr);
        VarZ.addConstraint(NewConstr);
        return NewConstr;
}

void ExprConstrYeqZ::postC(NsIntVar& VarX, bool positively) const
{
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(),
                                VarY.max() == VarZ.min() &&
                                    VarY.min() == VarZ.max(),
                                !IntersectionEmpty(&VarY, &VarZ));
                NewConstr = new ConstrMetaXeqYeqZ(&VarX, &VarY, &VarZ, true);
        } else {
                VarX = NsIntVar(
                    VarY.manager(), IntersectionEmpty(&VarY, &VarZ),
                    !(VarY.max() == VarZ.min() && VarY.min() == VarZ.max()));
                NewConstr = new ConstrMetaXeqYeqZ(&VarX, &VarY, &VarZ, false);
        }
        ExprConstrYopZ_post_constr(NewConstr, VarX, VarY, VarZ);
}

NsIntVar& ExprConstrYeqZ::postC(bool positively) const
{
        NsIntVar* VarX;
        NsConstraint* NewConstr;
        if (!is_positive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(),
                                    VarY.max() == VarZ.min() &&
                                        VarY.min() == VarZ.max(),
                                    !IntersectionEmpty(&VarY, &VarZ));
                NewConstr = new ConstrMetaXeqYeqZ(VarX, &VarY, &VarZ, true);
        } else {
                VarX = new NsIntVar(
                    VarY.manager(), IntersectionEmpty(&VarY, &VarZ),
                    !(VarY.max() == VarZ.min() && VarY.min() == VarZ.max()));
                NewConstr = new ConstrMetaXeqYeqZ(VarX, &VarY, &VarZ, false);
        }
        ExprConstrYopZ_post_constr(NewConstr, *VarX, VarY, VarZ);
        VarX->manager().RecordIntermediateVar(VarX);
        return *VarX;
}

//	NsConstraint*
// ExprConstrYneqZ::postConstraint (void) const
//{
//	NsConstraint  *NewConstr = new ConstrXneqY(&VarY, &VarZ);
//	VarY.addConstraint(NewConstr);
//	VarZ.addConstraint(NewConstr);
//	return  NewConstr;
//}
//
//
//	void
// ExprConstrYneqZ::post (NsIntVar *VarX) const
//{
//	*VarX = NsIntVar(*VarY.manager(), IntersectionEmpty(&VarY, &VarZ),
//			!(VarY.max()==VarZ.min() && VarY.min()==VarZ.max()));
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYneqZ(VarX, &VarY, &VarZ);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY, &VarZ);
//}
//
//
//	NsIntVar&
// ExprConstrYneqZ::post (void) const
//{
//	NsIntVar  *VarX = new NsIntVar(*VarY.manager(), IntersectionEmpty(&VarY,
//&VarZ),
//			!(VarY.max()==VarZ.min() && VarY.min()==VarZ.max()));
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYneqZ(VarX, &VarY, &VarZ);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY, &VarZ);
//
//	VarX->manager()->RecordIntermediateVar( VarX );
//	return  *VarX;
//}

//	NsConstraint*
// ExprConstrNegationY::postConstraint (void) const
//{
//	while ( VarY->min() < 0 )   {
//		if ( !VarY->removeSingle( VarY->min(), 0 ) )
//			return;	// ... to avoid an infinite loop
//	}
//	while ( VarY->max() > 0 )   {
//		if ( !VarY->removeSingle( VarY->max(), 0 ) )
//			return;	// ... to avoid an infinite loop
//	}
//	return  0;
//}
//
//
//	void
// ExprConstrNegationY::post (NsIntVar *VarX) const
//{
//	*VarX = NsIntVar(*VarY.manager(), VarY.max() < 1, VarY.min() < 1);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYlessthanC(VarX, &VarY, 1);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//}
//
//
//	NsIntVar&
// ExprConstrNegationY::post (void) const
//{
//	NsIntVar  *VarX = new NsIntVar(*VarY.manager(), VarY.max() < 1,
//VarY.min() < 1);
//
//	NsConstraint  *NewConstr = new ConstrMetaXeqYlessthanC(VarX, &VarY, 1);
//	ExprConstrYopZ_post_constr(NewConstr, VarX, &VarY);
//
//	VarX->manager()->RecordIntermediateVar( VarX );
//	return  *VarX;
//}

NsConstraint* ExprConstrAllDiff::postConstraint(bool positively) const
{
        assert_exc(positively,
                   "ExprConstrAllDiff::postConstraint: `positively'==false");
        if (VarArr.size() <= Capacity || (Capacity == 0 && VarArr.size() <= 1))
                return 0; // no constraint
        NsConstraint* NewConstr;
        if (Capacity == 0)
                // default case
                NewConstr = new ConstrAllDiff(&VarArr);
        else
                NewConstr = new ConstrAllDiffStrong(&VarArr, Capacity);
        for (NsIntVarArray::iterator X = VarArr.begin(); X != VarArr.end(); ++X)
                X->addConstraint(NewConstr);
        VarArr.addConstraint();
        return NewConstr;
}

NsConstraint* Ns_ExprConstrCount::postConstraint(bool positively) const
{
        assert_exc(positively,
                   "Ns_ExprConstrCount::postConstraint: `positively'==false");
        NsConstraint* newConstr =
            new Ns_ConstrCount(&VarArr, Values, Occurrences);
        for (NsIntVarArray::iterator X = VarArr.begin(); X != VarArr.end(); ++X)
                X->addConstraint(newConstr);
        VarArr.addConstraint();
        return newConstr;
}
