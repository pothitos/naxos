// Part of https://github.com/pothitos/naxos

#ifndef Ns_NAXOS_H
#define Ns_NAXOS_H

#include <climits>
#include <ctime>
#include <deque>
#include <iostream>

#ifdef __GNUC__
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
#define Ns_OLD_UNORDERED_SET
#endif
#endif // __GNUC__

#ifdef Ns_OLD_UNORDERED_SET
#include <ext/hash_map>
#include <ext/hash_set>
#define Ns_UNORDERED_SET __gnu_cxx::hash_set
#define Ns_UNORDERED_MAP __gnu_cxx::hash_map
#else
#include <unordered_map>
#include <unordered_set>
#define Ns_UNORDERED_SET std::unordered_set
#define Ns_UNORDERED_MAP std::unordered_map
#endif

#include <fstream>
#include <stdexcept>
#include <string>

namespace naxos {

class NsException : public std::logic_error {

    public:
        NsException(const std::string& error_message)
          : logic_error("Naxos: " + error_message)
        {
        }
};

/* Definitions of Naxos Solver types and their limits */

typedef long NsInt;
typedef unsigned long NsUInt;

const NsInt NsMINUS_INF = LONG_MIN;
const NsInt NsPLUS_INF = LONG_MAX;
const NsUInt NsUPLUS_INF = ULONG_MAX;

class NsIntVar;

template <class TemplType>
class NsDeque;

/* Abstract class to represent an (algebraic etc) expression consisting of
 * constrained variables */

class Expression {

    public:
        virtual void post(NsIntVar& VarX) const = 0;
        //  posts the constraint `VarX == *this'
        //  (`*this' represents an Expression)

        virtual NsIntVar& post(void) const = 0;
        //  produces/returns a variable to represent the Expression (`*this')

        //{  throw  NsException("Expression::post: this is an \"abstract\"
        //function");  }

        // The destructor of an abstract class should be virtual
        virtual ~Expression(void)
        {
        }
};

class Ns_ExprElement : public Expression {

    private:
        NsIntVar& VarIndex;
        const NsDeque<NsInt>& intArray;
        // NsIntVar&  VarValue;

    public:
        Ns_ExprElement(
            NsIntVar& VarIndex_init, const NsDeque<NsInt>& intArray_init
            /*,
                                                    NsIntVar& VarValue_init*/
            )
          : VarIndex(VarIndex_init), intArray(intArray_init)
        /*,
                                VarValue(VarValue_init)*/
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;

        // virtual Ns_Constraint*  postConstraint (bool positively)  const;

        // virtual void       postC (NsIntVar &VarX, bool positively)  const
        //{
        //	VarX.size();        // to suppress warnings
        //	positively = true;  // to suppress warnings
        //	throw  NsException("Ns_ExprConstrElement::postC: NsElement
        //cannot be used as a meta-constraint");
        //}
        // virtual NsIntVar&  postC (bool positively)  const
        //{
        //	positively = true;  // to suppress warnings
        //	throw  NsException("Ns_ExprConstrElement::postC: NsElement
        //cannot be used as a meta-constraint");
        //}
};

/*
 *  The following class is a safer version of `deque' that throws an
 *   exception when an `out_of_range' request is being made.  (It takes
 *   advantage of the internal `at()' method that does this work.)
 */

template <class TemplType>
class NsDeque : public std::deque<TemplType> {

    public:
        NsDeque(void)
        {
        }

        NsDeque(const typename std::deque<TemplType>::size_type n)
          : std::deque<TemplType>(n)
        {
        }

        typename std::deque<TemplType>::reference
        operator[](const typename std::deque<TemplType>::size_type i)
        {
                return this->at(i);
        }

        typename std::deque<TemplType>::const_reference
        operator[](const typename std::deque<TemplType>::size_type i) const
        {
                return this->at(i);
        }

        Ns_ExprElement operator[](NsIntVar& VarIndex) const
        {
                return Ns_ExprElement(VarIndex, *this);
        }

        Ns_ExprElement operator[](const Expression& VarIndexExpr) const
        {
                return (*this)[VarIndexExpr.post()];
        }

        typename std::deque<TemplType>::reference front(void)
        {
                if (std::deque<TemplType>::size() == 0)
                        throw std::out_of_range("NsDeque::front: Empty deque");
                return std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::const_reference front(void) const
        {
                if (std::deque<TemplType>::size() == 0)
                        throw std::out_of_range("NsDeque::front: Empty deque");
                return std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::reference back(void)
        {
                if (std::deque<TemplType>::size() == 0)
                        throw std::out_of_range("NsDeque::back: Empty deque");
                return std::deque<TemplType>::back();
        }

        typename std::deque<TemplType>::const_reference back(void) const
        {
                if (std::deque<TemplType>::size() == 0)
                        throw std::out_of_range("NsDeque::back: Empty deque");
                return std::deque<TemplType>::back();
        }
};

inline void assert_exc(const bool test, const char* error_message)
//  Here we used the type `char*' for `error_message',
//   instead of `string', plainly for time-performance reasons.
{
        if (!test)
                throw NsException(error_message);
}

inline void assert_Ns(const bool test, const char* error_message)
{
        assert_exc(test, error_message);
}

#include "queue.h"
#include "stack.h"

typedef unsigned long pointer_t;

class NsIntVar;
typedef NsDeque<NsIntVar*> PointArray_t;
typedef PointArray_t::size_type NsIndex;

///  The Ns_HistoryId_t of an object can be used to see whether it is valid.

///  Each frame of the Ns_StackSearch NsProblemManager::searchNodes is
///   represented by its \a level.  Each \a level has its own \a id that
///   stops being valid when the frame is popped.
///   \internal

struct Ns_HistoryId_t {

        ///  The depth of the node in the search tree.
        NsIndex level;

        ///  The identity of the search node.
        NsUInt id;
};
/*
 *  Class describing the domain of a constrained variable
 *
 *   A bitset is used to hold its values.  If the i-th bit is active,
 *    then the domain contains the value `min_dom + i'.
 */

class IntDomain {

    private:
        NsInt min_dom; // It is the initial minimum value of the domain

        NsInt min_val; // Minimum value of the domain
        NsInt max_val; // Maximum value of the domain

        NsUInt nbits; // Number of the bits (active or inactive) of the bitset
        NsUInt set_count; // Number of the active bits (values) of the domain

        // An array consisting of machine words.
        NsDeque<unsigned> machw;
        //  It contains the bits for the bitset.

        static const NsInt MW_BITS = CHAR_BIT * sizeof(unsigned);
        // The number of bits that a machine word can hold

        ///  The `timestamp' that can be used in chronological backtracking.
        Ns_HistoryId_t lastSaveId;

    public:
        ///  Returns the `lastSaveId'.
        Ns_HistoryId_t& lastSaveHistoryId(void)
        {
                return lastSaveId;
        }

    private:
        //  The following method performs a simple subtraction, to find
        //   out the corresponding bit number for the value `val'.
        //   However, it takes care about overflow issues, because the
        //   limit for a bit number is NsUPLUS_INF and not NsPLUS_INF!

        NsUInt CorrespondingBit(NsInt val) const
        {
                // to avoid an overflow
                if (min_dom < 0 && val > 0)
                        return (val + static_cast<NsUInt>(-min_dom));
                return (val - min_dom);
        }

    public:
        //  It returns `true' if the domain is continuous

        bool isContinuous(void) const
        {
                NsUInt diff;
                // to avoid an overflow
                if (min_val < 0 && max_val > 0)
                        diff = max_val + static_cast<NsUInt>(-min_val);
                else
                        diff = max_val - min_val;
                return (diff + 1 == set_count);
        }

        //  Declaration of an iterator to iterate through all the values
        //   of the domain, without changing them (const_iterator).

        friend class const_iterator;

        class const_iterator {

            private:
                const IntDomain* IntDom;
                NsInt curr_val;

            public:
                const_iterator(void) : IntDom(0)
                {
                }

                const_iterator(const IntDomain& IntDom_init)
                  : IntDom(&IntDom_init), curr_val(IntDom->min_val)
                {
                }

                bool operator==(const const_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::==:"
                                                " Uninitialized `*this'");
                        return (curr_val == b.curr_val);
                }

                bool operator!=(const const_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::!=:"
                                                " Uninitialized `*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::*: "
                                                "Uninitialized `*this'");
                        assert_exc(curr_val != NsPLUS_INF,
                                   "IntDomain::const_iterator::*: Bad request "
                                   "`*(something.end())'");
                        return curr_val;
                }

                const_iterator& end(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::"
                                                "end: Uninitialized `*this'");
                        curr_val = NsPLUS_INF;
                        return *this;
                }

                const_iterator& operator++(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::++:"
                                                " Uninitialized `*this'");
                        curr_val = IntDom->next(curr_val);
                        return *this;
                }

