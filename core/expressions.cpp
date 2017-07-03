/// @file
/// Processing the expressions between integer constrained variables
///
/// Part of https://github.com/pothitos/naxos

#include "internal.h"
#include "naxos-mini.h"
#include <cstdlib>

using namespace std;
using namespace naxos;

namespace {

enum op_type {
        opPlus,
        opMinus,
        opTimes,
        opDiv,
        opCDivY,
        opMod,
        opCModY,
        opAnd,
        opNand,
        opOr,
        opNor,
        opAbs
};

void exprYopC_post_constr(NsIntVar& VarX, NsIntVar& VarY, const NsInt C,
                          const op_type opType)
{
        Ns_Constraint* newConstr;
        switch (opType) {
        case opPlus:
                newConstr = new Ns_ConstrXeqYplusC(&VarX, &VarY, C);
                break;
        case opMinus:
                newConstr = new Ns_ConstrXeqCminusY(&VarX, C, &VarY);
                break;
        case opTimes:
                newConstr = new Ns_ConstrXeqYtimesC(&VarX, &VarY, C);
                break;
        case opDiv:
                newConstr = new Ns_ConstrXeqYdivC(&VarX, &VarY, C);
                break;
        case opCDivY:
                newConstr = new Ns_ConstrXeqCdivY(&VarX, C, &VarY);
                break;
        case opMod:
                newConstr = new Ns_ConstrXeqYmodC(&VarX, &VarY, C);
                break;
        case opCModY:
                newConstr = new Ns_ConstrXeqCmodY(&VarX, C, &VarY);
                break;
        case opAbs:
                newConstr = new Ns_ConstrXeqAbsY(&VarX, &VarY);
                break;
        default:
                throw NsException("exprYopC_post_constr: Wrong 'opType'");
                break;
        }
        VarX.addConstraint(newConstr);
        VarY.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}
} // end namespace

void Ns_ExprYplusC::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() + C, VarY.max() + C);
        exprYopC_post_constr(VarX, VarY, C, opPlus);
}

NsIntVar& Ns_ExprYplusC::post(void) const
{
        if (C == 0)
                return VarY;
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), VarY.min() + C, VarY.max() + C);
        exprYopC_post_constr(*VarX, VarY, C, opPlus);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprCminusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), C - VarY.max(), C - VarY.min());
        exprYopC_post_constr(VarX, VarY, C, opMinus);
}

NsIntVar& Ns_ExprCminusZ::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), C - VarY.max(), C - VarY.min());
        exprYopC_post_constr(*VarX, VarY, C, opMinus);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYtimesC::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), min(VarY.min() * C, VarY.max() * C),
                        max(VarY.min() * C, VarY.max() * C));
        if (C != 0) // if C==0, there is no constraint
                exprYopC_post_constr(VarX, VarY, C, opTimes);
}

NsIntVar& Ns_ExprYtimesC::post(void) const
{
        if (C == 1)
                return VarY;
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(), min(VarY.min() * C, VarY.max() * C),
                         max(VarY.min() * C, VarY.max() * C));
        if (C != 0) // if C==0, there is no constraint
                exprYopC_post_constr(*VarX, VarY, C, opTimes);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYdivC::post(NsIntVar& VarX) const
{
        if (C >= 0)
                VarX = NsIntVar(VarY.manager(), VarY.min() / C, VarY.max() / C);
        else
                VarX = NsIntVar(VarY.manager(), VarY.max() / C, VarY.min() / C);
        exprYopC_post_constr(VarX, VarY, C, opDiv);
}

NsIntVar& Ns_ExprYdivC::post(void) const
{
        if (C == 1)
                return VarY;
        NsIntVar* VarX;
        if (C >= 0) {
                VarX = new NsIntVar(VarY.manager(), VarY.min() / C,
                                    VarY.max() / C);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarY.max() / C,
                                    VarY.min() / C);
        }
        exprYopC_post_constr(*VarX, VarY, C, opDiv);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprCdivZ::post(NsIntVar& VarX) const
{
        if (C >= 0)
                VarX = NsIntVar(VarZ.manager(), C / VarZ.max(), C / VarZ.min());
        else
                VarX = NsIntVar(VarZ.manager(), C / VarZ.min(), C / VarZ.max());
        exprYopC_post_constr(VarX, VarZ, C, opCDivY);
}

