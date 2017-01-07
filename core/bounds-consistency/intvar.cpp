/// @file Constraint finite domain integer variables implementation
/// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace std;
using namespace naxos;

///  Prints the domain to standard output.

ostream&
naxos::operator << (ostream& os, const Ns_BitSet& domain)
{
        os << "[";
        NsInt  val, gap;
        gap  =  NsMINUS_INF;
        while ( (val = domain.next(gap))  !=  NsPLUS_INF ) {
                os << val;
                if ( (gap = domain.nextGap(val))  ==  NsPLUS_INF ) {
                        if ( domain.max()  !=  val )
                                os << ".." << domain.max();
                        break;
                }
                if ( gap  ==  val + 1 )
                        os << " ";
                else
                        os << ".." << gap-1 << " ";
        }
        os << "]";
        return  os;
}

std::ostream&
naxos::operator  << (std::ostream& os, const NsIntVarArray& VarArr)
{
        os << "[";
        NsIntVarArray::const_iterator  V = VarArr.begin();
        if ( V  !=  VarArr.end() ) {
                os << *V;
                ++V;
        }
        for ( ;   V != VarArr.end();   ++V)
                os << " " << *V;
        os << "]";
        return  os;
}

///  To remove in purpose all the values of the domain, thus making an inconsistency.

void
NsIntVar::removeAll (void)
{
        pm->foundAnInconsistency();
        //remove(NsMINUS_INF, NsPLUS_INF);
}

///  A bound (min or max) of the domain has been changed by the constraint constr.

void
Ns_QueueItem::boundChangedBy (const Ns_Constraint *constr)
{
        //if ( stale )
        //      varFired->manager().getQueue().pushToEnd( iter );
        removedBoundRec.boundChangedBy(constr ,
                                       varFired->manager().numConstraintChecks() );
}

///  Adds the tuple (removedValue,constraintThatRemovedIt) ot the removedValues collection.

void
Ns_QueueItem::add (const NsInt removedVal, const Ns_Constraint *constrThatRemovedIt)
{
        //if ( stale )
        //      varFired->manager().getQueue().pushToEnd( iter );
        removedValues.push_back(
                RemovedValueRecord_t( removedVal, constrThatRemovedIt) );
}

bool
NsIntVar::removeRange (const NsInt first, const NsInt last,
                       const Ns_Constraint *constr, bool& modified)
{
        bool  rangeEmpty = true;
        //  Check for bounds modifications.
        if ( ( first <= min() && min() <= last )
             || ( first <= max() && max() <= last ) ) {
                rangeEmpty  =  false;
                ////  To get the appropriate queueItem()...
                //if ( !pm->isCurrentHistoryId(domain.lastSaveHistoryId()) )
                //      pm->saveBitsetDomain(domain);
                if ( queueItem  ==  0 ) {
                        pm->getQueue().push( Ns_QueueItem(this) );
                        queueItem  =  & pm->getQueue().back();
                }
                queueItem->boundChangedBy(constr);
                //for (NsDeque<NsIntVar::ConstraintAndFailure>::size_type  currentConstr = 0;
                //      currentConstr < constraints.size();
                //      ++currentConstr)
                //{
                //      if ( constraints[currentConstr].constr->revisionType ==
                //                      Ns_Constraint::BIDIRECTIONAL_CONSISTENCY_OUT_OF_QUEUE )
                //      {
                //              constraints[currentConstr].constr->revisionType  =
                //                      Ns_Constraint::BIDIRECTIONAL_CONSISTENCY_IN_QUEUE;
                //      }
                //}
        }
        //  Check for modifications of the intermediate values of the
        //   domain, if necessary.  (E.g. when they must be stored for arc
        //   consistency constraints checks.)
        if ( rangeEmpty  ||  storeRemovedValues() ) {
                NsInt  newFirst = ( first  !=  NsMINUS_INF ) ?
                                  first - 1
                                  : first;
                NsInt  newLast  = ( last  !=  NsPLUS_INF ) ?
                                  last + 1
                                  : last;
                while ( ( newFirst = next(newFirst) )  <  newLast ) {
                        rangeEmpty  =  false;
                        //  Uncomment the following for AC-3.
                        //if ( queueItem  ==  0 )    {
                        //
                        //      pm->getQueue().push( Ns_QueueItem(this) );
                        //
                        //      queueItem  =  & pm->getQueue().back();
                        //}
                        //
                        //queueItem->boundChangedBy(constr);
                        if ( !storeRemovedValues() )
                                break;
                        ////  To get the appropriate queueItem()...
                        //if ( !pm->isCurrentHistoryId(domain.lastSaveHistoryId()) )
                        //      pm->saveBitsetDomain(domain);
                        if ( queueItem  ==  0 ) {
                                pm->getQueue().push( Ns_QueueItem(this) );
                                queueItem  =  & pm->getQueue().back();
                        }
                        queueItem->add(newFirst, constr);
                }
        }
        if ( !rangeEmpty ) {
                if ( !domain.removeRange(first,last) ) {
                        pm->foundAnInconsistency();
                        return  false;
                } else {
                        modified  =  true;
                }
        }
        return  true;
}