                const_iterator& operator--(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_iterator::--:"
                                                " Uninitialized `*this'");
                        curr_val = IntDom->previous(curr_val);
                        return *this;
                }
        };

        const const_iterator begin(void) const
        {
                return const_iterator(*this);
        }

        const const_iterator end(void) const
        {
                const_iterator iter_end(*this);
                return iter_end.end();
        }

        //  Declaration of an iterator to iterate through all the values
        //   of the domain, without changing them, in reverse order
        //   (const_iterator).

        friend class const_reverse_iterator;

        class const_reverse_iterator {

            private:
                const IntDomain* IntDom;
                NsInt curr_val;

            public:
                const_reverse_iterator(void) : IntDom(0)
                {
                }

                const_reverse_iterator(const IntDomain& IntDom_init)
                  : IntDom(&IntDom_init), curr_val(IntDom->max_val)
                {
                }

                bool operator==(const const_reverse_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::==: Uninitialized "
                                                "`*this'");
                        return (curr_val == b.curr_val);
                }

                bool operator!=(const const_reverse_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::!=: Uninitialized "
                                                "`*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::*: Uninitialized "
                                                "`*this'");
                        assert_exc(curr_val != NsPLUS_INF,
                                   "IntDomain::const_reverse_iterator::*: Bad "
                                   "request `*(something.end())'");
                        return curr_val;
                }

                const_reverse_iterator& end(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::end: Uninitialized "
                                                "`*this'");
                        curr_val = NsMINUS_INF;
                        return *this;
                }

                const_reverse_iterator& operator++(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::++: Uninitialized "
                                                "`*this'");
                        curr_val = IntDom->previous(curr_val);
                        return *this;
                }

                const_reverse_iterator& operator--(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_reverse_"
                                                "iterator::--: Uninitialized "
                                                "`*this'");
                        curr_val = IntDom->next(curr_val);
                        return *this;
                }
        };

        const const_reverse_iterator rbegin(void) const
        {
                return const_reverse_iterator(*this);
        }

        const const_reverse_iterator rend(void) const
        {
                const_reverse_iterator iter_end(*this);
                return iter_end.end();
        }

        //  Declaration of an iterator to iterate through all the missing values
        //  (gaps)
        //   of the domain [min_val .. max_val], without changing them
        //   (const_iterator).

        friend class const_gap_iterator;

        class const_gap_iterator {

            private:
                const IntDomain* IntDom;
                NsInt curr_gap;

            public:
                const_gap_iterator(void) : IntDom(0)
                {
                }

                const_gap_iterator(const IntDomain& IntDom_init)
                  : IntDom(&IntDom_init),
                    curr_gap(IntDom->next_gap(IntDom->min_val))
                {
                }

                bool operator==(const const_gap_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_gap_iterator:"
                                                ":==: Uninitialized `*this'");
                        return (curr_gap == b.curr_gap);
                }

                bool operator!=(const const_gap_iterator& b) const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_gap_iterator:"
                                                ":!=: Uninitialized `*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_exc(IntDom != 0, "IntDomain::const_gap_iterator:"
                                                ":*: Uninitialized `*this'");
                        assert_exc(curr_gap != NsPLUS_INF,
                                   "IntDomain::const_gap_iterator::*: Bad "
                                   "request `*(something.end())'");
                        return curr_gap;
                }

                const_gap_iterator& end(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_gap_iterator:"
                                                ":end: Uninitialized `*this'");
                        curr_gap = NsPLUS_INF;
                        return *this;
                }

                const_gap_iterator& operator++(void)
                {
                        assert_exc(IntDom != 0, "IntDomain::const_gap_iterator:"
                                                ":++: Uninitialized `*this'");
                        curr_gap = IntDom->next_gap(curr_gap);
                        return *this;
                }
        };

        const const_gap_iterator gap_begin(void) const
        {
                return const_gap_iterator(*this);
        }

        const const_gap_iterator gap_end(void) const
        {
                const_gap_iterator iter_end(*this);
                return iter_end.end();
        }

        IntDomain(const NsInt min_dom_init, const NsInt max_dom_init);

        IntDomain(void){};

        // IntDomain (const IntDomain& b);

        // Removes a value from the domain
        void reset(NsInt val);

        NsUInt size(void) const // It returns the domain size
        {
                return set_count;
        }

        // It returns the biggest value in the domain that is less than `to_val'
        NsInt previous(NsInt to_val) const;

        // It returns the smaller value in the domain that is greater than
        // `to_val'
        NsInt next(NsInt to_val) const;

        // It returns the smaller value *not* in the domain that is greater than
        // `to_val'
        NsInt next_gap(NsInt to_val) const;

        NsInt max(void) const
        {
                return max_val;
        }

        NsInt min(void) const
        {
                return min_val;
        }

        bool operator[](const NsInt val) const
        {
                return contains(val);
        }

        bool contains(const NsInt val) const;

        //  The following method returns `true' if the domain is represented
        //  only
        //   by its lower and upper limit (this is the case when it is
        //   continuous).
        bool boundsConsistent(void) const
        {
                return (machw.size() == 0);
        }

        friend std::ostream& operator<<(std::ostream& os, const IntDomain& dom);
};

class NsConstraint;
typedef NsDeque<NsConstraint*> CONSTR_arr_t;

class Expression;
class NsProblemManager;

/* The following class represents a constrained variable */

class NsIntVar {

    private:
        // Each constrained variable belongs to a specific "problem manager"
        NsProblemManager* pm;

        IntDomain dom; // The domain of the constrained variable

    public:
        void setDomain(const IntDomain& domain)
        {
                dom = domain;
        }

        //	const IntDomain&
        // getDomain (void)  const
        //{
        //	return  dom;
        //}

        typedef IntDomain::const_iterator const_iterator;

        const const_iterator begin(void) const
        {
                return const_iterator(dom);
        }

        const const_iterator end(void) const
        {
                const_iterator iter_end(dom);
                return iter_end.end();
        }

        typedef IntDomain::const_reverse_iterator const_reverse_iterator;

        const const_reverse_iterator rbegin(void) const
        {
                return const_reverse_iterator(dom);
        }

        const const_reverse_iterator rend(void) const
        {
                const_reverse_iterator iter_end(dom);
                return iter_end.end();
        }

        typedef IntDomain::const_gap_iterator const_gap_iterator;

        const const_gap_iterator gap_begin(void) const
        {
                return const_gap_iterator(dom);
        }

        const const_gap_iterator gap_end(void) const
        {
                const_gap_iterator iter_end(dom);
                return iter_end.end();
        }

        NsIntVar(void)
          /*, id(0)*/
          : pm(0), ArcsConnectedTo(0)
        {
        }

        NsIntVar(NsProblemManager& pm_init, const NsInt min_dom_init,
                 const NsInt max_dom_init);

        NsIntVar(const Expression& expr);

        NsIntVar& operator=(const Expression& expr);

        /* Methods that remove values from the domain follow */

        // To remove in purpose all the values of the
        void removeAll(void);
        //  domain, thus making an inconsistency

        // bool  remove_nosave (const NsInt val);

        void remove(const NsInt val);

        bool removeSingle(const NsInt val, const NsConstraint* c);
        // To remove a value, plus recording the constraint that
        //  made this removal.  If c==0 no constraint is recorded

        void remove(const NsInt val_start, const NsInt val_end)
        {
                removeRange(val_start, val_end);
        }

        bool removeRange(NsInt val_start, NsInt val_end,
                         const NsConstraint* c = 0);

        // To assign a value to the constrained variable...
        void set(const NsInt v_set);

        // void  set_save (const NsInt v_set);
        // void  setall (void);

        NsUInt size(void) const
        {
                return dom.size();
        }

        NsInt previous(const NsInt to_val) const
        {
                return dom.previous(to_val);
        }

        NsInt next(const NsInt to_val) const
        {
                return dom.next(to_val);
        }

        NsInt min(void) const
        {
                return dom.min();
        }

        NsInt max(void) const
        {
                return dom.max();
        }

        // If the constrained variable is instantiated, the following returns
        // its value
        NsInt value(void) const
        {
                assert_exc(isBound(),
                           "NsIntVar::value: `*this': Not a bound NsIntVar");
                return dom.min();
        }

        bool isBound(void) const
        {
                return (dom.size() == 1);
        }

        bool operator[](const NsInt val) const
        {
                return dom[val];
        }

        bool contains(const NsInt val) const
        {
                return dom.contains(val);
        }

        bool boundsConsistent(void) const
        {
                return dom.boundsConsistent();
        }

        // NsIntVar&  operator  &= (const NsIntVar& b);

        // friend istream&  operator >> (istream& i, NsIntVar& bp);
        friend std::ostream& operator<<(std::ostream& os, const NsIntVar& Var);

    public:
        CONSTR_arr_t CONSTR;

    private:
        int ArcsConnectedTo;

    public:
        void addConstraint(NsConstraint* c);

        // int  varsConnected (void)  const
        //{
        //	return  ArcsConnectedTo;
        //}

        const CONSTR_arr_t::const_iterator CONSTRbegin(void) const
        {
                return CONSTR.begin();
        }

        const CONSTR_arr_t::const_iterator CONSTRend(void) const
        {
                return CONSTR.end();
        }

        NsProblemManager& manager(void) const
        {
                return *pm;
        }

        // private:
        //	NsIndex  id;

        // public:
        //	void  setID (void);

        //		NsIndex
        //	getID (void)  const
        //	{
        //		assert_exc(id != 0,  "NsIntVar::getID: `id' not set");
        //		return  id;
        //	}

        //		bool
        //	IsSetID (void)  const
        //	{
        //		return  (id != 0);
        //	}
};

inline std::ostream& operator<<(std::ostream& os, const NsIntVar& Var)
{
        return (os << Var.dom);
}

class ExpressionArray;

/* Class describing a flexible array data type, to hold constrained variables
 * (`NsIntVar's) */

class NsIntVarArray {

    private:
        // class  VarPointAllocated  {
        //	public:
        //		NsIntVar  *Point;
        //		bool  WasAllocated;
        //		VarPointAllocated (NsIntVar *Point_init, bool Allocated)
        //			: Point(Point_init), WasAllocated(Allocated)	{
        //}
        //};

        PointArray_t PointArray;

        bool added_constraint;

    public:
        NsIntVarArray(void) : added_constraint(false)
        {
        }

        // NsIntVarArray (const NsIndex length) : PointArray(length),
        // added_constraint(false) 	{    }

        //~NsIntVarArray (void);

        NsIntVarArray(const ExpressionArray& expr);

        NsIntVarArray& operator=(const ExpressionArray& expr);

        void addConstraint(void)
        {
                added_constraint = true;
        }

        // void  resize (const NsIndex length)
        //{
        //	assert_exc( empty()  &&  !added_constraint,
        //"NsIntVarArray::push_front: Cannot resize, because it is not empty");
        //	PointArray.resize(length);
        //}

        void push_front(const NsIntVar& Var);

        // private:
        void push_front(NsIntVar& Var)
        {
                assert_exc(!added_constraint, "NsIntVarArray::push_front: "
                                              "Cannot add another variable, "
                                              "because a constraint has been "
                                              "already imposed on the array");
                PointArray.push_front(&Var);
        }

        void push_front(const Expression& expr);

        void push_back(const NsIntVar& Var);

        // private:
        void push_back(NsIntVar& Var)
        {
                assert_exc(!added_constraint, "NsIntVarArray::push_back: "
                                              "Cannot add another variable, "
                                              "because a constraint has been "
                                              "already imposed on the array");
                PointArray.push_back(&Var);
        }

        // public:
        // void  push_back (VarPointAllocated& VPA)
        //{
        //	assert_exc( !added_constraint, "NsIntVarArray::push_back: Cannot
        //add another variable, because a constraint has been already imposed on
        //the array"); 	PointArray.push_back( VPA );
        //}

        void push_back(const Expression& expr);

        //  Declaration of an iterator to iterate through
        //   the constrained variables of the array.

        class const_iterator;