NsIntVar& Ns_ExprCdivZ::post(void) const
{
        NsIntVar* VarX;
        if (C >= 0) {
                VarX = new NsIntVar(VarZ.manager(), C / VarZ.max(),
                                    C / VarZ.min());
        } else {
                VarX = new NsIntVar(VarZ.manager(), C / VarZ.min(),
                                    C / VarZ.max());
        }
        exprYopC_post_constr(*VarX, VarZ, C, opCDivY);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYmodC::post(NsIntVar& VarX) const
{
        NsInt min, max;
        YmodC_min_max(&VarY, C, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        exprYopC_post_constr(VarX, VarY, C, opMod);
}

NsIntVar& Ns_ExprYmodC::post(void) const
{
        if (0 <= VarY.min() && VarY.max() < C)
                return VarY;
        NsInt min, max;
        YmodC_min_max(&VarY, C, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        exprYopC_post_constr(*VarX, VarY, C, opMod);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprCmodZ::post(NsIntVar& VarX) const
{
        NsInt min, max;
        CmodY_min_max(C, &VarZ, min, max);
        VarX = NsIntVar(VarZ.manager(), min, max);
        exprYopC_post_constr(VarX, VarZ, C, opCModY);
}

NsIntVar& Ns_ExprCmodZ::post(void) const
{
        NsInt min, max;
        CmodY_min_max(C, &VarZ, min, max);
        NsIntVar* VarX = new NsIntVar(VarZ.manager(), min, max);
        exprYopC_post_constr(*VarX, VarZ, C, opCModY);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprAbsY::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(),
                        max(labs(max(VarY.min(), static_cast<NsInt>(0))),
                            labs(min(static_cast<NsInt>(0), VarY.max()))),
                        max(labs(VarY.min()), labs(VarY.max())));
        exprYopC_post_constr(VarX, VarY, 0, opAbs);
}

NsIntVar& Ns_ExprAbsY::post(void) const
{
        NsIntVar* VarX =
            new NsIntVar(VarY.manager(),
                         max(labs(max(VarY.min(), static_cast<NsInt>(0))),
                             labs(min(static_cast<NsInt>(0), VarY.max()))),
                         max(labs(VarY.min()), labs(VarY.max())));
        exprYopC_post_constr(*VarX, VarY, 0, opAbs);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

namespace {

void exprYopZ_post_constr(NsIntVar& VarX, NsIntVar& VarY, NsIntVar& VarZ,
                          const op_type opType)
{
        Ns_Constraint* newConstr;
        switch (opType) {
        case opPlus:
                newConstr = new Ns_ConstrXeqYplusZ(&VarX, &VarY, &VarZ);
                break;
        case opMinus:
                newConstr = new Ns_ConstrXeqYplusZ(&VarY, &VarX, &VarZ);
                break;
        case opTimes:
                newConstr = new Ns_ConstrXeqYtimesZ(&VarX, &VarY, &VarZ);
                break;
        case opDiv:
                newConstr = new Ns_ConstrXeqYdivZ(&VarX, &VarY, &VarZ);
                break;
        case opMod:
                newConstr = new Ns_ConstrXeqYmodZ(&VarX, &VarY, &VarZ);
                break;
        case opAnd:
                newConstr = new Ns_ConstrXeqYandZ(&VarX, &VarY, &VarZ, true);
                break;
        case opNand:
                newConstr = new Ns_ConstrXeqYandZ(&VarX, &VarY, &VarZ, false);
                break;
        case opOr:
                newConstr = new Ns_ConstrXeqYorZ(&VarX, &VarY, &VarZ, true);
                break;
        case opNor:
                newConstr = new Ns_ConstrXeqYorZ(&VarX, &VarY, &VarZ, false);
                break;
        default:
                throw NsException("exprYopZ_post_constr: Wrong 'opType'");
                break;
        }
        VarX.addConstraint(newConstr);
        VarY.addConstraint(newConstr);
        VarZ.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

} // end namespace

void Ns_ExprYplusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() + VarZ.min(),
                        VarY.max() + VarZ.max());
        exprYopZ_post_constr(VarX, VarY, VarZ, opPlus);
}

NsIntVar& Ns_ExprYplusZ::post(void) const
{
        NsIntVar* VarX = new NsIntVar(VarY.manager(), VarY.min() + VarZ.min(),
                                      VarY.max() + VarZ.max());
        exprYopZ_post_constr(*VarX, VarY, VarZ, opPlus);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYminusZ::post(NsIntVar& VarX) const
{
        VarX = NsIntVar(VarY.manager(), VarY.min() - VarZ.max(),
                        VarY.max() - VarZ.min());
        exprYopZ_post_constr(VarX, VarY, VarZ, opMinus);
}

NsIntVar& Ns_ExprYminusZ::post(void) const
{
        NsIntVar* VarX = new NsIntVar(VarY.manager(), VarY.min() - VarZ.max(),
                                      VarY.max() - VarZ.min());
        exprYopZ_post_constr(*VarX, VarY, VarZ, opMinus);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYtimesZ::post(NsIntVar& VarX) const
{
        NsInt min, max;
        product_min_max(&VarY, &VarZ, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(VarX, VarY, VarZ, opTimes);
}

NsIntVar& Ns_ExprYtimesZ::post(void) const
{
        NsInt min, max;
        product_min_max(&VarY, &VarZ, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(*VarX, VarY, VarZ, opTimes);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYdivZ::post(NsIntVar& VarX) const
{
        NsInt min, max;
        quotient_min_max(&VarY, &VarZ, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(VarX, VarY, VarZ, opDiv);
}

NsIntVar& Ns_ExprYdivZ::post(void) const
{
        NsInt min, max;
        quotient_min_max(&VarY, &VarZ, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(*VarX, VarY, VarZ, opDiv);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprYmodZ::post(NsIntVar& VarX) const
{
        NsInt min, max;
        remainder_min_max(&VarY, &VarZ, min, max);
        VarX = NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(VarX, VarY, VarZ, opMod);
}

NsIntVar& Ns_ExprYmodZ::post(void) const
{
        NsInt min, max;
        remainder_min_max(&VarY, &VarZ, min, max);
        NsIntVar* VarX = new NsIntVar(VarY.manager(), min, max);
        exprYopZ_post_constr(*VarX, VarY, VarZ, opMod);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprConstrYandZ::postC(NsIntVar& VarX, bool positively) const
{
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.min() * VarZ.min(),
                                VarY.max() * VarZ.max());
                exprYopZ_post_constr(VarX, VarY, VarZ, opAnd);
        } else {
                VarX = NsIntVar(VarY.manager(), !(VarY.max() && VarZ.max()),
                                !(VarY.min() && VarZ.min()));
                exprYopZ_post_constr(VarX, VarY, VarZ, opNand);
        }
}

NsIntVar& Ns_ExprConstrYandZ::postC(bool positively) const
{
        NsIntVar* VarX;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.min() * VarZ.min(),
                                    VarY.max() * VarZ.max());
                exprYopZ_post_constr(*VarX, VarY, VarZ, opAnd);
        } else {
                VarX = new NsIntVar(VarY.manager(), !(VarY.max() && VarZ.max()),
                                    !(VarY.min() && VarZ.min()));
                exprYopZ_post_constr(*VarX, VarY, VarZ, opNand);
        }
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprConstrYorZ::postC(NsIntVar& VarX, bool positively) const
{
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), (VarY.min() + VarZ.min() != 0),
                                (VarY.max() + VarZ.max() != 0));
                exprYopZ_post_constr(VarX, VarY, VarZ, opOr);
        } else {
                VarX = NsIntVar(VarY.manager(), !(VarY.max() + VarZ.max() != 0),
                                !(VarY.min() + VarZ.min() != 0));
                exprYopZ_post_constr(VarX, VarY, VarZ, opNor);
        }
}

NsIntVar& Ns_ExprConstrYorZ::postC(bool positively) const
{
        NsIntVar* VarX;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX =
                    new NsIntVar(VarY.manager(), (VarY.min() + VarZ.min() != 0),
                                 (VarY.max() + VarZ.max() != 0));
                exprYopZ_post_constr(*VarX, VarY, VarZ, opOr);
        } else {
                VarX = new NsIntVar(VarY.manager(),
                                    !(VarY.max() + VarZ.max() != 0),
                                    !(VarY.min() + VarZ.min() != 0));
                exprYopZ_post_constr(*VarX, VarY, VarZ, opNor);
        }
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYorZ::postConstraint(bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively)
                newConstr = new Ns_ConstrXorY(&VarY, &VarZ, true);
        else
                newConstr = new Ns_ConstrXorY(&VarY, &VarZ, false);
        VarY.addConstraint(newConstr);
        VarZ.addConstraint(newConstr);
        return newConstr;
}

namespace {

enum constr_type { cmin, cmax };

void exprGlob_post_constr(NsIntVar& VarX, NsIntVarArray& VarArr,
                          const constr_type ctype)
{
        Ns_Constraint* newConstr;
        switch (ctype) {
        case cmin:
                newConstr = new Ns_ConstrXeqMin(&VarX, &VarArr);
                break;
        case cmax:
                newConstr = new Ns_ConstrXeqMax(&VarX, &VarArr);
                break;
        default:
                throw NsException("exprGlob_post_constr: Wrong 'ctype'");
                break;
        }
        VarX.addConstraint(newConstr);
        for (NsIntVarArray::iterator V = VarArr.begin(); V != VarArr.end(); ++V)
                V->addConstraint(newConstr);
        VarArr.addConstraint();
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

void exprGlobSum_post_constr(NsIntVar& VarX, NsIntVarArray& VarArr,
                             const NsIndex start, const NsIndex length)
{
        Ns_Constraint* newConstr =
            new Ns_ConstrXeqSum(&VarX, &VarArr, start, length);
        VarX.addConstraint(newConstr);
        for (NsIndex i = start; i < start + length; ++i)
                VarArr[i].addConstraint(newConstr);
        VarArr.addConstraint();
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

} // end namespace

void Ns_ExprMin::post(NsIntVar& VarX) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the min of an empty array");
        NsInt min, minmax;
        array_min_minmax(&VarArr, min, minmax);
        VarX = NsIntVar(VarArr[0].manager(), min, minmax);
        exprGlob_post_constr(VarX, VarArr, cmin);
}

NsIntVar& Ns_ExprMin::post(void) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the min of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt min, minmax;
        array_min_minmax(&VarArr, min, minmax);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), min, minmax);
        exprGlob_post_constr(*VarX, VarArr, cmin);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprMax::post(NsIntVar& VarX) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the max of an empty array");
        NsInt maxmin, max;
        array_maxmin_max(&VarArr, maxmin, max);
        VarX = NsIntVar(VarArr[0].manager(), maxmin, max);
        exprGlob_post_constr(VarX, VarArr, cmax);
}

NsIntVar& Ns_ExprMax::post(void) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the max of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt maxmin, max;
        array_maxmin_max(&VarArr, maxmin, max);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), maxmin, max);
        exprGlob_post_constr(*VarX, VarArr, cmax);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

void Ns_ExprSum::post(NsIntVar& VarX) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the sum of an empty array");
        NsInt summin, summax;
        array_sum_min_max(&VarArr, start, length, summin, summax);
        VarX = NsIntVar(VarArr[0].manager(), summin, summax);
        exprGlobSum_post_constr(VarX, VarArr, start, length);
}

