/// @file
/// Constraint finite domain integer variables implementation
///
/// Part of https://github.com/pothitos/naxos

#include "naxos-mini.h"
using namespace std;
using namespace naxos;

/// Prints the domain to standard output
ostream& naxos::operator<<(ostream& os, const Ns_BitSet& domain)
{
        os << "[";
        NsInt val, gap;
        gap = NsMINUS_INF;
        while ((val = domain.next(gap)) != NsPLUS_INF) {
                os << val;
                if ((gap = domain.nextGap(val)) == NsPLUS_INF) {
                        if (domain.max() != val)
                                os << ".." << domain.max();
                        break;
                }
                if (gap == val + 1)
                        os << " ";
                else
                        os << ".." << gap - 1 << " ";
        }
        os << "]";
        return os;
}

std::ostream& naxos::operator<<(std::ostream& os, const NsIntVarArray& VarArr)
{
        os << "[";
        NsIntVarArray::const_iterator V = VarArr.begin();
        if (V != VarArr.end()) {
                os << *V;
                ++V;
        }
        for (/*VOID*/; V != VarArr.end(); ++V)
                os << " " << *V;
        os << "]";
        return os;
}

/// Removes in purpose all the values of the domain, thus making inconsistency
void NsIntVar::removeAll(void)
{
        pm->foundAnInconsistency();
}

/// A bound (min or max) of the domain has been changed by the constraint constr
void Ns_QueueItem::boundChangedBy(const Ns_Constraint* constr)
{
        removedBoundRec.boundChangedBy(
            constr, varFired->manager().numConstraintChecks());
}

/// Adds the tuple (removedValue, constraintThatRemovedIt) of the removedValues
void Ns_QueueItem::add(const NsInt removedVal,
                       const Ns_Constraint* constrThatRemovedIt)
{
        removedValues.push_back(
            RemovedValueRecord_t(removedVal, constrThatRemovedIt));
}

bool NsIntVar::removeRange(const NsInt first, const NsInt last,
                           const Ns_Constraint* constr, bool& modified)
{
        bool rangeEmpty = true;
        // Check for bounds modifications.
        if ((first <= min() && min() <= last) ||
            (first <= max() && max() <= last)) {
                rangeEmpty = false;
                if (queueItem == 0) {
                        pm->getQueue().push(Ns_QueueItem(this));
                        queueItem = &pm->getQueue().back();
                }
                queueItem->boundChangedBy(constr);
        }
        // Check for modifications of the intermediate values of the
        // domain, if necessary. (E.g. when they must be stored for arc
        // consistency constraints checks.)
        if (rangeEmpty || storeRemovedValues()) {
                NsInt newFirst = (first != NsMINUS_INF) ? first - 1 : first;
                NsInt newLast = (last != NsPLUS_INF) ? last + 1 : last;
                while ((newFirst = next(newFirst)) < newLast) {
                        rangeEmpty = false;
                        // Uncomment the following for AC-3:
                        // if (queueItem == 0) {
                        //        pm->getQueue().push(Ns_QueueItem(this));
                        //        queueItem = &pm->getQueue().back();
                        //}
                        // queueItem->boundChangedBy(constr);
                        if (!storeRemovedValues())
                                break;
                        if (queueItem == 0) {
                                pm->getQueue().push(Ns_QueueItem(this));
                                queueItem = &pm->getQueue().back();
                        }
                        queueItem->add(newFirst, constr);
                }
        }
        if (!rangeEmpty) {
                if (!domain.removeRange(first, last)) {
                        pm->foundAnInconsistency();
                        return false;
                } else {
                        modified = true;
                }
        }
        return true;
}

void NsIntVar::addConstraint(Ns_Constraint* constr)
{
        constraints.push_back(ConstraintAndFailure(constr));
        assert_Ns(constr->varsInvolvedIn() >= 1,
                  "NsIntVar::addConstraint: Wrong 'varsInvolvedIn' constraint "
                  "'constr'");
        arcsConnectedTo += constr->varsInvolvedIn() - 1;
        if (constr->revisionType == Ns_Constraint::VALUE_CONSISTENCY)
                constraintNeedsRemovedValues = true;
}

NsIntVar::NsIntVar(NsProblemManager& pm_init, const NsInt min_init,
                   const NsInt max_init)
  : pm(&pm_init),
    domain(pm_init, min_init, max_init),
    arcsConnectedTo(0),
    constraintNeedsRemovedValues(false),
    queueItem(0)
{
        pm->addVar(this);
}

NsIntVar::NsIntVar(const Ns_Expression& expr)
{
        expr.post(*this);
        pm->removeLastVar();
        pm->addVar(this);
}

NsIntVar& NsIntVar::operator=(const Ns_Expression& expr)
{
        assert_Ns(pm == 0 && constraints.empty() &&
                      constraintNeedsRemovedValues == false &&
                      arcsConnectedTo == 0 && queueItem == 0,
                  "NsIntVar::operator=: Some constraints already imposed on "
                  "'*this'");
        expr.post(*this);
        pm->removeLastVar();
        pm->addVar(this);
        return *this;
}

/// Makes variable transparent to backtracking/store; useful for temporary ones
void NsIntVar::transparent(void)
{
        domain.lastSaveHistoryId() = manager().getCurrentHistoryId();
        manager().removeLastVar();
}

NsIntVarArray::NsIntVarArray(const Ns_ExpressionArray& expr)
  : addedConstraint(false)
{
        expr.post(*this);
}

NsIntVarArray& NsIntVarArray::operator=(const Ns_ExpressionArray& expr)
{
        assert_Ns(PointArray.empty() && !addedConstraint,
                  "NsIntVarArray::operator=: Some constraints already imposed "
                  "on '*this'");
        expr.post(*this);
        return *this;
}

void NsIntVarArray::push_front(const NsIntVar& Var)
{
        assert_Ns(!addedConstraint,
                  "NsIntVarArray::push_front: Cannot add another variable, "
                  "because a constraint has been already imposed on the array");
        NsIntVar* NewVar = new NsIntVar(Var);
        PointArray.push_front(NewVar);
        NewVar->manager().recordIntermediateVar(NewVar);
        NewVar->manager().removeLastVar();
        NewVar->manager().addVar(NewVar);
}

void NsIntVarArray::push_front(const Ns_Expression& expr)
{
        assert_Ns(!addedConstraint,
                  "NsIntVarArray::push_front: Cannot add another variable, "
                  "because a constraint has been already imposed on the array");
        PointArray.push_front(&expr.post());
}

void NsIntVarArray::push_back(const NsIntVar& Var)
{
        assert_Ns(!addedConstraint,
                  "NsIntVarArray::push_back: Cannot add another variable, "
                  "because a constraint has been already imposed on the array");
        NsIntVar* NewVar = new NsIntVar(Var);
        PointArray.push_back(NewVar);
        NewVar->manager().recordIntermediateVar(NewVar);
        NewVar->manager().removeLastVar();
        NewVar->manager().addVar(NewVar);
}

void NsIntVarArray::push_back(const Ns_Expression& expr)
{
        assert_Ns(!addedConstraint,
                  "NsIntVarArray::push_back: Cannot add another variable, "
                  "because a constraint has been already imposed on the array");
        PointArray.push_back(&expr.post());
}