        // friend class iterator;

        class iterator {

            private:
                PointArray_t* PointArr;

                PointArray_t::iterator it;

            public:
                friend class const_iterator;

                iterator(void) : PointArr(0)
                {
                }

                iterator(PointArray_t& PointArr_init)
                  : PointArr(&PointArr_init), it(PointArr->begin())
                {
                }

                bool operator==(const iterator& b) const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::iterator::==:"
                                                  " Uninitialized `*this'");
                        return (it == b.it);
                }

                bool operator!=(const iterator& b) const
                {
                        return !(*this == b);
                }

                NsIntVar& operator*(void)const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::iterator::*: "
                                                  "Uninitialized `*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::iterator::*: Bad request "
                                   "`*(something.end())'");
                        return **it;
                }

                NsIntVar* operator->(void)const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::iterator::*: "
                                                  "Uninitialized `*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::iterator::*: Bad request "
                                   "`*(something.end())'");
                        return *it;
                }

                iterator& end(void)
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::iterator::"
                                                  "end: Uninitialized `*this'");
                        it = PointArr->end();
                        return *this;
                }

                iterator& operator++(void)
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::iterator::++:"
                                                  " Uninitialized `*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::iterator::end: Bad request "
                                   "`++(something.end())'");
                        ++it;
                        return *this;
                }
        };

        iterator begin(void)
        {
                return iterator(PointArray);
        }

        iterator end(void)
        {
                iterator iter_end(PointArray);
                return iter_end.end();
        }

        class const_iterator {

            private:
                const PointArray_t* PointArr;

                PointArray_t::const_iterator it;

            public:
                const_iterator(void) : PointArr(0)
                {
                }

                const_iterator(const PointArray_t& PointArr_init)
                  : PointArr(&PointArr_init), it(PointArr->begin())
                {
                }

                const_iterator(const iterator iter)
                  : PointArr(iter.PointArr), it(iter.it)
                {
                }

                const_iterator& operator=(const iterator iter)
                {
                        PointArr = iter.PointArr;
                        it = iter.it;
                        return *this;
                }

                bool operator==(const const_iterator& b) const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::const_"
                                                  "iterator::==: Uninitialized "
                                                  "`*this'");
                        return (it == b.it);
                }

                bool operator!=(const const_iterator& b) const
                {
                        return !(*this == b);
                }

                const NsIntVar& operator*(void)const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::const_"
                                                  "iterator::*: Uninitialized "
                                                  "`*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::const_iterator::*: Bad "
                                   "request `*(something.end())'");
                        return **it;
                }

                const NsIntVar* operator->(void)const
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::const_"
                                                  "iterator::*: Uninitialized "
                                                  "`*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::const_iterator::*: Bad "
                                   "request `*(something.end())'");
                        return *it;
                }

                const_iterator& end(void)
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::const_"
                                                  "iterator::end: "
                                                  "Uninitialized `*this'");
                        it = PointArr->end();
                        return *this;
                }

                const_iterator& operator++(void)
                {
                        assert_exc(PointArr != 0, "NsIntVarArray::const_"
                                                  "iterator::++: Uninitialized "
                                                  "`*this'");
                        assert_exc(it != PointArr->end(),
                                   "NsIntVarArray::const_iterator::end: Bad "
                                   "request `++(something.end())'");
                        ++it;
                        return *this;
                }
        };

        const_iterator begin(void) const
        {
                return const_iterator(PointArray);
        }

        const_iterator end(void) const
        {
                const_iterator iter_end(PointArray);
                return iter_end.end();
        }

        NsIntVar& operator[](const NsIndex i)
        {
                return *PointArray[i];
        }

        const NsIntVar& operator[](const NsIndex i) const
        {
                return *PointArray[i];
        }

        NsIntVar& front(void)
        {
                return *PointArray.front();
        }

        NsIntVar& back(void)
        {
                return *PointArray.back();
        }

        NsIndex size(void) const
        {
                return PointArray.size();
        }

        bool empty(void) const
        {
                return PointArray.empty();
        }
};

std::ostream& operator<<(std::ostream& os, const NsIntVarArray& VarArr);

// class  NsIntVarArray  {
//	private:
//		std::deque<NsIntVar>  Var;
//
//	public:
//		NsIntVarArray (int size, const NsInt min_dom, const NsInt
//max_dom);
//
//		NsIntVar&  operator  [] (const int i)
//		{
//			return  Var[i];
//		}
//};

class QueueItem;

/* Abstract class to represent a constraint between constrained variables */

class NsConstraint {

    public:
        // The following two methods are used by the AC-5 algorithm

        virtual void ArcCons(void) = 0;
        virtual void LocalArcCons(QueueItem& Qitem) = 0;

        virtual int VarsInvolvedIn(void) const = 0;

        // The destructor of an abstract class should be virtual
        virtual ~NsConstraint(void)
        {
        }
};

class ConstrXlessthanY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        ConstrXlessthanY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXlessthanY::ConstrXlessthanY: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXlesseqthanY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        ConstrXlesseqthanY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXlesseqthanY::ConstrXlesseqthanY: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYplusC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqYplusC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqYplusC::ConstrXeqYplusC: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqCminusY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqCminusY(NsIntVar* X, const NsInt C_init, NsIntVar* Y)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqCminusY::ConstrXeqCminusY: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYtimesC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqYtimesC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqYtimesC::ConstrXeqYtimesC: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(C != 0, "ConstrXeqYtimesC::ConstrXeqYtimesC: If "
                                   "C==0 the constraint has no reason to "
                                   "exist");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

//  The following constraint is somehow "stronger" than the simple `X == Y +
//  C*Z'.
//   It requires some special conditions, that allow the efficient application
//   of the pure arc-consistency---i.e. not only bounds consistency.

class ConstrXeqYplusCZspecial : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        NsInt C;

    public:
        ConstrXeqYplusCZspecial(NsIntVar* X, NsIntVar* Y, const NsInt C_init,
                                NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrXeqYplusCZspecial::ConstrXeqYplusCZspecial: "
                           "All the variables of a constraint must belong to "
                           "the same NsProblemManager");
                assert_exc(X->min() >= 0, "ConstrXeqYplusCZspecial::"
                                          "ConstrXeqYplusCZspecial: Special "
                                          "condition required:  X >= 0");
                assert_exc(0 <= Y->min() && Y->max() < C,
                           "ConstrXeqYplusCZspecial::ConstrXeqYplusCZspecial: "
                           "Special condition required:  0 <= Y < C");
                assert_exc(C > 0, "ConstrXeqYplusCZspecial::"
                                  "ConstrXeqYplusCZspecial: Condition "
                                  "required:  C > 0");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYplusZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        ConstrXeqYplusZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrXeqYplusZ::ConstrXeqYplusZ: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYtimesZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        ConstrXeqYtimesZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrXeqYtimesZ::ConstrXeqYtimesZ: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYdivC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqYdivC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqYdivC::ConstrXeqYdivC: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
                // assert_exc(Y->min() >= 0,
                //		"ConstrXeqYdivC::ConstrXeqYdivC: Special condition
                //required:  Y >= 0");
                assert_exc(C > 0, "ConstrXeqYdivC::ConstrXeqYdivC: Special "
                                  "condition required:  C > 0");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqCdivY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqCdivY(NsIntVar* X, const NsInt C_init, NsIntVar* Y)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqCdivY::ConstrXeqCdivY: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(C > 0, "ConstrXeqCdivY::ConstrXeqCdivY: Special "
                                  "condition required:  C > 0");
                if (VarY->contains(0))
                        VarY->removeSingle(0, this);
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYmodC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrXeqYmodC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqYmodC::ConstrXeqYmodC: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(Y->min() >= 0, "ConstrXeqYmodC::ConstrXeqYmodC: "
                                          "Special condition required:  Y >= "
                                          "0");
                assert_exc(C > 0, "ConstrXeqYmodC::ConstrXeqYmodC: Special "
                                  "condition required:  C > 0");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYlessthanC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYlessthanC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYlessthanC::ConstrMetaXeqYlessthanC: "
                           "All the variables of a constraint must belong to "
                           "the same NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYlessthanZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        ConstrMetaXeqYlessthanZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrMetaXeqYlessthanZ::ConstrMetaXeqYlessthanZ: "
                           "All the variables of a constraint must belong to "
                           "the same NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYlesseqthanC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYlesseqthanC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYlesseqthanC::"
                           "ConstrMetaXeqYlesseqthanC: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYlesseqthanZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        ConstrMetaXeqYlesseqthanZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrMetaXeqYlesseqthanZ::"
                           "ConstrMetaXeqYlesseqthanZ: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYgreaterthanC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYgreaterthanC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYgreaterthanC::"
                           "ConstrMetaXeqYgreaterthanC: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYgreatereqthanC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYgreatereqthanC(NsIntVar* X, NsIntVar* Y,
                                     const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYgreatereqthanC::"
                           "ConstrMetaXeqYgreatereqthanC: All the variables of "
                           "a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYeqC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYeqC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYeqC::ConstrMetaXeqYeqC: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYeqZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        const bool neg;
        // If `neg==true' the constraint becomes 'ConstrMetaXeqY neq Z'.

    public:
        ConstrMetaXeqYeqZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z, const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrMetaXeqYeqZ::ConstrMetaXeqYeqZ: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrMetaXeqYneqC : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        ConstrMetaXeqYneqC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrMetaXeqYneqC::ConstrMetaXeqYneqC: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

// class ConstrMetaXeqYneqZ : public NsConstraint  {
//	private:
//		NsIntVar  *VarX, *VarY, *VarZ;
//
//	public:
//		ConstrMetaXeqYneqZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
//			: VarX(X), VarY(Y), VarZ(Z)
//		{
//			assert_exc( VarX->manager() == VarY->manager()  &&
//VarY->manager() == VarZ->manager(),  "ConstrMetaXeqYneqZ::ConstrMetaXeqYneqZ:
//All the variables of a constraint must belong to the same NsProblemManager");
//		}
//
//		virtual int   VarsInvolvedIn (void)  const    {  return 3;  }
//
//		virtual void  ArcCons      (void)  const;
//		virtual void  LocalArcCons (QueueItem& Qitem)  const;
//};

class ConstrXeqYandZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        const bool neg;
        // If `neg==true' the constraint becomes 'ConstrXeqY nand Z'.

    public:
        ConstrXeqYandZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z, const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrXeqYandZ::ConstrXeqYandZ: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(0 <= VarX->min() && VarX->max() <= 1 &&
                               0 <= VarY->min() && VarY->max() <= 1 &&
                               0 <= VarZ->min() && VarZ->max() <= 1,
                           "ConstrXeqYandZ::ConstrXeqYandZ: All the variables "
                           "should be \"boolean\"");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqYorZ : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        const bool neg;
        // If `neg==true' the constraint becomes 'ConstrXeqY nor Z'.

    public:
        ConstrXeqYorZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z, const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_exc(&VarX->manager() == &VarY->manager() &&
                               &VarY->manager() == &VarZ->manager(),
                           "ConstrXeqYorZ::ConstrXeqYorZ: All the variables of "
                           "a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(0 <= VarX->min() && VarX->max() <= 1 &&
                               0 <= VarY->min() && VarY->max() <= 1 &&
                               0 <= VarZ->min() && VarZ->max() <= 1,
                           "ConstrXeqYorZ::ConstrXeqYorZ: All the variables "
                           "should be \"boolean\"");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXorY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;
        const bool neg;
        // If `neg==true' the constraint becomes 'Constr not X and not Y'.

    public:
        ConstrXorY(NsIntVar* X, NsIntVar* Y, const bool pos)
          : VarX(X), VarY(Y), neg(!pos)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXorY::ConstrXorY: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(0 <= VarX->min() && VarX->max() <= 1 &&
                               0 <= VarY->min() && VarY->max() <= 1,
                           "ConstrXorY::ConstrXorY: All the variables should "
                           "be \"boolean\"");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqMin : public NsConstraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;

    public:
        ConstrXeqMin(NsIntVar* X, NsIntVarArray* VarArr_init);

        virtual int VarsInvolvedIn(void) const
        {
                return (1 + VarArr->size());
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqMax : public NsConstraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;

    public:
        ConstrXeqMax(NsIntVar* X, NsIntVarArray* VarArr_init);

        virtual int VarsInvolvedIn(void) const
        {
                return (1 + VarArr->size());
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqSum : public NsConstraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;
        NsIndex start, length;

    public:
        ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init);
        ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init,
                     const NsIndex start_init, const NsIndex length_init);

        virtual int VarsInvolvedIn(void) const
        {
                return (1 + length);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        ConstrXeqY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqY::ConstrXeqY: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXneqY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        ConstrXneqY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXneqY::ConstrXneqY: All the variables of a "
                           "constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrXeqAbsY : public NsConstraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        ConstrXeqAbsY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_exc(&VarX->manager() == &VarY->manager(),
                           "ConstrXeqAbsY::cononstrXeqAbsY: All the variables "
                           "of a constraint must belong to the same "
                           "NsProblemManager");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrAllDiff : public NsConstraint {

    private:
        NsIntVarArray* VarArr;

    public:
        ConstrAllDiff(NsIntVarArray* VarArr_init);

        virtual int VarsInvolvedIn(void) const
        {
                return VarArr->size();
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrAllDiffStrong : public NsConstraint {

    public:
        //  `groupedNsIntVar', as the name suggests, is a class that
        //   extends `NsIntVar', by adding the information concerning
        //   the id of the group taht the constrained variable belongs to.

        class groupedNsIntVar {

            public:
                NsIntVar& Var;

            private:
                NsIntVar vGroup;

            public:
                typedef NsInt group_t;

                static const group_t FIRST_GROUP = NsMINUS_INF + 1;

                groupedNsIntVar(NsIntVar& Var_init)
                  : Var(Var_init),
                    vGroup(Var.manager(), FIRST_GROUP, NsPLUS_INF - 1)
                {
                }

                group_t group(void) const
                {
                        return vGroup.min();
                }

                void setGroup(const group_t groupVal)
                {
                        assert_exc(vGroup.contains(groupVal - 1),
                                   "groupedNsIntVar::setGroup: `groupVal-1' is "
                                   "not contained in`vGroup'");
                        assert_exc(
                            vGroup.removeRange(NsMINUS_INF, groupVal - 1),
                            "groupedNsIntVar::setGroup: Could not change group "
                            "to `groupVal'");
                        assert_exc(group() == groupVal,
                                   "groupedNsIntVar::setGroup: Not succesful "
                                   "change of group to `groupVal'");
                }

                bool removeDomain(const IntDomain& V, const NsConstraint* c);
        };

    private:
        NsDeque<groupedNsIntVar> VarArr;

        typedef Ns_UNORDERED_MAP<pointer_t, groupedNsIntVar*> VarPointerGroup_t;

        VarPointerGroup_t VarPointerGroup;

        // NsIntVarArray  VarArrGroup;
        // NsIntVar       vGroupCounter;

        unsigned long Capacity;

    public:
        ConstrAllDiffStrong(NsIntVarArray* VarArr_init, unsigned long Cap);

        virtual int VarsInvolvedIn(void) const
        {
                return VarArr.size();
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class Ns_ConstrCount : public NsConstraint {

    private:
        NsIntVarArray* VarArr;
        const NsDeque<NsInt>& Values;
        const NsDeque<NsInt>& Occurrences;

        /////  vCount[i].min() contains the number of the occurrences of
        ///Values[i] in VarArr.
        // NsIntVarArray  vCount;

    public:
        typedef Ns_UNORDERED_MAP<NsInt, NsIndex> ValueIndex_t;

    private:
        ValueIndex_t ValueIndex;

    public:
        Ns_ConstrCount(NsIntVarArray* VarArr_init,
                       const NsDeque<NsInt>& Values_init,
                       const NsDeque<NsInt>& Occurrences_init);

        virtual int VarsInvolvedIn(void) const
        {
                return VarArr->size();
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ConstrInverse : public NsConstraint {

    private:
        NsIntVarArray *VarArrInv, *VarArr;
        // NsIndex  BaseIdArrInv, BaseIdArr;
        NsIntVarArray &VArrInv, &VArr;

        class ArrayIndex_t {
            public:
                const bool InInverse;
                const NsIndex index;
                ArrayIndex_t(const bool InInverse_init,
                             const NsIndex index_init)
                  : InInverse(InInverse_init), index(index_init)
                {
                }
        };

        typedef Ns_UNORDERED_MAP<pointer_t, ArrayIndex_t> VarArrayIndex_t;

        VarArrayIndex_t VarArrayIndex;

    public:
        ConstrInverse(NsIntVarArray* VarArrInv_init,
                      NsIntVarArray* VarArr_init);

        virtual int VarsInvolvedIn(void) const
        {
                return (VarArrInv->size() + VarArr->size());
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class Ns_ConstrElement : public NsConstraint {

    private:
        NsIntVar* VarIndex;
        const NsDeque<NsInt>& intArray;
        NsIntVar* VarValue;

    public:
        Ns_ConstrElement(NsIntVar* VarIndex_init,
                         const NsDeque<NsInt>& intArray_init,
                         NsIntVar* VarValue_init)
          : VarIndex(VarIndex_init),
            intArray(intArray_init),
            VarValue(VarValue_init)
        {
                // revisionType  =  VALUE_CONSISTENCY;
                assert_exc(&VarIndex->manager() == &VarValue->manager(),
                           "Ns_ConstrElement::Ns_ConstrElement: All the "
                           "variables of a constraint must belong to the same "
                           "NsProblemManager");
                assert_exc(
                    !intArray.empty(),
                    "Ns_ConstrElement::Ns_ConstrElement: Empty element array");
        }

        virtual int VarsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarValue << " -> Var"
                                     << VarIndex << "   [label=\"element\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(QueueItem& Qitem);
};

class ExprYplusC : public Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprYplusC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYplusC operator+(NsIntVar& Y, const NsInt C)
{
        return ExprYplusC(Y, C);
}

inline ExprYplusC operator+(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() + C);
}

inline ExprYplusC operator+(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr.post() + C);
}

inline ExprYplusC operator+(const NsInt C, NsIntVar& Y)
{
        return (Y + C);
}

inline ExprYplusC operator-(NsIntVar& Y, const NsInt C)
{
        return (Y + -C);
}

inline ExprYplusC operator-(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() - C);
}

class ExprYdivC : public Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprYdivC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYdivC operator/(NsIntVar& Y, const NsInt C)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (Y / -C) );
        return ExprYdivC(Y, C);
}

inline ExprYdivC operator/(const Expression& Yexpr, const NsInt C)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (Yexpr / -C) );
        return (Yexpr.post() / C);
}

class ExprYplusCZspecial : public Expression {

    private:
        NsIntVar &VarY, &VarZ;
        NsInt C;

    public:
        ExprYplusCZspecial(NsIntVar& Y, const NsInt C_init, NsIntVar& Z)
          : VarY(Y), VarZ(Z), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYplusCZspecial NsYplusCZspecial(NsIntVar& Y, const NsInt C,
                                           NsIntVar& Z)
{
        return ExprYplusCZspecial(Y, C, Z);
}

class ExprYplusZ : public Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprYplusZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYplusZ operator+(NsIntVar& Y, NsIntVar& Z)
{
        return ExprYplusZ(Y, Z);
}

inline ExprYplusZ operator+(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() + Z);
}

inline ExprYplusZ operator+(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y + Zexpr.post());
}

inline ExprYplusZ operator+(const Expression& Yexpr, const Expression& Zexpr)
{
        return (Yexpr.post() + Zexpr.post());
}

class ExprYminusZ : public Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprYminusZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYminusZ operator-(NsIntVar& Y, NsIntVar& Z)
{
        return ExprYminusZ(Y, Z);
}

inline ExprYminusZ operator-(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() - Z);
}

inline ExprYminusZ operator-(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y - Zexpr.post());
}

inline ExprYminusZ operator-(const Expression& Yexpr, const Expression& Zexpr)
{
        return (Yexpr.post() - Zexpr.post());
}

class ExprCminusZ : public Expression {

    private:
        NsInt C;
        NsIntVar& VarY;

    public:
        ExprCminusZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarY(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprCminusZ operator-(const NsInt C, NsIntVar& Z)
{
        return ExprCminusZ(C, Z);
}

inline ExprCminusZ operator-(const NsInt C, const Expression& Zexpr)
{
        return (C - Zexpr.post());
}

class ExprYtimesZ : public Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprYtimesZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYtimesZ operator*(NsIntVar& Y, NsIntVar& Z)
{
        return ExprYtimesZ(Y, Z);
}

inline ExprYtimesZ operator*(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() * Z);
}

inline ExprYtimesZ operator*(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y * Zexpr.post());
}