NsIntVar& Ns_ExprSum::post(void) const
{
        assert_Ns(!VarArr.empty(), "Cannot get the sum of an empty array");
        if (VarArr.size() == 1)
                return VarArr[0];
        NsInt summin, summax;
        array_sum_min_max(&VarArr, start, length, summin, summax);
        NsIntVar* VarX = new NsIntVar(VarArr[0].manager(), summin, summax);
        exprGlobSum_post_constr(*VarX, VarArr, start, length);
        VarX->manager().recordIntermediateVar(VarX);
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
        assert_Ns(minElement != NsPLUS_INF,
                  "Failed to index the integer array");
}

void exprElement_post_constr(NsIntVar& VarIndex, const NsDeque<NsInt>& intArray,
                             NsIntVar& VarX)
{
        Ns_Constraint* newConstr =
            new Ns_ConstrElement(&VarIndex, intArray, &VarX);
        VarX.addConstraint(newConstr);
        VarIndex.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
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
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

namespace {

void exprVarArrElement_post_constr(NsIntVarArray& VarArr, NsIntVar& VarIndex,
                                   NsIntVar& VarX)
{
        Ns_Constraint* newConstr =
            new Ns_ConstrVarArrElement(VarArr, VarIndex, VarX);
        VarX.addConstraint(newConstr);
        VarIndex.addConstraint(newConstr);
        for (NsIntVarArray::iterator X = VarArr.begin(); X != VarArr.end(); ++X)
                X->addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

} // end namespace

void Ns_ExprVarArrElement::post(NsIntVar& VarX) const
{
        assert_Ns(!VarArr.empty(), "Array of constrained variables is empty");
        NsInt minElement, maxElement;
        array_VarArr_elements_min_max(VarArr, VarIndex, minElement, maxElement);
        VarX = NsIntVar(VarIndex.manager(), minElement, maxElement);
        exprVarArrElement_post_constr(VarArr, VarIndex, VarX);
}

NsIntVar& Ns_ExprVarArrElement::post(void) const
{
        assert_Ns(!VarArr.empty(), "Array of constrained variables is empty");
        NsInt minElement, maxElement;
        array_VarArr_elements_min_max(VarArr, VarIndex, minElement, maxElement);
        NsIntVar* VarX =
            new NsIntVar(VarIndex.manager(), minElement, maxElement);
        exprVarArrElement_post_constr(VarArr, VarIndex, *VarX);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

namespace {

void exprConstrYopZ_post_constr(Ns_Constraint* newConstr, NsIntVar& VarX,
                                NsIntVar& VarY)
{
        VarX.addConstraint(newConstr);
        VarY.addConstraint(newConstr);
        newConstr->ArcCons();
        VarX.manager().recordConstraint(newConstr);
}

void exprConstrYopZ_post_constr(Ns_Constraint* newConstr, NsIntVar& VarX,
                                NsIntVar& VarY, NsIntVar& VarZ)
{
        VarZ.addConstraint(newConstr);
        exprConstrYopZ_post_constr(newConstr, VarX, VarY);
}

} // end namespace

Ns_Constraint* Ns_ExprConstrYlessthanC::postConstraint(bool positively) const
{
        if (!isPositive)
                positively = !positively;
        if (positively)
                VarY.remove(C, NsPLUS_INF);
        else
                VarY.remove(NsMINUS_INF, C - 1);
        return 0; // unary constraint
}

void Ns_ExprConstrYlessthanC::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() < C, VarY.min() < C);
                newConstr = new Ns_ConstrMetaXeqYlessthanC(&VarX, &VarY, C);
        } else {
                VarX =
                    NsIntVar(VarY.manager(), VarY.min() >= C, VarY.max() >= C);
                newConstr =
                    new Ns_ConstrMetaXeqYgreatereqthanC(&VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY);
}

NsIntVar& Ns_ExprConstrYlessthanC::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() < C,
                                    VarY.min() < C);
                newConstr = new Ns_ConstrMetaXeqYlessthanC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarY.min() >= C,
                                    VarY.max() >= C);
                newConstr = new Ns_ConstrMetaXeqYgreatereqthanC(VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYlesseqthanC::postConstraint(bool positively) const
{
        if (!isPositive)
                positively = !positively;
        if (positively)
                VarY.remove(C + 1, NsPLUS_INF);
        else
                VarY.remove(NsMINUS_INF, C);
        return 0; // unary constraint
}

void Ns_ExprConstrYlesseqthanC::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX =
                    NsIntVar(VarY.manager(), VarY.max() <= C, VarY.min() <= C);
                newConstr = new Ns_ConstrMetaXeqYlesseqthanC(&VarX, &VarY, C);
        } else {
                VarX = NsIntVar(VarY.manager(), VarY.min() > C, VarY.max() > C);
                newConstr = new Ns_ConstrMetaXeqYgreaterthanC(&VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY);
}