void
NsIntVar::addConstraint (Ns_Constraint *constr)
{
        constraints.push_back( ConstraintAndFailure(constr) );
        //if ( constr->needsRemovedValues() )
        //      constraintsArcCons.push_back(constr);
        //else
        //      constraintsBoundsCons.push_back(constr);
        assert_Ns( constr->varsInvolvedIn() >= 1 ,
                   "NsIntVar::addConstraint: Wrong `varsInvolvedIn' constraint `constr'");
        arcsConnectedTo  +=  constr->varsInvolvedIn() - 1;
        if ( constr->revisionType  ==  Ns_Constraint::VALUE_CONSISTENCY )
                constraintNeedsRemovedValues  =  true;
        //|| constr->needsRemovedValues();
}

NsIntVar::NsIntVar (NsProblemManager& pm_init, const NsInt min_init, const NsInt max_init)
        : pm(&pm_init),
          domain(pm_init, min_init, max_init),
          arcsConnectedTo(0),
          constraintNeedsRemovedValues(false),
          queueItem(0)
{
        pm->addVar(this);
        //domain  =  new Ns_BitSet(pm_init, min_init, max_init);
}

NsIntVar::NsIntVar (const Ns_Expression& expr)
{
        expr.post(*this);
        pm->removeLastVar();
        pm->addVar(this);
}

NsIntVar&
NsIntVar::operator = (const Ns_Expression& expr)
{
        assert_Ns( pm == 0                       //&&  domain == 0
                   &&  constraints.empty()
                   &&  constraintNeedsRemovedValues == false
                   //&&  constraintsBoundsCons.empty()  &&  constraintsArcCons.empty()
                   &&  arcsConnectedTo == 0  &&  queueItem == 0 ,
                   "NsIntVar::operator=: Some constraints already imposed on `*this'");
        expr.post(*this);
        pm->removeLastVar();
        pm->addVar(this);
        return  *this;
}

///  Makes the variable transparent to backtracking (store).  Useful for temporary variables.

void
NsIntVar::transparent (void)
{
        domain.lastSaveHistoryId()  =  manager().getCurrentHistoryId();
        manager().removeLastVar();
}

NsIntVarArray::NsIntVarArray (const Ns_ExpressionArray& expr)
        : addedConstraint(false)
{
        expr.post(*this);
}

NsIntVarArray&
NsIntVarArray::operator = (const Ns_ExpressionArray& expr)
{
        assert_Ns( PointArray.empty()  &&  !addedConstraint ,
                   "NsIntVarArray::operator=: Some constraints already imposed on `*this'");
        expr.post(*this);
        return  *this;
}

void
NsIntVarArray::push_front (const NsIntVar& Var)
{
        assert_Ns( !addedConstraint ,  "NsIntVarArray::push_front: Cannot add another variable, because a constraint has been already imposed on the array");
        NsIntVar  *NewVar = new NsIntVar(Var);
        PointArray.push_front( NewVar );
        NewVar->manager().recordIntermediateVar( NewVar );
        NewVar->manager().removeLastVar();
        NewVar->manager().addVar(NewVar);
}

void
NsIntVarArray::push_front (const Ns_Expression& expr)
{
        assert_Ns( !addedConstraint ,  "NsIntVarArray::push_front: Cannot add another variable, because a constraint has been already imposed on the array");
        PointArray.push_front( &expr.post() );
}

void
NsIntVarArray::push_back (const NsIntVar& Var)
{
        assert_Ns( !addedConstraint ,  "NsIntVarArray::push_back: Cannot add another variable, because a constraint has been already imposed on the array");
        NsIntVar  *NewVar = new NsIntVar(Var);
        PointArray.push_back( NewVar );
        NewVar->manager().recordIntermediateVar( NewVar );
        NewVar->manager().removeLastVar();
        NewVar->manager().addVar(NewVar);
}

void
NsIntVarArray::push_back (const Ns_Expression& expr)
{
        assert_Ns( !addedConstraint ,  "NsIntVarArray::push_back: Cannot add another variable, because a constraint has been already imposed on the array");
        PointArray.push_back( &expr.post() );
}