inline ExprYtimesZ operator*(const Expression& Yexpr, const Expression& Zexpr)
{
        return (Yexpr.post() * Zexpr.post());
}

class ExprYtimesC : public Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprYtimesC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYtimesC operator*(NsIntVar& Y, const NsInt C)
{
        return ExprYtimesC(Y, C);
}

inline ExprYtimesC operator*(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() * C);
}

inline ExprYtimesC operator*(const NsInt C, NsIntVar& Y)
{
        return (Y * C);
}

inline ExprYtimesC operator*(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr.post() * C);
}

inline ExprCminusZ operator-(NsIntVar& Y)
{
        return (0 - Y);
}

inline ExprCminusZ operator-(const Expression& Yexpr)
{
        return (0 - Yexpr.post());
}

class ExprYdivZ : public Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprYdivZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYdivZ operator/(NsIntVar& Y, NsIntVar& Z)
{
        return ExprYdivZ(Y, Z);
}

inline ExprYdivZ operator/(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() / Z);
}

inline ExprYdivZ operator/(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y / Zexpr.post());
}

inline ExprYdivZ operator/(const Expression& Yexpr, const Expression& Zexpr)
{
        return (Yexpr.post() / Zexpr.post());
}

class ExprCdivZ : public Expression {

    private:
        NsInt C;
        NsIntVar& VarZ;

    public:
        ExprCdivZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprCdivZ operator/(const NsInt C, NsIntVar& Z)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (-C / Z) );
        return ExprCdivZ(C, Z);
}

inline ExprCdivZ operator/(const NsInt C, const Expression& Zexpr)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (-C / Zexpr) );
        return (C / Zexpr.post());
}

class ExprYmodZ : public Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprYmodZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYmodZ operator%(NsIntVar& Y, NsIntVar& Z)
{
        return ExprYmodZ(Y, Z);
}

inline ExprYmodZ operator%(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() % Z);
}

inline ExprYmodZ operator%(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y % Zexpr.post());
}

inline ExprYmodZ operator%(const Expression& Yexpr, const Expression& Zexpr)
{
        return (Yexpr.post() % Zexpr.post());
}

class ExprYmodC : public Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprYmodC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprYmodC operator%(NsIntVar& Y, const NsInt C)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (Y % -C) );
        return ExprYmodC(Y, C);
}

inline ExprYmodC operator%(const Expression& Yexpr, const NsInt C)
{
        // if (C < 0)
        //// In order to conform to the requirements of the constraint
        ///implementation
        //	return  ( - (Yexpr % -C) );
        return (Yexpr.post() % C);
}

class ExprCmodZ : public Expression {

    private:
        NsInt C;
        NsIntVar& VarZ;

    public:
        ExprCmodZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprCmodZ operator%(const NsInt C, NsIntVar& Z)
{
        return ExprCmodZ(C, Z);
}

inline ExprCmodZ operator%(const NsInt C, const Expression& Zexpr)
{
        return (C % Zexpr.post());
}

class ExprAbsY : public Expression {

    private:
        NsIntVar& VarY;

    public:
        ExprAbsY(NsIntVar& Y) : VarY(Y)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprAbsY NsAbs(NsIntVar& Y)
{
        return ExprAbsY(Y);
}

inline ExprAbsY NsAbs(const Expression& Yexpr)
{
        return NsAbs(Yexpr.post());
}

class ExprInDomain : public Expression {

    private:
        NsProblemManager& pm;
        NsDeque<NsInt>& domain;

    public:
        ExprInDomain(NsProblemManager& pm_init, NsDeque<NsInt>& domain_init)
          : pm(pm_init), domain(domain_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprInDomain NsInDomain(NsProblemManager& pm, NsDeque<NsInt>& domain)
{
        return ExprInDomain(pm, domain);
}

inline ExprInDomain NsInDomain(NsProblemManager& pm, NsDeque<NsInt>& domain,
                               NsDeque<NsInt>& domainPrevious,
                               NsDeque<NsInt>& domainNext)
{
        // To suppress warnings...
        domainPrevious.size();
        domainNext.size();
        return NsInDomain(pm, domain);
}

class ExprMin : public Expression {

    private:
        NsIntVarArray& VarArr;

    public:
        ExprMin(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprMin NsMin(NsIntVarArray& Arr)
{
        return ExprMin(Arr);
}

class ExprMax : public Expression {

    private:
        NsIntVarArray& VarArr;

    public:
        ExprMax(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprMax NsMax(NsIntVarArray& Arr)
{
        return ExprMax(Arr);
}

class ExprSum : public Expression {

    private:
        NsIntVarArray& VarArr;
        NsIndex start, length;

    public:
        ExprSum(NsIntVarArray& VarArr_init)
          : VarArr(VarArr_init), start(0), length(VarArr_init.size())
        {
        }

        ExprSum(NsIntVarArray& VarArr_init, const NsIndex start_init,
                const NsIndex length_init)
          : VarArr(VarArr_init), start(start_init), length(length_init)
        {
                //&&  length >= 0,
                assert_exc(start + length <= VarArr.size(),
                           "ExprSum::ExprSum: Wrong `start' or `length'");
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline ExprSum NsSum(NsIntVarArray& Arr)
{
        return ExprSum(Arr);
}

inline ExprSum NsSum(NsIntVarArray& Arr, const NsIndex start,
                     const NsIndex length)
{
        return ExprSum(Arr, start, length);
}

/* Abstract class to represent an expression having to do with arrays of
 * constrained variables */

class ExpressionArray {

    public:
        virtual void post(NsIntVarArray& VarArr) const = 0;

        // The destructor of an abstract class should be virtual
        virtual ~ExpressionArray(void)
        {
        }
};

class ExprInverse : public ExpressionArray {

    private:
        NsIntVarArray& VarArr;
        NsInt MaxDom;

    public:
        ExprInverse(NsIntVarArray& VarArr_init, const NsInt MaxDom_init = -1)
          : VarArr(VarArr_init), MaxDom(MaxDom_init)
        {
        }

        virtual void post(NsIntVarArray& VarArrInv) const;
};

inline ExprInverse NsInverse(NsIntVarArray& Arr)
{
        return ExprInverse(Arr);
}

inline ExprInverse NsInverse(NsIntVarArray& Arr, const NsInt MaxDom)
{
        return ExprInverse(Arr, MaxDom);
}

//  The following abstract class represents the Expressions category   //
//   that can be viewed both as a constraint (e.g. `X < Y') and as     //
//   an expression/meta-constraint (e.g. `Z == X < Y').                //

class ExprConstr : public Expression {

    protected:
        bool is_positive;
        //  If `is_positive==false' then the semantics of the
        //  constraint-expression
        //   is inverted.  E.g. `X < Y' becomes 'X >= Y'.

    public:
        // void  negate (void)
        //{
        //	is_positive = !is_positive;
        //}

        ExprConstr(const bool is_positive_init) : is_positive(is_positive_init)
        {
        }

        virtual NsConstraint* postConstraint(bool positively = true) const = 0;
        //	{  throw  NsException("ExprConstr::postConstraint:
        //Unimplemented");  }

        virtual void post(NsIntVar& VarX) const
        {
                postC(VarX, true);
        }

        virtual NsIntVar& post(void) const
        {
                return postC(true);
        }

        virtual void postC(NsIntVar& VarX, bool positively = true) const = 0;
        virtual NsIntVar& postC(bool positively = true) const = 0;
};

class ExprConstrYlessthanC : public ExprConstr {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprConstrYlessthanC(NsIntVar& Y, const NsInt C_init,
                             const bool is_positive_init)
          : ExprConstr(is_positive_init), VarY(Y), C(C_init)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class ExprConstrYlesseqthanC : public ExprConstr {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprConstrYlesseqthanC(NsIntVar& Y, const NsInt C_init,
                               const bool is_positive_init)
          : ExprConstr(is_positive_init), VarY(Y), C(C_init)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

// class ExprConstrYgreaterthanC : public ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		ExprConstrYgreaterthanC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual NsConstraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};
//
//
//
//
//
// class ExprConstrYgreatereqthanC : public ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		ExprConstrYgreatereqthanC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual NsConstraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};

class ExprConstrYeqC : public ExprConstr {
    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        ExprConstrYeqC(NsIntVar& Y, const NsInt C_init,
                       const bool is_positive_init)
          : ExprConstr(is_positive_init), VarY(Y), C(C_init)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

// class ExprConstrYneqC : public ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		ExprConstrYneqC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual NsConstraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};

class ExprConstrYlessthanZ : public ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprConstrYlessthanZ(NsIntVar& Y, NsIntVar& Z)
          : ExprConstr(true), VarY(Y), VarZ(Z)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class ExprConstrYlesseqthanZ : public ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprConstrYlesseqthanZ(NsIntVar& Y, NsIntVar& Z)
          : ExprConstr(true), VarY(Y), VarZ(Z)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class ExprConstrYeqZ : public ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprConstrYeqZ(NsIntVar& Y, NsIntVar& Z, const bool is_positive_init)
          : ExprConstr(is_positive_init), VarY(Y), VarZ(Z)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

// class ExprConstrYneqZ : public ExprConstr  {
//	private:
//		NsIntVar  &VarY, &VarZ;
//
//	public:
//		ExprConstrYneqZ (NsIntVar& Y, NsIntVar& Z)
//			: VarY(Y), VarZ(Z)	{   }
//
//		virtual NsConstraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};

class ExprConstrYandZ : public ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprConstrYandZ(NsIntVar& Y, NsIntVar& Z)
          : ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_exc(0 <= VarY.min() && VarY.max() <= 1 &&
                               0 <= VarZ.min() && VarZ.max() <= 1,
                           "ExprConstrYandZ::ExprConstrYandZ: `VarY' and "
                           "`VarZ' should be boolean");
        }

        virtual NsConstraint* postConstraint(bool positively) const
        {
                // to suppress warnings
                positively = positively;
                throw NsException("ExprConstrYandZ::postConstraint: Please, "
                                  "add the two constraints separately");
        }

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class ExprConstrYorZ : public ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        ExprConstrYorZ(NsIntVar& Y, NsIntVar& Z)
          : ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_exc(0 <= VarY.min() && VarY.max() <= 1 &&
                               0 <= VarZ.min() && VarZ.max() <= 1,
                           "ExprConstrYorZ::ExprConstrYorZ: `VarY' and `VarZ' "
                           "should be boolean");
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class ExprConstrNegationY : public ExprConstr {

    private:
        const ExprConstr& Yexpr;

    public:
        ExprConstrNegationY(const ExprConstr& Y) : ExprConstr(true), Yexpr(Y)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const
        {
                return Yexpr.postConstraint(!positively);
        }

        virtual void postC(NsIntVar& VarX, bool positively) const
        {
                Yexpr.postC(VarX, !positively);
        }
        virtual NsIntVar& postC(bool positively) const
        {
                return Yexpr.postC(!positively);
        }
};

class ExprConstrAllDiff : public ExprConstr {

    private:
        NsIntVarArray& VarArr;
        unsigned long Capacity;

    public:
        ExprConstrAllDiff(NsIntVarArray& Arr, unsigned long Cap)
          : ExprConstr(true), VarArr(Arr), Capacity(Cap)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const
        {
                VarX.size(); // to suppress warnings
                // to suppress warnings
                positively = positively;
                throw NsException("ExprConstrAllDiff::postC: NsAllDiff cannot "
                                  "be used as a meta-constraint");
        }
        virtual NsIntVar& postC(bool positively) const
        {
                // to suppress warnings
                positively = positively;
                throw NsException("ExprConstrAllDiff::postC: NsAllDiff cannot "
                                  "be used as a meta-constraint");
        }
};

class Ns_ExprConstrCount : public ExprConstr {

    private:
        NsIntVarArray& VarArr;
        const NsDeque<NsInt>& Values;
        const NsDeque<NsInt>& Occurrences;

    public:
        Ns_ExprConstrCount(NsIntVarArray& Arr,
                           const NsDeque<NsInt>& Values_init,
                           const NsDeque<NsInt>& Occurrences_init)
          : ExprConstr(true),
            VarArr(Arr),
            Values(Values_init),
            Occurrences(Occurrences_init)
        {
        }

        virtual NsConstraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const
        {
                VarX.size(); // to suppress warnings
                // to suppress warnings
                positively = positively;
                throw NsException("Ns_ExprConstrCount::postC: NsCount cannot "
                                  "be used as a meta-constraint");
        }
        virtual NsIntVar& postC(bool positively) const
        {
                // to suppress warnings
                positively = positively;
                throw NsException("Ns_ExprConstrCount::postC: NsCount cannot "
                                  "be used as a meta-constraint");
        }
};

inline ExprConstrYlessthanC operator<(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYlessthanC(Y, C, true);
}

inline ExprConstrYlessthanC operator>(const NsInt C, NsIntVar& Y)
{
        return (Y < C);
}

inline ExprConstrYlessthanC operator<(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() < C);
}

inline ExprConstrYlessthanC operator>(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr < C);
}

inline ExprConstrYlesseqthanC operator<=(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYlesseqthanC(Y, C, true);
}

inline ExprConstrYlesseqthanC operator>=(const NsInt C, NsIntVar& Y)
{
        return (Y <= C);
}

inline ExprConstrYlesseqthanC operator<=(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() <= C);
}

inline ExprConstrYlesseqthanC operator>=(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr <= C);
}

inline ExprConstrYlesseqthanC operator>(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYlesseqthanC(Y, C, false);
}

inline ExprConstrYlesseqthanC operator<(const NsInt C, NsIntVar& Y)
{
        return (Y > C);
}

inline ExprConstrYlesseqthanC operator>(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() > C);
}

inline ExprConstrYlesseqthanC operator<(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr > C);
}

inline ExprConstrYlessthanC operator>=(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYlessthanC(Y, C, false);
}

inline ExprConstrYlessthanC operator<=(const NsInt C, NsIntVar& Y)
{
        return (Y >= C);
}

inline ExprConstrYlessthanC operator>=(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() >= C);
}

inline ExprConstrYlessthanC operator<=(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr >= C);
}

inline ExprConstrYeqC operator==(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYeqC(Y, C, true);
}

inline ExprConstrYeqC operator==(const NsInt C, NsIntVar& Y)
{
        return (Y == C);
}

inline ExprConstrYeqC operator==(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() == C);
}

inline ExprConstrYeqC operator==(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr == C);
}

inline ExprConstrYeqC operator!=(NsIntVar& Y, const NsInt C)
{
        return ExprConstrYeqC(Y, C, false);
}

inline ExprConstrYeqC operator!=(const NsInt C, NsIntVar& Y)
{
        return (Y != C);
}

inline ExprConstrYeqC operator!=(const Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() != C);
}

inline ExprConstrYeqC operator!=(const NsInt C, const Expression& Yexpr)
{
        return (Yexpr != C);
}

inline ExprConstrYlessthanZ operator<(NsIntVar& Y, NsIntVar& Z)
{
        return ExprConstrYlessthanZ(Y, Z);
}

inline ExprConstrYlessthanZ operator>(NsIntVar& Z, NsIntVar& Y)
{
        return (Y < Z);
}

inline ExprConstrYlessthanZ operator<(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y < Zexpr.post());
}