NsIntVar& Ns_ExprConstrYlesseqthanC::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() <= C,
                                    VarY.min() <= C);
                newConstr = new Ns_ConstrMetaXeqYlesseqthanC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarY.min() > C,
                                    VarY.max() > C);
                newConstr = new Ns_ConstrMetaXeqYgreaterthanC(VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYeqC::postConstraint(bool positively) const
{
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarY.remove(NsMINUS_INF, C - 1);
                VarY.remove(C + 1, NsPLUS_INF);
        } else {
                if (VarY.contains(C))
                        VarY.remove(C);
        }
        return 0; // unary constraint
}

void Ns_ExprConstrYeqC::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(),
                                (VarY.min() == C && VarY.max() == C),
                                VarY.contains(C));
                newConstr = new Ns_ConstrMetaXeqYeqC(&VarX, &VarY, C);
        } else {
                VarX = NsIntVar(VarY.manager(), !VarY.contains(C),
                                (VarY.min() != C || VarY.max() != C));
                newConstr = new Ns_ConstrMetaXeqYneqC(&VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY);
}

NsIntVar& Ns_ExprConstrYeqC::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(),
                                    (VarY.min() == C && VarY.max() == C),
                                    VarY.contains(C));
                newConstr = new Ns_ConstrMetaXeqYeqC(VarX, &VarY, C);
        } else {
                VarX = new NsIntVar(VarY.manager(), !VarY.contains(C),
                                    (VarY.min() != C || VarY.max() != C));
                newConstr = new Ns_ConstrMetaXeqYneqC(VarX, &VarY, C);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYlessthanZ::postConstraint(bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively)
                newConstr = new Ns_ConstrXlessthanY(&VarY, &VarZ);
        else
                newConstr = new Ns_ConstrXlesseqthanY(&VarZ, &VarY);
        VarY.addConstraint(newConstr);
        VarZ.addConstraint(newConstr);
        return newConstr;
}

