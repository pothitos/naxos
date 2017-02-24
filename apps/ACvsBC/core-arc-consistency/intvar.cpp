// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace std;
using namespace naxos;

// IntDomain::IntDomain (void)
//{
//	nmw = 0;
//	machw = 0;
//	set_count = 0;
//	min_bit = PLUS_INF;
//	max_bit = MINUS_INF;
//	min_dom = 0;
//	nbits = 0;
//}

IntDomain::IntDomain(const NsInt min_dom_init, const NsInt max_dom_init)
  : min_dom(min_dom_init),
    min_val(min_dom_init),
    max_val(max_dom_init),
    nbits(CorrespondingBit(max_dom_init) + 1),
    set_count(nbits)
{
        assert_exc(NsMINUS_INF < min_dom_init && min_dom_init <= max_dom_init &&
                       max_dom_init < NsPLUS_INF,
                   "IntDomain::IntDomain: Domain out of range");
        // assert_exc(nbits > 0,
        //		"IntDomain::IntDomain: Request to construct an empty
        //domain");
        //  Making `lastSaveId' dirty as the domain has not been saved.
        lastSaveId.id = NsUPLUS_INF;
        //  Initializing `lastSaveId.level' to prevent valgrind from
        //   reporting a warning.
        lastSaveId.level = 0;
}

//	void
// IntDomain::setall (void)
//{
//	assert(nbits>=0  &&  (nbits-1)/MW_BITS+1==machw.size());
//	for (NsInt i=0;  i<machw.size();  ++i)
//		machw[i] = ~0u;
//	// ενεργοποίηση των bit τής τελευταίας λέξης μηχανής
//	if (nbits%MW_BITS != 0)
//		for (unsigned bit=1<<(nbits%MW_BITS);  bit!=0;  bit<<=1)
//			machw[machw.size()-1] &= ~bit;
//	set_count = nbits;
//	min_bit = 0;
//	max_bit = nbits-1;
//}

// IntDomain::IntDomain (const IntDomain& b)
//{
//	nmw = b.nmw;
//	assert(nmw >= 0);
//	if (nmw > 0 )  {
//		machw = new unsigned[nmw];
//		for (NsInt i=0;  i<machw.size();  ++i)
//			machw[i] = b.machw[i];
//	} else  {
//		machw = 0;
//	}
//	set_count = b.set_count;
//	min_bit = b.min_bit;
//	max_bit = b.max_bit;
//}

//	void
// IntDomain::set (const NsInt val, const bool setbit)
//{
//	NsUInt  nbit = CorrespondingBit( val );
//	NsUInt  mw = nbit/MW_BITS;
//	assert_exc(machw.size() != 0,
//			"IntDomain::set: this function is
//Bounds-Consistency-incompatible"); 	assert_exc(min_val <= val && val <=
//max_val && nbit < nbits  &&  mw < machw.size(),
//			"IntDomain::set: Machine word out of `*this' range");
//	unsigned  mwbit = 1<<(nbit%MW_BITS);
//	assert_exc((machw[mw] & mwbit)? !setbit : setbit,
//			"IntDomain::set: Bit to set/clear already set/cleared");
//	if (setbit)  {
//		machw[mw] |= mwbit;
//		++set_count;
//		if (nbit < CorrespondingBit( min_val ))
//			min_val = nbit + min_dom;
//		if (nbit > CorrespondingBit( max_val ))
//			max_val = nbit + min_dom;
//	} else  {
//		machw[mw] &= ~mwbit;
//		--set_count;
//		if (nbit == CorrespondingBit( min_val ))
//			min_val = next(val);
//		if (nbit == CorrespondingBit( max_val ))
//			max_val = previous(val);
//	}
//}