inline ExprConstrYlessthanZ operator<(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() < Z);
}

inline ExprConstrYlessthanZ operator>(NsIntVar& Z, const Expression& Yexpr)
{
        return (Yexpr < Z);
}

inline ExprConstrYlessthanZ operator>(const Expression& Zexpr, NsIntVar& Y)
{
        return (Y < Zexpr);
}

inline ExprConstrYlessthanZ operator<(const Expression& Yexpr,
                                      const Expression& Zexpr)
{
        return (Yexpr.post() < Zexpr.post());
}

inline ExprConstrYlessthanZ operator>(const Expression& Zexpr,
                                      const Expression& Yexpr)
{
        return (Yexpr < Zexpr);
}

inline ExprConstrYlesseqthanZ operator<=(NsIntVar& Y, NsIntVar& Z)
{
        return ExprConstrYlesseqthanZ(Y, Z);
}

inline ExprConstrYlesseqthanZ operator>=(NsIntVar& Z, NsIntVar& Y)
{
        return (Y <= Z);
}

inline ExprConstrYlesseqthanZ operator<=(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y <= Zexpr.post());
}

inline ExprConstrYlesseqthanZ operator<=(const Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() <= Z);
}

inline ExprConstrYlesseqthanZ operator>=(NsIntVar& Z, const Expression& Yexpr)
{
        return (Yexpr <= Z);
}

inline ExprConstrYlesseqthanZ operator>=(const Expression& Zexpr, NsIntVar& Y)
{
        return (Y <= Zexpr);
}

inline ExprConstrYlesseqthanZ operator<=(const Expression& Yexpr,
                                         const Expression& Zexpr)
{
        return (Yexpr.post() <= Zexpr.post());
}

inline ExprConstrYlesseqthanZ operator>=(const Expression& Zexpr,
                                         const Expression& Yexpr)
{
        return (Yexpr <= Zexpr);
}

inline ExprConstrYeqZ operator==(NsIntVar& Y, NsIntVar& Z)
{
        return ExprConstrYeqZ(Y, Z, true);
}

inline ExprConstrYeqZ operator==(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y == Zexpr.post());
}

inline ExprConstrYeqZ operator==(const Expression& Zexpr, NsIntVar& Y)
{
        return (Y == Zexpr);
}

inline ExprConstrYeqZ operator==(const Expression& Yexpr,
                                 const Expression& Zexpr)
{
        return (Yexpr.post() == Zexpr.post());
}

inline ExprConstrYeqZ operator!=(NsIntVar& Y, NsIntVar& Z)
{
        return ExprConstrYeqZ(Y, Z, false);
}

inline ExprConstrYeqZ operator!=(NsIntVar& Y, const Expression& Zexpr)
{
        return (Y != Zexpr.post());
}

inline ExprConstrYeqZ operator!=(const Expression& Zexpr, NsIntVar& Y)
{
        return (Y != Zexpr);
}

inline ExprConstrYeqZ operator!=(const Expression& Yexpr,
                                 const Expression& Zexpr)
{
        return (Yexpr.post() != Zexpr.post());
}

inline ExprConstrYandZ operator&&(const ExprConstr& Yexpr,
                                  const ExprConstr& Zexpr)
{
        return ExprConstrYandZ(Yexpr.post(), Zexpr.post());
}

inline ExprConstrYorZ operator||(const ExprConstr& Yexpr,
                                 const ExprConstr& Zexpr)
{
        return ExprConstrYorZ(Yexpr.post(), Zexpr.post());
}

inline ExprConstrNegationY operator!(const ExprConstr& Yexpr)
{
        return ExprConstrNegationY(Yexpr);
}

inline ExprConstrAllDiff NsAllDiff(NsIntVarArray& Arr,
                                   unsigned long Capacity = 0)
{
        return ExprConstrAllDiff(Arr, Capacity);
}

inline Ns_ExprConstrCount NsCount(NsIntVarArray& Arr,
                                  const NsDeque<NsInt>& Values,
                                  const NsDeque<NsInt>& Occurrences)
{
        return Ns_ExprConstrCount(Arr, Values, Occurrences);
}

inline ExprConstrYorZ NsIfThen(const ExprConstr& Yexpr, const ExprConstr& Zexpr)
{
        //  "p => q"  is equivalent to  "!p || q".
        return (!Yexpr || Zexpr);
}

inline ExprConstrYeqZ NsEquiv(const ExprConstr& Yexpr, const ExprConstr& Zexpr)
{
        //  "p <=> q"  is equivalent to  "p == q".
        return (Yexpr == Zexpr);
}

inline ExprConstrYeqZ NsElement(NsIntVar& VarIndex,
                                const NsDeque<NsInt>& intArray,
                                NsIntVar& VarValue)
{
        return (intArray[VarIndex] == VarValue);
}

inline ExprConstrYeqZ NsElement(const Expression& VarIndexExpr,
                                const NsDeque<NsInt>& intArray,
                                NsIntVar& VarValue)
{
        return NsElement(VarIndexExpr.post(), intArray, VarValue);
}

inline ExprConstrYeqZ NsElement(NsIntVar& VarIndex,
                                const NsDeque<NsInt>& intArray,
                                const Expression& VarValueExpr)
{
        return NsElement(VarIndex, intArray, VarValueExpr.post());
}

inline ExprConstrYeqZ NsElement(const Expression& VarIndexExpr,
                                const NsDeque<NsInt>& intArray,
                                const Expression& VarValueExpr)
{
        return NsElement(VarIndexExpr.post(), intArray, VarValueExpr.post());
}