void Ns_ExprConstrYlessthanZ::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() < VarZ.min(),
                                VarY.min() < VarZ.max());
                newConstr = new Ns_ConstrMetaXeqYlessthanZ(&VarX, &VarY, &VarZ);
        } else {
                VarX = NsIntVar(VarY.manager(), VarZ.max() <= VarY.min(),
                                VarZ.min() <= VarY.max());
                newConstr =
                    new Ns_ConstrMetaXeqYlesseqthanZ(&VarX, &VarZ, &VarY);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY, VarZ);
}

NsIntVar& Ns_ExprConstrYlessthanZ::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() < VarZ.min(),
                                    VarY.min() < VarZ.max());
                newConstr = new Ns_ConstrMetaXeqYlessthanZ(VarX, &VarY, &VarZ);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarZ.max() <= VarY.min(),
                                    VarZ.min() <= VarY.max());
                newConstr =
                    new Ns_ConstrMetaXeqYlesseqthanZ(VarX, &VarZ, &VarY);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY, VarZ);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYlesseqthanZ::postConstraint(bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively)
                newConstr = new Ns_ConstrXlesseqthanY(&VarY, &VarZ);
        else
                newConstr = new Ns_ConstrXlessthanY(&VarZ, &VarY);
        VarY.addConstraint(newConstr);
        VarZ.addConstraint(newConstr);
        return newConstr;
}