void IntDomain::reset(NsInt val)
{
        assert_exc(min_val <= val && val <= max_val && set_count > 1,
                   "IntDomain::reset: Bad request");
        --set_count;
        if (machw.size() == 0) { // Bounds Consistency
                if (val == min_val) {
                        ++min_val;
                        return;
                } else if (val == max_val) {
                        --max_val;
                        return;
                } else {
                        min_dom = min_val;
                        nbits = CorrespondingBit(max_val) + 1;
                        machw.resize((nbits - 1) / MW_BITS + 1);
                        for (NsUInt i = 0; i < machw.size(); ++i)
                                machw[i] = ~0u;
                        // ενεργοποίηση των bit τής τελευταίας λέξης μηχανής
                        if (nbits % MW_BITS != 0) {
                                for (unsigned bit = 1 << (nbits % MW_BITS);
                                     bit != 0; bit <<= 1)
                                        machw[machw.size() - 1] &= ~bit;
                        }
                        //  continuing on reseting the value...
                }
        }
        NsUInt nbit = CorrespondingBit(val);
        NsUInt mw = nbit / MW_BITS;
        assert_exc(machw.size() != 0, "IntDomain::re-set: this function is "
                                      "Bounds-Consistency-incompatible");
        assert_exc(min_val <= val && val <= max_val && nbit < nbits &&
                       mw < machw.size(),
                   "IntDomain::re-set: Machine word out of `*this' range");
        unsigned mwbit = 1 << (nbit % MW_BITS);
        assert_exc((machw[mw] & mwbit),
                   "IntDomain::re-set: Bit to clear already cleared");
        machw[mw] &= ~mwbit;
        if (val == min_val)
                min_val = next(val);
        if (val == max_val)
                max_val = previous(val);
}

NsInt IntDomain::previous(NsInt val) const
// επιστρέφει τον αριθμό τού επόμενου
// ενεργού bit μετά το nbit
{
        if (val <= min_val)
                return NsMINUS_INF;
        if (val > max_val)
                return max_val;
        if (isContinuous()) // Bounds Consistency
                return (val - 1);
        NsUInt nbit = CorrespondingBit(val) - 1;
        NsInt mw = nbit / MW_BITS;
        unsigned mwbit = 1 << (nbit % MW_BITS);
        assert_exc(nbit < nbits && static_cast<NsUInt>(mw) < machw.size(),
                   "IntDomain::previous: Machine word out of `*this' range");
        if (machw[mw] == 0) { // speedup by means of comparing the whole word
                nbit = mw * MW_BITS - 1;
        } else {
                for (; mwbit != 0; mwbit >>= 1) {
                        if (machw[mw] & mwbit)
                                return (nbit + min_dom);
                        --nbit;
                }
        }
        --mw;
        for (; mw >= 0; --mw) {
                if (machw[mw] ==
                    0) { // speedup by means of comparing the whole word
                        nbit -= MW_BITS;
                } else {
                        for (mwbit = 1 << (MW_BITS - 1); mwbit != 0;
                             mwbit >>= 1) {
                                if (machw[mw] & mwbit)
                                        return (nbit + min_dom);
                                --nbit;
                        }
                }
        }
        throw NsException("IntDomain::previous: Should not reach here");
}

NsInt IntDomain::next(NsInt val) const
// επιστρέφει τον αριθμό τού επόμενου
// ενεργού bit μετά το nbit
{
        if (val >= max_val)
                return NsPLUS_INF;
        if (val < min_val)
                return min_val;
        if (isContinuous()) // Bounds Consistency
                return (val + 1);
        NsUInt nbit = CorrespondingBit(val) + 1;
        NsUInt mw = nbit / MW_BITS;
        unsigned mwbit = 1 << (nbit % MW_BITS);
        assert_exc(nbit < nbits && mw < machw.size(),
                   "IntDomain::next: Machine word out of `*this' range");
        if (machw[mw] == 0) { // speedup by means of comparing the whole word
                nbit = (mw + 1) * MW_BITS;
        } else {
                for (; mwbit != 0; mwbit <<= 1) {
                        if (machw[mw] & mwbit)
                                return (nbit + min_dom);
                        ++nbit;
                }
        }
        ++mw;
        for (; mw < machw.size(); ++mw) {
                if (machw[mw] ==
                    0) { // speedup by means of comparing the whole word
                        nbit += MW_BITS;
                } else {
                        for (mwbit = 1; mwbit != 0; mwbit <<= 1) {
                                if (machw[mw] & mwbit)
                                        return (nbit + min_dom);
                                ++nbit;
                        }
                }
        }
        throw NsException("IntDomain::next: Should not reach here");
}

