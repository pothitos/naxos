/// @file
/// Unnecessary constraints for the XCSP3 Mini-Solver competition
///
/// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace naxos;
using namespace std;

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

Ns_ConstrCount::Ns_ConstrCount(NsIntVarArray* VarArr_init,
                               const NsDeque<NsInt>& Values,
                               const NsDeque<NsInt>& Occurrences,
                               const NsDeque<NsDeque<NsIndex>>& SplitPositions,
                               const NsIndex Split_init,
                               const NsIndex Dwin_init)
  : VarArr(VarArr_init), Split(Split_init), Dwin(Dwin_init)
{
        NsProblemManager* pm = 0;
        NsIntVarArray::iterator X = VarArr->begin();
        if (X != VarArr->end()) {
                pm = &X->manager();
                ++X;
        }
        for (; X != VarArr->end(); ++X) {
                assert_Ns(pm == &X->manager(), "Ns_ConstrCount::Ns_ConstrCount:"
                                               " All the variables of a "
                                               "constraint must belong to the "
                                               "same NsProblemManager");
        }
        NsIndex i;
        for (i = 0; i < VarArr->size(); ++i) {
                assert_Ns(VarIndex.count((Ns_pointer_t) & (*VarArr)[i]) == 0,
                          "Ns_ConstrCount::Ns_ConstrCount: Duplicate NsIntVar");
                VarIndex.insert(make_pair((Ns_pointer_t) & (*VarArr)[i], i));
        }
        assert_Ns(Values.size() == Occurrences.size(),
                  "Ns_ConstrCount::Ns_ConstrCount: 'Values' and 'Occurrences' "
                  "sizes should match");
        if (Split) {
                assert_Ns(Values.size() == SplitPositions.size(),
                          "Ns_ConstrCount::Ns_ConstrCount: 'Values' and "
                          "'SplitPositions' sizes should match");
        }
        // Sort tuple <Value, Occurrence> by value.
        NsIndex occurrencesSum = 0;
        for (i = 0; i < Values.size(); ++i) {
                if (!Split) {
                        ValuesOccurrences.push_back(
                            ValueOccurrence_t(Values[i], Occurrences[i], *pm));
                } else {
                        ValuesOccurrences.push_back(
                            ValueOccurrence_t(Values[i], Occurrences[i], *pm,
                                              SplitPositions[i], Split));
                        assert_Ns(Occurrences[i] / Split ==
                                      SplitPositions[i].size(),
                                  "Ns_ConstrCount::Ns_ConstrCount: "
                                  "'SplitPositions[i]' size should match "
                                  "'Occurrences[i] / Split'");
                        for (NsIndex j = 0; j < SplitPositions[i].size(); ++j) {
                                assert_Ns(SplitPositions[i][j] < VarArr->size(),
                                          "Ns_ConstrCount::Ns_ConstrCount: "
                                          "Wrong 'SplitPositions[i][j]'");
                        }
                }
                occurrencesSum += Occurrences[i];
        }
        sort(ValuesOccurrences.begin(), ValuesOccurrences.end());
        assert_Ns(occurrencesSum == VarArr->size(),
                  "Ns_ConstrCount::Ns_ConstrCount: 'Occurrences' sum does not "
                  "match 'VarArr' size");
        for (i = 0; i < Values.size(); ++i) {
                assert_Ns(ValueIndex.count(ValuesOccurrences[i].value) == 0,
                          "Ns_ConstrCount::Ns_ConstrCount: Duplicate value");
                ValueIndex.insert(make_pair(ValuesOccurrences[i].value, i));
        }
}

namespace {

void countBoundsCons(
    bool lowerBound, NsIntVarArray& VarArr, const NsIndex i,
    const Ns_ConstrCount::ValueIndex_t& ValueIndex,
    NsDeque<Ns_ConstrCount::ValueOccurrence_t>& ValuesOccurrences,
    const NsIndex Dwin, const Ns_Constraint* constraint)
{
        NsInt val;
        NsInt index;
        val = ((lowerBound) ? VarArr[i].min() : VarArr[i].max());
        Ns_ConstrCount::ValueIndex_t::const_iterator cit_ind =
            ValueIndex.find(val);
        if (cit_ind != ValueIndex.end()) {
                index = cit_ind->second;
        } else {
                Ns_ConstrCount::ValueOccurrence_t val_occur(val);
                NsDeque<Ns_ConstrCount::ValueOccurrence_t>::const_iterator
                    cit_val;
                if (lowerBound) {
                        cit_val =
                            lower_bound(ValuesOccurrences.begin(),
                                        ValuesOccurrences.end(), val_occur);
                } else {
                        cit_val =
                            upper_bound(ValuesOccurrences.begin(),
                                        ValuesOccurrences.end(), val_occur);
                        // In order to put 'cit_val' inside the range.
                        if (cit_val == ValuesOccurrences.end())
                                --cit_val;
                }
                index = cit_val - ValuesOccurrences.begin();
        }
        NsIndex splitIndex = 0;
        while (0 <= index &&
               static_cast<NsIndex>(index) < ValuesOccurrences.size()) {
                splitIndex =
                    ValuesOccurrences[index].splitIndexForPosition(i / Dwin);
                if (ValuesOccurrences[index].vCount[splitIndex].max() != 0 &&
                    VarArr[i].contains(ValuesOccurrences[index].value))
                        break;
                if (lowerBound)
                        ++index;
                else
                        --index;
        }
        if (!(0 <= index &&
              static_cast<NsIndex>(index) < ValuesOccurrences.size())) {
                VarArr[i].removeAll();
        } else {
                if (lowerBound) {
                        VarArr[i].removeRange(
                            NsMINUS_INF, ValuesOccurrences[index].value - 1,
                            constraint);
                } else {
                        VarArr[i].removeRange(ValuesOccurrences[index].value +
                                                  1,
                                              NsPLUS_INF, constraint);
                }
                if (VarArr[i].isBound())
                        ValuesOccurrences[index].vCount[splitIndex].remove(
                            ValuesOccurrences[index].vCount[splitIndex].max());
        }
}

void countArcCons(NsIntVarArray& VarArr, const NsIndex i,
                  const Ns_ConstrCount::ValueIndex_t& ValueIndex,
                  NsDeque<Ns_ConstrCount::ValueOccurrence_t>& ValuesOccurrences,
                  const NsIndex Dwin, const Ns_Constraint* constraint)
{
        countBoundsCons(true, VarArr, i, ValueIndex, ValuesOccurrences, Dwin,
                        constraint);
        if (!VarArr[i].isBound()) {
                countBoundsCons(false, VarArr, i, ValueIndex, ValuesOccurrences,
                                Dwin, constraint);
        }
}

} // end namespace

void Ns_ConstrCount::ArcCons(void)
{
        for (NsIndex i = 0; i < VarArr->size(); ++i) {
                countArcCons(*VarArr, i, ValueIndex, ValuesOccurrences, Dwin,
                             this);
        }
}

void Ns_ConstrCount::LocalArcCons(Ns_QueueItem& Qitem)
{
        VarIndex_t::const_iterator cit =
            VarIndex.find((Ns_pointer_t)Qitem.getVarFired());
        NsIndex i = cit->second;
        countArcCons(*VarArr, i, ValueIndex, ValuesOccurrences, Dwin, this);
}