void Ns_ExprConstrYlesseqthanZ::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(VarY.manager(), VarY.max() <= VarZ.min(),
                                VarY.min() <= VarZ.max());
                newConstr =
                    new Ns_ConstrMetaXeqYlesseqthanZ(&VarX, &VarY, &VarZ);
        } else {
                VarX = NsIntVar(VarY.manager(), VarZ.max() < VarY.min(),
                                VarZ.min() < VarY.max());
                newConstr = new Ns_ConstrMetaXeqYlessthanZ(&VarX, &VarZ, &VarY);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY, VarZ);
}

NsIntVar& Ns_ExprConstrYlesseqthanZ::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(), VarY.max() <= VarZ.min(),
                                    VarY.min() <= VarZ.max());
                newConstr =
                    new Ns_ConstrMetaXeqYlesseqthanZ(VarX, &VarY, &VarZ);
        } else {
                VarX = new NsIntVar(VarY.manager(), VarZ.max() < VarY.min(),
                                    VarZ.min() < VarY.max());
                newConstr = new Ns_ConstrMetaXeqYlessthanZ(VarX, &VarZ, &VarY);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY, VarZ);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrYeqZ::postConstraint(bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively)
                newConstr = new Ns_ConstrXeqY(&VarY, &VarZ);
        else
                newConstr = new Ns_ConstrXneqY(&VarY, &VarZ);
        VarY.addConstraint(newConstr);
        VarZ.addConstraint(newConstr);
        return newConstr;
}