NsInt IntDomain::next_gap(NsInt val) const
// επιστρέφει τον αριθμό τού επόμενου
// inactive bit μετά το nbit
{
        // || Bounds Consistency
        if (val >= max_val - 1 || isContinuous())
                return NsPLUS_INF;
        NsUInt nbit;
        if (val < min_val)
                nbit = CorrespondingBit(min_val) + 1;
        else
                nbit = CorrespondingBit(val) + 1;
        NsUInt mw = nbit / MW_BITS;
        unsigned mwbit = 1 << (nbit % MW_BITS);
        NsUInt maxbit = CorrespondingBit(max_val);
        assert_exc(maxbit < nbits && nbit < nbits && mw < machw.size(),
                   "IntDomain::next_gap: Machine word out of `*this' range");
        if (machw[mw] == ~0u) { // speedup by means of comparing the whole word
                nbit = (mw + 1) * MW_BITS;
        } else {
                for (; mwbit != 0 && nbit <= maxbit; mwbit <<= 1) {
                        if (!(machw[mw] & mwbit))
                                return (nbit + min_dom);
                        ++nbit;
                }
        }
        ++mw;
        for (; mw < machw.size(); ++mw) {
                if (machw[mw] ==
                    ~0u) { // speedup by means of comparing the whole word
                        nbit += MW_BITS;
                } else {
                        for (mwbit = 1; mwbit != 0 && nbit <= maxbit;
                             mwbit <<= 1) {
                                if (!(machw[mw] & mwbit))
                                        return (nbit + min_dom);
                                ++nbit;
                        }
                }
        }
        return NsPLUS_INF;
}

//	IntDomain&
// IntDomain::operator  &= (const IntDomain& b)
//{
//	assert(machw.size() == b.machw.size());
//	NsInt  i, bit;
//	for (i=0;  i<machw.size();  ++i)
//		machw[i] &= b.machw[i];
//	set_count = 0;
//	for (i=0;  i<machw.size();  ++i)
//		for (bit=1;  bit!=0;  bit<<=1)
//			if (machw[i] & bit)
//				++set_count;
//	return  (*this);
//}
//
//
//	IntDomain
// operator  & (IntDomain& a, IntDomain& b)
//{
//	IntDomain result = a;
//	result &= b;
//	return  result;
//}

bool IntDomain::contains(const NsInt val) const
{
        if (val < min_val || val > max_val)
                return false;
        if (isContinuous()) // Bounds Consistency
                return true;
        NsUInt nbit = CorrespondingBit(val);
        NsUInt mw = nbit / MW_BITS;
        assert_exc(nbit < nbits && mw < machw.size(),
                   "IntDomain::contains: Machine word out of `*this' range");
        unsigned mwbit = 1 << (nbit % MW_BITS);
        return (machw[mw] & mwbit);
}

//	istream&
// naxos::operator >> (istream& i, IntDomain& bp)
//{
//	i >> bp.machw.size();
//	assert(bp.machw.size() >= 0);
//	if(bp.nmw > 0)  {
//		bp.machw = new unsigned[bp.nmw];
//		for(NsInt j=0;  j<bp.machw.size();  ++j)
//			i >> hex >> bp.machw[j];
//		i >> dec >> bp.set_count;
//	} else  {
//		bp.machw = 0;
//		bp.set_count = 0;
//	}
//
//	return  i;
//}

ostream& naxos::operator<<(ostream& os, const IntDomain& dom)
{
        // o << bp.machw.size();
        ////assert(bp.machw.size() >= 0);
        // if (bp.machw.size() > 0)  {
        //	for (NsUInt j=0;  j<bp.machw.size();  ++j)
        //		o << ' ' << hex << setw(bp.MW_BITS/4) << setfill('0') <<
        //bp.machw[j]; 	o << ' ' << dec << bp.set_count;
        //}
        // return  o;
        os << "[";
        NsInt val, gap;
        gap = dom.min_val - 1;
        while ((val = dom.next(gap)) != NsPLUS_INF) {
                os << val;
                gap = dom.next_gap(val);
                if (gap == NsPLUS_INF) {
                        if (dom.max_val != val)
                                os << ".." << dom.max_val;
                        break;
                }
                if (gap != val + 1)
                        os << ".." << gap - 1 << " ";
                else
                        os << " ";
        }
        // cout << "-";
        // if (dom.isContinuous())  {
        //	//  Bounds Consistency  //
        //	if (dom.min_val == dom.max_val)
        //		os << dom.min_val;
        //	else
        //		os << dom.min_val << ".." << dom.max_val;
        //
        //} else  {
        //	NsInt  previous=NsMINUS_INF;  // initialized to suppress
        //warnings 	NsIntVar::const_iterator  v = dom.begin(); 	if (v !=
        //dom.end())  { 		os << *v; 		previous = *v;
        //		++v;
        //	}
        //	while (v != dom.end())  {
        //		if (*v != previous + 1)  {
        //			os << " " << *v;
        //			previous = *v;
        //			++v;
        //		} else  {
        //			do {
        //				previous = *v;
        //				++v;
        //			} while (v != dom.end()   &&   *v == previous +
        //1); 			os << ".." << previous;
        //		}
        //	}
        //}
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
        for (; V != VarArr.end(); ++V)
                os << " " << *V;
        os << "]";
        return os;
}