/*
 *  `NsGoal' is an Abstract class representing an (internal or constructed by
 * user) goal.
 *
 *  Each time a goal is executed by the solver, its method `GOAL()' is called.
 * This method can make assignments or remove values from constrained variables.
 */

class NsGoal {

    public:
        virtual bool isGoalAND(void) const
        {
                return false;
        }

        virtual bool isGoalOR(void) const
        {
                return false;
        }

        virtual NsGoal* getFirstSubGoal(void) const
        {
                throw NsException(
                    "NsGoal::getFirstSubGoal: *this: Not an AND/OR goal");
        }

        virtual NsGoal* getSecondSubGoal(void) const
        {
                throw NsException(
                    "NsGoal::getSecondSubGoal: *this: Not an AND/OR goal");
        }

        virtual NsGoal* GOAL(void) = 0;

        // The destructor of an abstract class should be virtual
        virtual ~NsGoal(void)
        {
        }
};

//  The following goal classes `NsgAND' and `NsgOR' represent "meta-goals."
//   I.e. they are goals used to combine two other goals.

class NsgAND : public NsGoal {

    private:
        NsGoal* FirstSubGoal;
        NsGoal* SecondSubGoal;

    public:
        NsgAND(NsGoal* FirstSubGoal_init, NsGoal* SecondSubGoal_init)
          : FirstSubGoal(FirstSubGoal_init), SecondSubGoal(SecondSubGoal_init)
        {
                assert_exc(FirstSubGoal != 0 && SecondSubGoal != 0,
                           "NsgAND::NsgAND: A subgoal is zero");
        }

        virtual bool isGoalAND(void) const
        {
                return true;
        }

        virtual NsGoal* getFirstSubGoal(void) const
        {
                return FirstSubGoal;
        }

        virtual NsGoal* getSecondSubGoal(void) const
        {
                return SecondSubGoal;
        }