void Ns_ExprConstrYeqZ::postC(NsIntVar& VarX, bool positively) const
{
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = NsIntVar(
                    VarY.manager(),
                    (VarY.max() == VarZ.min() && VarY.min() == VarZ.max()),
                    !intersectionEmpty(&VarY, &VarZ));
                newConstr = new Ns_ConstrMetaXeqYeqZ(&VarX, &VarY, &VarZ, true);
        } else {
                VarX = NsIntVar(
                    VarY.manager(), intersectionEmpty(&VarY, &VarZ),
                    !(VarY.max() == VarZ.min() && VarY.min() == VarZ.max()));
                newConstr =
                    new Ns_ConstrMetaXeqYeqZ(&VarX, &VarY, &VarZ, false);
        }
        exprConstrYopZ_post_constr(newConstr, VarX, VarY, VarZ);
}

NsIntVar& Ns_ExprConstrYeqZ::postC(bool positively) const
{
        NsIntVar* VarX;
        Ns_Constraint* newConstr;
        if (!isPositive)
                positively = !positively;
        if (positively) {
                VarX = new NsIntVar(VarY.manager(),
                                    VarY.max() == VarZ.min() &&
                                        VarY.min() == VarZ.max(),
                                    !intersectionEmpty(&VarY, &VarZ));
                newConstr = new Ns_ConstrMetaXeqYeqZ(VarX, &VarY, &VarZ, true);
        } else {
                VarX = new NsIntVar(
                    VarY.manager(), intersectionEmpty(&VarY, &VarZ),
                    !(VarY.max() == VarZ.min() && VarY.min() == VarZ.max()));
                newConstr = new Ns_ConstrMetaXeqYeqZ(VarX, &VarY, &VarZ, false);
        }
        exprConstrYopZ_post_constr(newConstr, *VarX, VarY, VarZ);
        VarX->manager().recordIntermediateVar(VarX);
        return *VarX;
}

Ns_Constraint* Ns_ExprConstrAllDiff::postConstraint(bool positively) const
{
        assert_Ns(positively,
                  "Ns_ExprConstrAllDiff::postConstraint: 'positively'==false");
        if (VarArr.size() <= Capacity || (Capacity == 0 && VarArr.size() <= 1))
                return 0; // no constraint
        Ns_Constraint* newConstr;
        if (Capacity == 0)
                newConstr = new Ns_ConstrAllDiff(&VarArr); // default case
        else
                newConstr = new Ns_ConstrAllDiffStrong(&VarArr, Capacity);
        for (NsIntVarArray::iterator X = VarArr.begin(); X != VarArr.end(); ++X)
                X->addConstraint(newConstr);
        VarArr.addConstraint();
        return newConstr;
}

Ns_Constraint* Ns_ExprConstrTable::postConstraint(bool positively) const
{
        assert_Ns(positively,
                  "Ns_ExprConstrTable::postConstraint: 'positively'==false");
        if (VarArr.empty() && table.empty())
                return 0; // no constraint
        Ns_Constraint* newConstr =
            new Ns_ConstrTable(VarArr, table, isSupportsTable);
        for (NsIntVarArray::iterator X = VarArr.begin(); X != VarArr.end(); ++X)
                X->addConstraint(newConstr);
        VarArr.addConstraint();
        return newConstr;
}