void NsIntVar::removeAll(void)
{
        pm->FoundAnInconsistency();
}

void NsIntVar::remove(const NsInt v)
{
        if (!contains(v))
                return;
        removeSingle(v, 0);
}

bool NsIntVar::removeRange(NsInt vstart, NsInt vend, const NsConstraint* constr)
{
        if (vstart <= min() && vend >= max()) {
                pm->FoundAnInconsistency();
                return false;
        }
        if (vstart != NsMINUS_INF)
                --vstart;
        if (vend != NsPLUS_INF)
                ++vend;
        if (vend > max()) {
                // Bounds-Consistency compatibility:  When vend > max()
                //  we should remove values from right to left.
                while ((vend = previous(vend)) > vstart)
                        assert_exc(
                            removeSingle(vend, constr),
                            "NsIntVar::removeRange: Problem removing `vend'");
        } else {
                while ((vstart = next(vstart)) < vend)
                        assert_exc(
                            removeSingle(vstart, constr),
                            "NsIntVar::removeRange: Problem removing `vstart'");
        }
        return true;
}

bool NsIntVar::removeSingle(const NsInt v, const NsConstraint* constr)
{
        if (size() == 1) {
                pm->FoundAnInconsistency();
                return false;
        }
        //  The domain is going to be changed.
        //  Saving the domain for future backtracking purposes.
        if (!pm->isCurrentHistoryId(dom.lastSaveHistoryId())) {
                pm->getCurrDomsStore().DomsOrig.push(VarDom_t(this, dom));
                dom.lastSaveHistoryId() = pm->getCurrentHistoryId();
        }
        // pm->saveBitsetDomain(*this);
        ////save(pm.getCurrDomsStore());
        // if ( pm->getCurrDomsStore().AlreadySaved.count( (pointer_t)this )  ==
        // 0 )   { 	pm->getCurrDomsStore().AlreadySaved.insert(
        //(pointer_t)this ); 	pm->getCurrDomsStore().DomsOrig.push( VarDom_t(
        //this , dom ) );
        //}
        dom.reset(v);
        pm->getQ().push(QueueItem(this, constr, v));
        return true;
}

//	bool
// NsIntVar::remove_nosave (const NsInt val)
//{
//	//save(pm.getPrevDomsStore());
//
//	if (count() == 1)  {
//		pm.FoundAnInconsistency();
//		return  false;
//	}
//	dom.reset(val);
//
//	pm.getQ().push( QueueItem(this, 0, val) );
//	return  true;
//}

void NsIntVar::set(const NsInt v_set)
{
        // assert_exc( contains(v_set),  "NsIntVar::set: `*this' does not
        // contain the value to set");  assert_exc( !isBound(),  "NsIntVar::set:
        // `*this' is already bound");  for (const_iterator v = begin();   v !=
        // end();   ++v)  { 	if (*v != v_set) 		assert_exc(
        //remove( *v, 0 ),  "NsIntVar::set: Failed to remove a value in the
        //domain");
        //}
        //  Bounds Consistency compatible set
        // while ( min() < v_set )
        //	assert_exc( removeSingle( min(), 0 ),  "NsIntVar::set: Failed to
        //remove a value in the domain");  while ( max() > v_set )
        //	assert_exc( removeSingle( max(), 0 ),  "NsIntVar::set: Failed to
        //remove a value in the domain");
        removeRange(NsMINUS_INF, v_set - 1, 0);
        removeRange(v_set + 1, NsPLUS_INF, 0);
}

//	void
// NsIntVar::set_save (const NsInt v_set)
//{
//	save(pm.getPrevDomsStore());
//
//	//assert_exc( !isBound(),  "NsIntVar::set: `*this' is already bound");
//	for (const_iterator v = begin();  v != end();  ++v)  {
//		if (*v != v_set)
//			assert_exc( remove(*v, 0),  "NsIntVar::set: Failed to remove
//a value in the domain");
//	}
//}