        NsGoal* GOAL(void)
        {
                throw NsException(
                    "NsgAND::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

class NsgOR : public NsGoal {

    private:
        NsGoal* FirstSubGoal;
        NsGoal* SecondSubGoal;

    public:
        NsgOR(NsGoal* FirstSubGoal_init, NsGoal* SecondSubGoal_init)
          : FirstSubGoal(FirstSubGoal_init), SecondSubGoal(SecondSubGoal_init)
        {
                assert_exc(FirstSubGoal != 0 && SecondSubGoal != 0,
                           "NsgOR::NsgOR: A subgoal is zero");
        }

        virtual bool isGoalOR(void) const
        {
                return true;
        }

        virtual NsGoal* getFirstSubGoal(void) const
        {
                return FirstSubGoal;
        }

        virtual NsGoal* getSecondSubGoal(void) const
        {
                return SecondSubGoal;
        }

        NsGoal* GOAL(void)
        {
                throw NsException(
                    "NsgOR::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

//  `NsgSetValue' is a simple goal that assigns a value to a constrained
//   variable.  On the other hand, `NsgRemoveValue' is a goal that
//   removes a value from the domain of a given constrained variable.

class NsgSetValue : public NsGoal {

    private:
        NsIntVar& Var;
        const NsInt value;

    public:
        NsgSetValue(NsIntVar& Var_init, const NsInt value_init)
          : Var(Var_init), value(value_init)
        {
        }

        NsGoal* GOAL(void)
        {
                // std::cout << "  NsgSetValue:   ";
                Var.set(value);
                return 0;
        }
};

class NsgRemoveValue : public NsGoal {

    private:
        NsIntVar& Var;
        const NsInt value;

    public:
        NsgRemoveValue(NsIntVar& Var_init, const NsInt value_init)
          : Var(Var_init), value(value_init)
        {
        }

        NsGoal* GOAL(void)
        {
                // std::cout << "  NsgRemoveValue:";
                Var.remove(value);
                return 0;
        }
};

//  `NsgInDomain' is a goal that tries to instantiate a constrained
//   variable.   I.e. it assigns a value from the domain of the variable;
//   if this assignment results to an inconsistency, it removes the value
//   from the domain, and continues trying the rest of the values.

class NsgInDomain : public NsGoal {

    private:
        NsIntVar& Var;

    public:
        NsgInDomain(NsIntVar& Var_init) : Var(Var_init)
        {
        }

        NsGoal* GOAL(void)
        {
                // std::cout << "  NsgInDomain:    ";
                if (Var.isBound())
                        return 0;
                NsInt value = Var.min();
                return (new NsgOR(new NsgSetValue(Var, value),
                                  new NsgAND(new NsgRemoveValue(Var, value),
                                             new NsgInDomain(*this))));
        }
};

//  `NsgLabeling' is the generalization of `NsgInDomain' that applies to arrays.
//   It uses `NsgInDomain' to iteratively instantiate each variable of the
//   array. The next variable that is chosen is the one having the minimum
//   domain size (according to the "first-fail" heuristic).

class NsgLabeling : public NsGoal {

    private:
        NsIntVarArray& VarArr;

    public:
        NsgLabeling(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        NsGoal* GOAL(void)
        {
                // std::cout << "  NsgLabeling:       ";
                int index = -1;
                NsUInt minDom = NsUPLUS_INF;
                for (NsIndex i = 0; i < VarArr.size(); ++i) {
                        if (!VarArr[i].isBound() && VarArr[i].size() < minDom) {
                                minDom = VarArr[i].size();
                                index = i;
                        }
                }
                if (index == -1)
                        return 0;
                return (new NsgAND(new NsgInDomain(VarArr[index]),
                                   new NsgLabeling(*this)));
        }
};

//  `QueueItem' is the type of the objects in the queue for the AC-5 algorithm.
//   When a value is removed from the domain of a variable, an item is added
//   into this queue.

class QueueItem {

    private:
        //  `constr_fired' is the constraint that fired the removal of
        //   the value `w' out of the domain of the variable `var_fired'.

        const NsIntVar* var_fired;
        const NsConstraint* constr_fired;
        const NsInt w;

        //  If no constraint provoke the removal, then `constr_fired==0'.

        CONSTR_arr_t::size_type curr_constr;

    public:
        QueueItem(const NsIntVar* var_f, const NsConstraint* constr_f,
                  const int w_init)
          : var_fired(var_f), constr_fired(constr_f), w(w_init), curr_constr(0)
        {
        }

        //  The following method returns the next constraint to `curr_constr'
        //   concerning the constrained variable `var_fired'.
        NsConstraint* getNextConstraint(void);

        const NsIntVar* getVarFired(void) const
        {
                return var_fired;
        }

        // const NsConstraint*  getConstrFired (void)  const
        //  {  return  constr_fired;  }

        NsInt getW(void) const
        {
                return w;
        }
};

//  `StackGoals' is a class normally used for describing the stack holding
//  AND-goals
//   that have to be satisfied.  (This stack is also called `StackAND'.)

class StackGoals : public NsStack<NsGoal*> {

    public:
        ~StackGoals(void);
};

//  `VarDom_t' describes a tuple "(VariablePointer, aDomain)".

class VarDom_t {

    public:
        NsIntVar* Var;
        const IntDomain Dom;

        VarDom_t(NsIntVar* Var_init, const IntDomain& Dom_init)
          : Var(Var_init), Dom(Dom_init)
        {
        }
};

typedef NsStack<VarDom_t> StackDomains;
typedef Ns_UNORDERED_SET<pointer_t> Set_t;

//  An object of the class `DomsStore_t' stores all the necessary data that
//   could be used if we wanted to restore the initial domains of all the
//   constrained variables.  E.g., before the domain of a variable `V' is
//   altered, we add the tuple `(V, DomainOfV)' to the stack `DomsOrig'.
//   When it is time to restore the initial domains, we pop all the frames
//   out of `DomsOrig', and for each tuple `(V, DomainOfV)' we call the
//   method `V->setDomain(DomainOfV)'.  The data-member `AlreadySaved'
//   is a (hash) set containing all the pointers of the variables already
//   stored.  We could live without it--as the same information is also
//   contained in `DomsOrig'--but its usefulness has to do with the
//   acceleration of the searching whether the domain of a specific variable
//   has been already saved.  (It takes constant time, though a search
//   within `DomsOrig' takes linear time.)

class DomsStore_t {

    public:
        StackDomains DomsOrig;
        Set_t AlreadySaved;

        void clear(void)
        {
                DomsOrig.clear();
                AlreadySaved.clear();
        }
};

//  `DomsStore_t' could be only a `hash_map<pointer_t,IntDomain>'. `DomsStore_t'
//   plays a role of a hash_map, although we implemented it in an indirect way
//   --using hash_set--plainly for performance reasons (having to do with time).

//  `Ns_StackSearch' contains all the `SearchNode's.  A `SearchNode' contains
//   the current status of the problem; it is pushed into the stack when we
//   make a choice (e.g. when we select a specific value to assign it to a
//   variable) and it is popped when we want to cancel this choice, and we
//   want to revert back to the previous problem status.

class SearchNode;

class Ns_StackSearch : public NsStack<SearchNode> {

    public:
        Ns_StackSearch(void);

        ~Ns_StackSearch(void);

        void clear(void);

        ///  @{
        ///  \name  Provision of history ids to the search nodes

    private:
        NsDeque<NsUInt> validHistoryId;

        unsigned long nSearchTreeNodes;

    public:
        unsigned long numSearchTreeNodes(void) const
        {
                return (nSearchTreeNodes - 1);
        }

        void push(const value_type& newNode);

        void pop(void);

        ///  Restores the validHistoryId 's state as it was before search began.
        void reset(void)
        {
                validHistoryId[0] = 0;
        }

        Ns_HistoryId_t getCurrentHistoryId(void) const
        {
                Ns_HistoryId_t di;
                di.level = size() - 1;
                di.id = validHistoryId[di.level];
                return di;
        }

        bool isCurrentHistoryId(const Ns_HistoryId_t di) const
        {
                return (di.level == size() - 1 &&
                        di.id == validHistoryId[di.level]);
        }

        bool isValidHistoryId(const Ns_HistoryId_t di) const
        {
                return (di.id == validHistoryId[di.level]);
        }

        ///  @}

        ///  @{
        ///  \name  Representation of the search tree as a graph

    private:
        ///  File to store the search tree graph.
        std::ofstream fileSearchGraph;

        ///  True if it should write the objective value.
        bool recordObjective;

        ///  The last recorded objectiveValue.
        NsInt objectiveValue;

    public:
        void searchToGraphFile(const char* fileName);

        void solutionNode(const NsIntVar* vObjective);

        ///  @}

        //  Declaration of an iterator to iterate through all the goals
        //  in the current `StackGoals' and the `StackGoals' below it.
        //  (All of them consist a stack of `StackGoals', named
        //  `StackOfStacks'.)

        friend class goal_iterator;

        class goal_iterator {

            private:
                //  `StackOfStacks' is a stack containing the search nodes
                //   (so each frame of this stack contains a `StackGoals').
                Ns_StackSearch* StackOfStacks;

                // points to the current search node.
                Ns_StackSearch::iterator curr_Stack_it;
                // points to the current goal (in the
                StackGoals::iterator curr_node_it;
                //  `StackGoals') of the current search node.

            public:
                goal_iterator(void) : StackOfStacks(0)
                {
                }

                goal_iterator(Ns_StackSearch& StackOfStacks_init);

                bool operator==(const goal_iterator& b) const
                {
                        assert_exc(StackOfStacks != 0, "Ns_StackSearch::goal_"
                                                       "iterator::==: "
                                                       "Uninitialized `*this'");
                        return (curr_Stack_it == b.curr_Stack_it &&
                                (curr_Stack_it == StackOfStacks->end() ||
                                 curr_node_it == b.curr_node_it));
                }

                bool operator!=(const goal_iterator& b) const
                {
                        return !(*this == b);
                }

                NsGoal* operator*(void)const
                {
                        assert_exc(StackOfStacks != 0, "Ns_StackSearch::goal_"
                                                       "iterator::*: "
                                                       "Uninitialized `*this'");
                        assert_exc(
                            curr_Stack_it != StackOfStacks->end()
                            /*&&  curr_node_it != curr_Stack_it->StackAND.end()*/
                            ,
                            "Ns_StackSearch::goal_iterator::*: Bad request");
                        return *curr_node_it;
                }

                goal_iterator& end(void)
                {
                        assert_exc(StackOfStacks != 0, "Ns_StackSearch::goal_"
                                                       "iterator::end: "
                                                       "Uninitialized `*this'");
                        curr_Stack_it = StackOfStacks->end();
                        return *this;
                }

                goal_iterator& operator++(void);
        };

        goal_iterator gbegin(void)
        {
                return goal_iterator(*this);
        }

        goal_iterator gend(void)
        {
                goal_iterator iter_end(*this);
                return iter_end.end();
        }
};

//  `SearchNode' is a critical unit describing the current status TODO

class SearchNode {

    public:
        NsGoal* GoalNextChoice;

        Ns_StackSearch::goal_iterator delayed_goal;

        DomsStore_t DomsStore;

        StackGoals StackAND;

        SearchNode(NsGoal* GoalNextChoice_init,
                   Ns_StackSearch::goal_iterator git)
          : GoalNextChoice(GoalNextChoice_init),
            delayed_goal(git),
            has_a_child(false)
        {
        }

        ///  True, if the current node creates another OR-node.
        bool has_a_child;
};

typedef Queue<QueueItem> Queue_t;

class NsProblemManager {

    private:
        NsDeque<NsIntVar*> IntermediateVar;
        CONSTR_arr_t CONSTR;
        Ns_StackSearch searchNodes;

        Queue_t Q;

        // NsIntVarArray  *VarGoal;
        NsIntVar* VarMinObj;
        NsInt BestMinObj;

        // bool  initialized;
        // bool  just_initialized;
        // bool  finished;

        bool FoundInconsistency;

        bool ArcConsinstent(void);
        bool BackTrack(void);

        bool isRealTime;
        clock_t startTime;
        time_t startRealTime;
        unsigned long timeLim;
        bool FirstNextSolution;
        bool CalledTimeLimit;

        unsigned long nFailures, nBackTracks, nGoals, nConstraintChecks,
            backTrackLim, nVars;

    public:
        NsProblemManager(void)
          : VarMinObj(0),
            FoundInconsistency(false),
            timeLim(0),
            FirstNextSolution(true),
            CalledTimeLimit(false),
            nFailures(0),
            nBackTracks(0),
            nGoals(0),
            nConstraintChecks(0),
            backTrackLim(0),
            nVars(0)
        //, VarsID(0), VarGoal(0), initialized(false), finished(false),
        {
                assert_exc(sizeof(NsIntVar*) <= sizeof(pointer_t),
                           "NsProblemManager::NsProblemManager: Cannot run on "
                           "this machine, because a pointer does not fit in an "
                           "`unsigned long' (the `unordered_set' type)");
                // searchNodes.push( SearchNode( 0, StackGoals::iterator(),
                // searchNodes.gbegin() ) );
                searchNodes.push(SearchNode(0, searchNodes.gbegin()));
                // searchNodes.push( SearchNode( 0, searchNodes.gbegin() ) );
                ////  The first push takes place, because some goals
                ///(NsgSetValue and NsgRemoveValue) /   save the domains of the
                ///variables they handle, in the *previous* node of the /
                ///search tree.
        }

        ~NsProblemManager(void);

        DomsStore_t& getCurrDomsStore(void)
        {
                return searchNodes.top().DomsStore;
        }

        //	DomsStore_t&
        // getPrevDomsStore (void)
        //{
        //	return  searchNodes.top_MinusOne().DomsStore;
        //}

        Queue_t& getQ(void)
        {
                return Q;
        }

        void FoundAnInconsistency(void)
        {
                FoundInconsistency = true;
        }

        bool isPartiallyConsistent(void) const
        {
                return !FoundInconsistency;
        }

        //	void
        // ClearInconsistency (void)
        //{
        //	FoundInconsistency = false;
        //}

        void addGoal(NsGoal* goal)
        {
                if (goal != 0)
                        searchNodes.top().StackAND.push(goal);
        }

        void add(const ExprConstr& expr);

        // void  add_XeqYplusC (NsIntVar& X, NsIntVar& Y, const NsInt C);
        // void  add_XeqYplusCZspecial (NsIntVar& X, NsIntVar& Y, const NsInt C,
        // NsIntVar& Z);  void  add_XeqYdivC (NsIntVar& X, NsIntVar& Y, const
        // NsInt C);  void  add_XneqY (NsIntVar& X, NsIntVar& Y);

        // void  setGoal (NsIntVarArray& VarArr)
        //{
        //	assert( VarArr.size() > 0 );
        //	VarGoal = &VarArr;
        //}

        void minimize(const Expression& expr)
        {
                minimize(expr.post());
        }

        void minimize(NsIntVar& VarObj)
        {
                // assert_exc( !initialized ,  "NsProblemManager::minimize:
                // Search has been initialized");
                VarMinObj = &VarObj;
                BestMinObj = NsPLUS_INF;
        }

        void objectiveUpperLimit(NsInt max)
        {
                assert_exc(VarMinObj != 0, "NsProblemManager::"
                                           "objectiveUpperLimit: No cost "
                                           "variable exists");
                if (BestMinObj > max + 1) {
                        BestMinObj = max + 1;
                        VarMinObj->removeRange(BestMinObj, NsPLUS_INF, 0);
                }
        }

        void realTimeLimit(const unsigned long lim)
        {
                CalledTimeLimit = true;
                isRealTime = true;
                timeLim = lim;
        }

        void timeLimit(const unsigned long lim)
        {
                CalledTimeLimit = true;
                // We use CPU time
                isRealTime = false;
                timeLim = lim;
        }

        void backtrackLimit(const unsigned long lim)
        {
                backTrackLim = nBackTracks + lim;
        }

        // bool  Initialize (void);
        bool nextSolution(void);

        void restart(void);

        void RecordIntermediateVar(NsIntVar* Var)
        {
                IntermediateVar.push_back(Var);
        }

        void RecordConstraint(NsConstraint* NewConstr)
        {
                CONSTR.push_back(NewConstr);
        }

        /* Statistic methods "end TODO"*/

        unsigned long numFailures(void) const
        {
                return nFailures;
        }

        unsigned long numGoals(void) const
        {
                return nGoals;
        }

        unsigned long numBacktracks(void) const
        {
                return nBackTracks;
        }

        unsigned long numVars(void) const
        {
                return nVars;
        }

        CONSTR_arr_t::size_type numConstraints(void) const
        {
                return CONSTR.size();
        }

        unsigned long numConstraintChecks(void) const
        {
                return nConstraintChecks;
        }

        unsigned long numSearchTreeNodes(void) const
        {
                return searchNodes.numSearchTreeNodes();
        }

        void addVar(void)
        {
                ++nVars;
        }

        void printCspParameters(void) const
        {
        }

        ///  @{
        ///  \name  Provision of history ids to the search nodes

        Ns_HistoryId_t getCurrentHistoryId(void) const
        {
                return searchNodes.getCurrentHistoryId();
        }

        bool isCurrentHistoryId(const Ns_HistoryId_t di) const
        {
                return searchNodes.isCurrentHistoryId(di);
        }

        bool isValidHistoryId(const Ns_HistoryId_t di) const
        {
                return searchNodes.isValidHistoryId(di);
        }

        ///  @}

        ///  Writes to a file a view of the search tree in a Graphviz supported
        ///  format.
        void searchToGraphFile(const char* fileName)
        {
                searchNodes.searchToGraphFile(fileName);
        }

        //	private:
        //		NsIndex  VarsID;
        //
        //	public:
        //		NsIndex  generateID (void)
        //		{
        //			assert_exc(++VarsID != 0,
        //"NsProblemManager::generateID: `VarsID' overflow");
        //			return  VarsID;
        //		}
};

// INTERNAL DECLARATIONS //

bool IntersectionEmpty(const NsIntVar* VarY, const NsIntVar* VarZ);

void YmodC_min_max(const NsIntVar* VarY, const NsInt C, NsInt& min,
                   NsInt& minmax);

void product_min_max(const NsIntVar* VarY, const NsIntVar* VarZ, NsInt& min,
                     NsInt& max);

void array_min_minmax(const NsIntVarArray* VarArr, NsInt& min, NsInt& minmax);

void array_maxmin_max(const NsIntVarArray* VarArr, NsInt& maxmin, NsInt& max);

void array_sum_min_max(const NsIntVarArray* VarArr, const NsIndex start,
                       const NsIndex length, NsInt& summin, NsInt& summax);

} // end namespace
#endif // Ns_NAXOS_H