// IntVarArray::IntVarArray (int size, const NsInt min_dom, const NsInt max_dom)
//{
//	NsIntVar  TempVar(min_dom, max_dom);
//	while ( size-- > 0 )
//		Var.push_back(TempVar);
//}

void NsIntVar::addConstraint(NsConstraint* constr)
{
        CONSTR.push_back(constr);
        assert_exc(constr->VarsInvolvedIn() >= 1, "NsIntVar::addConstraint: "
                                                  "Wrong VarsInvolvedIn "
                                                  "constraint `constr'");
        ArcsConnectedTo += constr->VarsInvolvedIn() - 1;
}

NsIntVar::NsIntVar(NsProblemManager& pm_init, const NsInt min_dom_init,
                   const NsInt max_dom_init)
  : pm(&pm_init), dom(min_dom_init, max_dom_init), ArcsConnectedTo(0)
{
        pm->addVar();
}

// NsIntVar::NsIntVar (ExprMessage mess)
//	: ArcsConnectedTo(0)
//{
//	mess.Expr->post(this);
//	delete  mess.Expr;
//}

NsIntVar::NsIntVar(const Expression& expr) : ArcsConnectedTo(0) //, id(0)
{
        assert_exc(CONSTR.size() == 0 && ArcsConnectedTo == 0,
                   "NsIntVar::NsIntVar(expr): Some constraints already imposed "
                   "on `this'");
        expr.post(*this);
}

NsIntVar& NsIntVar::operator=(const Expression& expr)
{
        assert_exc(
            pm == 0 && CONSTR.size() == 0 && ArcsConnectedTo == 0,
            "NsIntVar::operator=: Some constraints already imposed on `this'");
        expr.post(*this);
        return *this;
}

//	void
// NsIntVar::setID (void)
//{
//	assert_exc(id==0,  "NsIntVar::setID: `id' already set");
//	id = pm->generateID();
//}

// NsIntVarArray::~NsIntVarArray (void)
//{
//	for (PointArray_t::iterator  p = PointArray.begin();
//			p != PointArray.end();  ++p)
//	{
//		if (p->WasAllocated)
//			delete  p->Point;
//	}
//}

NsIntVarArray::NsIntVarArray(const ExpressionArray& expr)
  : added_constraint(false)
{
        expr.post(*this);
}

NsIntVarArray& NsIntVarArray::operator=(const ExpressionArray& expr)
{
        assert_exc(PointArray.size() == 0 && !added_constraint,
                   "NsIntVarArray::operator=: Some constraints already imposed "
                   "on `this'");
        expr.post(*this);
        return *this;
}

//	void
// NsIntVarArray::push_back (ExprMessage mess)
//{
//	assert_exc( !added_constraint, "NsIntVarArray::push_back: Cannot add
//another variable, because a constraint has been already imposed on the
//array"); 	PointArray.push_back( VarPointAllocated( &mess.Expr->post(),
//true ) ); 	delete  mess.Expr;
//}

void NsIntVarArray::push_front(const NsIntVar& Var)
{
        assert_exc(!added_constraint, "NsIntVarArray::push_front: Cannot add "
                                      "another variable, because a constraint "
                                      "has been already imposed on the array");
        NsIntVar* NewVar = new NsIntVar(Var);
        PointArray.push_front(NewVar);
        NewVar->manager().RecordIntermediateVar(NewVar);
}

void NsIntVarArray::push_front(const Expression& expr)
{
        assert_exc(!added_constraint, "NsIntVarArray::push_front: Cannot add "
                                      "another variable, because a constraint "
                                      "has been already imposed on the array");
        PointArray.push_front(&expr.post());
}

void NsIntVarArray::push_back(const NsIntVar& Var)
{
        assert_exc(!added_constraint, "NsIntVarArray::push_back: Cannot add "
                                      "another variable, because a constraint "
                                      "has been already imposed on the array");
        NsIntVar* NewVar = new NsIntVar(Var);
        PointArray.push_back(NewVar);
        NewVar->manager().RecordIntermediateVar(NewVar);
}

void NsIntVarArray::push_back(const Expression& expr)
{
        assert_exc(!added_constraint, "NsIntVarArray::push_back: Cannot add "
                                      "another variable, because a constraint "
                                      "has been already imposed on the array");
        PointArray.push_back(&expr.post());
}
