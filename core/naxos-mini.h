/// @file
/// Definitions of internal and public classes of Naxos

// In the comments, words beginning with the symbol '@' should
// be ignored by the reader, because they are used by Doxygen,
// a program which generates a source code reference manual.

/// @mainpage Naxos: A C++ Constraint Programming Library.
/// For a general idea and information on how to use it,
/// please visit the https://github.com/pothitos/naxos page.
/// The comments you're reading right now can be only used for
/// internal developing purposes.
///
/// The names of Naxos classes and other declarations begin with
/// 'Ns', so it is a good practice not to put this prefix to
/// your classes or data-types. Moreover, prefix 'Nsg' is used
/// for Naxos goal classes, and prefix 'Ns_' is used for
/// internal classes that should not be used by the users of
/// Naxos.

#ifndef Ns_NAXOS_MINI_H
#define Ns_NAXOS_MINI_H

#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <stdexcept>
#include <string>

#include <unordered_map>
#include <unordered_set>
#define Ns_UNORDERED_SET std::unordered_set
#define Ns_UNORDERED_MAP std::unordered_map

/// Naxos solver's namespace
namespace naxos {

/// Naxos methods throw this type of exception
class NsException : public std::logic_error {

    public:
        NsException(const std::string& error_message)
          : logic_error("Naxos: " + error_message)
        {
        }
};

/// Asserts that the condition is true; if it is not, it throws a NsException
///
/// Here we used the type char* for message, instead of string,
/// plainly for time-performance reasons.
inline void assert_Ns(const bool condition, const char* message)
{
        if (!condition)
                throw NsException(message);
}

// Assertions are disabled when the corresponding DEBUG_LEVEL_*
// is not #define'd, to improve performance. The Debug Level 1
// is assumed to be active by default.
#ifdef DEBUG_LEVEL_3
#define DEBUG_LEVEL_2
#define assert_Ns_3(condition, message) (assert_Ns((condition), (message)))
#else
// Level 3 assertions are disabled.
#define assert_Ns_3(condition, message)
#endif

#ifdef DEBUG_LEVEL_2
#define assert_Ns_2(condition, message) (assert_Ns((condition), (message)))
#define assert_Ns_2_run(condition, message)                                    \
        (assert_Ns_2((condition), (message)))
#else
// Level 2 assertions are disabled.
#define assert_Ns_2(condition, message)
// assert_Ns_2_run always executes condition (as a command).
#define assert_Ns_2_run(condition, message) (condition)
#endif

/// @{
/// @name Naxos Solver types and their limits

typedef long NsInt;
typedef unsigned long NsUInt;

const NsInt NsMINUS_INF = LONG_MIN;
const NsInt NsPLUS_INF = LONG_MAX;
const NsUInt NsUPLUS_INF = ULONG_MAX;

/// @}

class NsIntVar;
class NsIntVarArray;

template <typename TemplType>
class NsDeque;

/// An abstract (algebraic etc.) expression between constrained variables
class Ns_Expression {

    public:
        /// Posts constraint VarX == *this; *this represents an Ns_Expression
        virtual void post(NsIntVar& VarX) const = 0;

        /// Produces/returns a variable to represent the Ns_Expression(*this)
        virtual NsIntVar& post(void) const = 0;

        /// The destructor of an abstract class should be virtual
        virtual ~Ns_Expression(void)
        {
        }
};

class Ns_ExprElement : public Ns_Expression {

    private:
        NsIntVar& VarIndex;
        const NsDeque<NsInt>& intArray;

    public:
        Ns_ExprElement(NsIntVar& VarIndex_init,
                       const NsDeque<NsInt>& intArray_init)
          : VarIndex(VarIndex_init), intArray(intArray_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

class Ns_ExprVarArrElement : public Ns_Expression {

    private:
        NsIntVarArray& VarArr;
        NsIntVar& VarIndex;

    public:
        Ns_ExprVarArrElement(NsIntVarArray& VarArr_init,
                             NsIntVar& VarIndex_init)
          : VarArr(VarArr_init), VarIndex(VarIndex_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

/// A flexible array data structure, like std::deque
///
/// A safer version of std::deque that throws an exception when
/// an out_of_range request is being made. It takes advantage of
/// the internal at() method that does this work.
template <typename TemplType>
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

        Ns_ExprElement operator[](const Ns_Expression& VarIndexExpr) const
        {
                return (*this)[VarIndexExpr.post()];
        }

        typename std::deque<TemplType>::reference front(void)
        {
                if (std::deque<TemplType>::empty())
                        throw std::out_of_range("NsDeque::front: Empty deque");
                return std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::const_reference front(void) const
        {
                if (std::deque<TemplType>::empty())
                        throw std::out_of_range("NsDeque::front: Empty deque");
                return std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::reference back(void)
        {
                if (std::deque<TemplType>::empty())
                        throw std::out_of_range("NsDeque::back: Empty deque");
                return std::deque<TemplType>::back();
        }

        typename std::deque<TemplType>::const_reference back(void) const
        {
                if (std::deque<TemplType>::empty())
                        throw std::out_of_range("NsDeque::back: Empty deque");
                return std::deque<TemplType>::back();
        }
};

/// std::queue with exceptions enabled
template <typename TemplType>
class NsQueue : public std::queue<TemplType> {

    public:
        void clear(void)
        {
                while (!std::queue<TemplType>::empty())
                        std::queue<TemplType>::pop();
        }

        typename std::queue<TemplType>::reference front(void)
        {
                if (std::queue<TemplType>::empty())
                        throw std::out_of_range("NsQueue::front: Empty queue");
                return std::queue<TemplType>::front();
        }

        typename std::queue<TemplType>::const_reference front(void) const
        {
                if (std::queue<TemplType>::empty())
                        throw std::out_of_range("NsQueue::front: Empty queue");
                return std::queue<TemplType>::front();
        }

        typename std::queue<TemplType>::reference back(void)
        {
                if (std::queue<TemplType>::empty())
                        throw std::out_of_range("NsQueue::back: Empty queue");
                return std::queue<TemplType>::back();
        }

        typename std::queue<TemplType>::const_reference back(void) const
        {
                if (std::queue<TemplType>::empty())
                        throw std::out_of_range("NsQueue::back: Empty queue");
                return std::queue<TemplType>::back();
        }

        void pop(void)
        {
                if (std::queue<TemplType>::empty())
                        throw std::out_of_range("NsQueue::pop: Empty queue");
                std::queue<TemplType>::pop();
        }
};

/// std::list with exceptions enabled
template <typename TemplType>
class NsList : public std::list<TemplType> {

    public:
        NsList(void)
        {
        }

        NsList(TemplType item)
        {
                this->push_back(item);
        }

        NsList(TemplType item1, TemplType item2)
        {
                this->push_back(item1);
                this->push_back(item2);
        }

        typename std::list<TemplType>::reference front(void)
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range("NsList::front: Empty list");
                return std::list<TemplType>::front();
        }

        typename std::list<TemplType>::const_reference front(void) const
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range("NsList::front: Empty list");
                return std::list<TemplType>::front();
        }

        typename std::list<TemplType>::reference back(void)
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range("NsList::back: Empty list");
                return std::list<TemplType>::back();
        }

        typename std::list<TemplType>::const_reference back(void) const
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range("NsList::back: Empty list");
                return std::list<TemplType>::back();
        }

        void pop_front(void)
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range(
                            "NsList::pop_front: Empty list");
                std::list<TemplType>::pop_front();
        }

        void pop_back(void)
        {
                if (std::list<TemplType>::empty())
                        throw std::out_of_range("NsList::pop_back: Empty list");
                std::list<TemplType>::pop_back();
        }
};

/// An unordered_set
template <typename TemplType>
class NsSet : public Ns_UNORDERED_SET<TemplType> {
};

#include "stack.h"

typedef size_t Ns_pointer_t;

typedef NsDeque<NsIntVar*> Ns_PointArray_t;

/// Array index type
typedef Ns_PointArray_t::size_type NsIndex;

/// Array index maximum size
const NsIndex NsINDEX_INF = Ns_PointArray_t().max_size();

/// The Ns_HistoryId_t of an object can be used to see whether it is valid
///
/// Each frame of the Ns_StackSearch
/// NsProblemManager::searchNodes is represented by its level.
/// Each level has its own valid id that stops being valid when
/// the frame is popped.
struct Ns_HistoryId_t {

        /// The depth of the node in the search tree
        NsIndex level;

        /// The identity of the search node
        NsUInt id;
};

class NsProblemManager;

class Ns_QueueItem;

/// Class describing the domain of a constrained variable as a bit-set
///
/// A bit-set is used to hold its values. If the i-th bit is
/// active, then the domain contains the value minDom + i.
class Ns_BitSet {

    private:
        /// The problem manager to which the domain belongs to
        NsProblemManager* pm;

        /// The initial minimum value of the domain
        NsInt minDom;

        /// Minimum value of the domain
        NsInt minVal;

        /// Maximum value of the domain
        NsInt maxVal;

        /// Number of the bits (active or inactive) of the bit-set
        NsUInt nBits;

        /// Number of the active bits (values) of the domain
        NsUInt setCount;

        /// An array consisting of machine words: the bits for the bitset
        NsDeque<size_t> machw;

        /// The number of bits that a machine word can hold
        static const NsUInt MW_BITS = CHAR_BIT * sizeof(size_t);

        /// The 'timestamp' that can be used in chronological backtracking
        Ns_HistoryId_t lastSaveId;

    public:
        /// Returns the 'lastSaveId'
        Ns_HistoryId_t& lastSaveHistoryId(void)
        {
                return lastSaveId;
        }

        Ns_BitSet(void)
        {
        }

        Ns_BitSet(NsProblemManager& pm_init, const NsInt minDom_init,
                  const NsInt maxDom_init);

        Ns_BitSet* clone(void)
        {
                return new Ns_BitSet(*this);
        }

        NsUInt size(void) const
        {
                return setCount;
        }

        bool isBound(void) const
        {
                return (min() == max());
        }

        NsInt previous(const NsInt toVal) const;

        NsInt next(const NsInt toVal) const;

        NsInt nextGap(const NsInt toVal) const;

        NsInt max(void) const
        {
                return maxVal;
        }

        NsInt min(void) const
        {
                return minVal;
        }

        bool containsRange(const NsInt rangeMin, const NsInt rangeMax) const;

        bool removeRange(NsInt rangeMin, NsInt rangeMax);

        /// Generic function to print a domain
        friend std::ostream& operator<<(std::ostream& os,
                                        const Ns_BitSet& domain);

        /// Iterates through the values of the domain, without changing them
        class const_iterator {

            private:
                const Ns_BitSet* domain;
                NsInt currVal;

            public:
                const_iterator(void) : domain(0)
                {
                }

                const_iterator(const Ns_BitSet& domain_init)
                  : domain(&domain_init), currVal(domain->min())
                {
                }

                bool operator==(const const_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::==: "
                                               "Uninitialized '*this'");
                        return (currVal == b.currVal);
                }

                bool operator!=(const const_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::!=: "
                                               "Uninitialized '*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::*: "
                                               "Uninitialized '*this'");
                        assert_Ns(currVal != NsPLUS_INF,
                                  "Ns_BitSet::const_iterator::*: Bad request "
                                  "'*(something.end())'");
                        return currVal;
                }

                const_iterator& end(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::end:"
                                               " Uninitialized '*this'");
                        currVal = NsPLUS_INF;
                        return *this;
                }

                const_iterator& operator++(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::++: "
                                               "Uninitialized '*this'");
                        currVal = domain->next(currVal);
                        return *this;
                }

                const_iterator& operator--(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_iterator::--: "
                                               "Uninitialized '*this'");
                        currVal = domain->previous(currVal);
                        return *this;
                }
        };

        /// Iterates reversely through the domain values, without changing them
        class const_reverse_iterator {

            private:
                const Ns_BitSet* domain;
                NsInt currVal;

            public:
                const_reverse_iterator(void) : domain(0)
                {
                }

                const_reverse_iterator(const Ns_BitSet& domain_init)
                  : domain(&domain_init), currVal(domain->max())
                {
                }

                bool operator==(const const_reverse_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::==: Uninitialized "
                                               "'*this'");
                        return (currVal == b.currVal);
                }

                bool operator!=(const const_reverse_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::!=: Uninitialized "
                                               "'*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::*: Uninitialized "
                                               "'*this'");
                        assert_Ns(currVal != NsPLUS_INF,
                                  "Ns_BitSet::const_reverse_iterator::*: Bad "
                                  "request '*(something.end())'");
                        return currVal;
                }

                const_reverse_iterator& end(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::end: Uninitialized "
                                               "'*this'");
                        currVal = NsMINUS_INF;
                        return *this;
                }

                const_reverse_iterator& operator++(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::++: Uninitialized "
                                               "'*this'");
                        currVal = domain->previous(currVal);
                        return *this;
                }

                const_reverse_iterator& operator--(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_reverse_"
                                               "iterator::--: Uninitialized "
                                               "'*this'");
                        currVal = domain->next(currVal);
                        return *this;
                }
        };

        /// Iterates through all the missing values (gaps) of the domain
        class const_gap_iterator {

            private:
                const Ns_BitSet* domain;
                NsInt currGap;

            public:
                const_gap_iterator(void) : domain(0)
                {
                }

                const_gap_iterator(const Ns_BitSet& domain_init)
                  : domain(&domain_init),
                    currGap(domain->nextGap(domain->min()))
                {
                }

                bool operator==(const const_gap_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_gap_iterator::"
                                               "==: Uninitialized '*this'");
                        return (currGap == b.currGap);
                }

                bool operator!=(const const_gap_iterator& b) const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_gap_iterator::"
                                               "!=: Uninitialized '*this'");
                        return !(*this == b);
                }

                NsInt operator*(void)const
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_gap_iterator::"
                                               "*: Uninitialized '*this'");
                        assert_Ns(currGap != NsPLUS_INF,
                                  "Ns_BitSet::const_gap_iterator::*: Bad "
                                  "request '*(something.end())'");
                        return currGap;
                }

                const_gap_iterator& end(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_gap_iterator::"
                                               "end: Uninitialized '*this'");
                        currGap = NsPLUS_INF;
                        return *this;
                }

                const_gap_iterator& operator++(void)
                {
                        assert_Ns(domain != 0, "Ns_BitSet::const_gap_iterator::"
                                               "++: Uninitialized '*this'");
                        currGap = domain->nextGap(currGap);
                        return *this;
                }
        };

        const const_iterator begin(void) const
        {
                return const_iterator(*this);
        }

        const const_iterator end(void) const
        {
                const_iterator iterEnd(*this);
                return iterEnd.end();
        }

        const const_reverse_iterator rbegin(void) const
        {
                return const_reverse_iterator(*this);
        }

        const const_reverse_iterator rend(void) const
        {
                const_reverse_iterator iterEnd(*this);
                return iterEnd.end();
        }

        const const_gap_iterator gap_begin(void) const
        {
                return const_gap_iterator(*this);
        }

        const const_gap_iterator gap_end(void) const
        {
                const_gap_iterator iterEnd(*this);
                return iterEnd.end();
        }
};

std::ostream& operator<<(std::ostream& os, const Ns_BitSet& domain);

class Ns_Constraint;
typedef NsDeque<Ns_Constraint*> Ns_constraints_array_t;

class Ns_Expression;

/// A constrained variable
class NsIntVar {

    private:
        /// Each constrained variable belongs to a specific NsProblemManager
        NsProblemManager* pm;

        /// The domain of the constrained variable
        Ns_BitSet domain;

    public:
        /// @{
        /// @name Iterators

        typedef Ns_BitSet::const_iterator const_iterator;

        const const_iterator begin(void) const
        {
                return const_iterator(domain);
        }

        const const_iterator end(void) const
        {
                const_iterator iterEnd(domain);
                return iterEnd.end();
        }

        typedef Ns_BitSet::const_reverse_iterator const_reverse_iterator;

        const const_reverse_iterator rbegin(void) const
        {
                return const_reverse_iterator(domain);
        }

        const const_reverse_iterator rend(void) const
        {
                const_reverse_iterator iterEnd(domain);
                return iterEnd.end();
        }

        typedef Ns_BitSet::const_gap_iterator const_gap_iterator;

        const const_gap_iterator gap_begin(void) const
        {
                return const_gap_iterator(domain);
        }

        const const_gap_iterator gap_end(void) const
        {
                const_gap_iterator iterEnd(domain);
                return iterEnd.end();
        }

        /// @}

        /// Dummy constructor for the declaration of an uninitialized NsIntVar
        NsIntVar(void)
          : pm(0),
            arcsConnectedTo(0),
            constraintNeedsRemovedValues(false),
            queueItem(0)
        {
        }

        NsIntVar(NsProblemManager& pm_init, const NsInt min_init,
                 const NsInt max_init);

        NsIntVar(const Ns_Expression& expr);

        NsIntVar& operator=(const Ns_Expression& expr);

        /// @{
        /// @name Methods that remove values from the domain

        void removeAll(void);

        void remove(const NsInt val)
        {
                return remove(val, val);
        }

        /// Removes a value, plus recording the constraint that called it
        ///
        /// If c == 0, no constraint is recorded.
        bool removeSingle(const NsInt val, const Ns_Constraint* c)
        {
                return removeRange(val, val, c);
        }

        void remove(const NsInt first, const NsInt last)
        {
                removeRange(first, last, 0);
        }

        bool removeRange(const NsInt first, const NsInt last,
                         const Ns_Constraint* c)
        {
                bool modifiedFoo;
                return removeRange(first, last, c, modifiedFoo);
        }

        bool removeRange(const NsInt first, const NsInt last,
                         const Ns_Constraint* c, bool& modified);

        /// @}

        /// Assigns a value to the constrained variable
        void set(const NsInt val)
        {
                assert_Ns(val != NsMINUS_INF && val != NsPLUS_INF,
                          "NsIntVar::set: Cannot assign infinity");
                remove(NsMINUS_INF, val - 1);
                remove(val + 1, NsPLUS_INF);
        }

        NsUInt size(void) const
        {
                return domain.size();
        }

        NsInt previous(const NsInt toVal) const
        {
                return domain.previous(toVal);
        }

        NsInt next(const NsInt toVal) const
        {
                return domain.next(toVal);
        }

        NsInt min(void) const
        {
                return domain.min();
        }

        NsInt max(void) const
        {
                return domain.max();
        }

        /// If the constrained variable is instantiated, this returns its value
        NsInt value(void) const
        {
                assert_Ns(isBound(), "NsIntVar::value: Called for an "
                                     "uninstantiated variable");
                return domain.min();
        }

        bool isBound(void) const
        {
                return domain.isBound();
        }

        bool contains(const NsInt val) const
        {
                return contains(val, val);
        }

        bool contains(const NsInt first, const NsInt last) const
        {
                return domain.containsRange(first, last);
        }

        friend std::ostream& operator<<(std::ostream& os, const NsIntVar& Var);

        /// The NsProblemManager that the variable belongs to
        NsProblemManager& manager(void) const
        {
                return *pm;
        }

        /// @{
        /// @name Auxiliary AC algorithm data-members

    public:
        /// Pair of a constraint and the inconsistencies that has provoked
        struct ConstraintAndFailure {

                /// The constraint
                Ns_Constraint* constr;

                /// The inconsistencies that constr provoked
                unsigned long failures;

                /// Constructor
                ConstraintAndFailure(Ns_Constraint* constr_init)
                  : constr(constr_init), failures(0)
                {
                }
        };

        /// An array of the constraints that the variable is involved in
        NsDeque<ConstraintAndFailure> constraints;

    private:
        /// The number of other variables connected to this via constraints
        int arcsConnectedTo;

        /// True, if the variable is involved in an 'Inverse' constraint, or
        /// another constraint that needs to know the values that have been
        /// removed from the variable (the w's in the AC-5 Algorithm)
        bool constraintNeedsRemovedValues;

    public:
        /// Returns true if the variable is involved in an 'Inverse' constraint,
        /// or another constraint that needs to know the values that have been
        /// removed from the variable
        bool storeRemovedValues(void) const
        {
                return constraintNeedsRemovedValues;
        }

        /// Adds a constraint to the collection of constraints of the variable
        void addConstraint(Ns_Constraint* c);

        /// @}

        /// Points to the item in the AC queue that refers to the variable
        ///
        /// If there is no such item, the pointer is null.
        Ns_QueueItem* queueItem;

        void transparent(void);
};

inline std::ostream& operator<<(std::ostream& os, const NsIntVar& Var)
{
        return (os << Var.domain);
}

class Ns_ExpressionArray;

/// A flexible array data type, to hold constrained variables (NsIntVar's)
class NsIntVarArray {

    private:
        Ns_PointArray_t PointArray;

        bool addedConstraint;

    public:
        NsIntVarArray(void) : addedConstraint(false)
        {
        }

        NsIntVarArray(const Ns_ExpressionArray& expr);

        NsIntVarArray& operator=(const Ns_ExpressionArray& expr);

        /// Signifies that a constraint has been imposed on the array
        void addConstraint(void)
        {
                addedConstraint = true;
        }

        void push_front(const NsIntVar& Var);

        void push_front(NsIntVar& Var)
        {
                assert_Ns(!addedConstraint, "NsIntVarArray::push_front: Cannot "
                                            "add another variable, because a "
                                            "constraint has been already "
                                            "imposed on the array");
                PointArray.push_front(&Var);
        }

        void push_front(const Ns_Expression& expr);

        void push_back(const NsIntVar& Var);

        void push_back(NsIntVar& Var)
        {
                assert_Ns(!addedConstraint, "NsIntVarArray::push_back: Cannot "
                                            "add another variable, because a "
                                            "constraint has been already "
                                            "imposed on the array");
                PointArray.push_back(&Var);
        }

        void push_back(const Ns_Expression& expr);

        class const_iterator;

        /// Iterator that iterates through the array's constrained variables
        class iterator {

            private:
                Ns_PointArray_t* PointArr;

                Ns_PointArray_t::iterator it;

            public:
                friend class const_iterator;

                iterator(void) : PointArr(0)
                {
                }

                iterator(Ns_PointArray_t& PointArr_init)
                  : PointArr(&PointArr_init), it(PointArr->begin())
                {
                }

                bool operator==(const iterator& b) const
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::iterator::==: "
                                                 "Uninitialized '*this'");
                        return (it == b.it);
                }

                bool operator!=(const iterator& b) const
                {
                        return !(*this == b);
                }

                NsIntVar& operator*(void)const
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::iterator::*: "
                                                 "Uninitialized '*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::*: Bad request "
                                  "'*(something.end())'");
                        return **it;
                }

                NsIntVar* operator->(void)const
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::iterator::*: "
                                                 "Uninitialized '*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::*: Bad request "
                                  "'*(something.end())'");
                        return *it;
                }

                iterator& end(void)
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::iterator::end:"
                                                 " Uninitialized '*this'");
                        it = PointArr->end();
                        return *this;
                }

                iterator& operator++(void)
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::iterator::++: "
                                                 "Uninitialized '*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::end: Bad request "
                                  "'++(something.end())'");
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
                iterator iterEnd(PointArray);
                return iterEnd.end();
        }

        /// Iterator that goes through the array without modifying its elements
        class const_iterator {

            private:
                const Ns_PointArray_t* PointArr;

                Ns_PointArray_t::const_iterator it;

            public:
                const_iterator(void) : PointArr(0)
                {
                }

                const_iterator(const Ns_PointArray_t& PointArr_init)
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
                        assert_Ns(PointArr != 0, "NsIntVarArray::const_"
                                                 "iterator::==: Uninitialized "
                                                 "'*this'");
                        return (it == b.it);
                }

                bool operator!=(const const_iterator& b) const
                {
                        return !(*this == b);
                }

                const NsIntVar& operator*(void)const
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::const_"
                                                 "iterator::*: Uninitialized "
                                                 "'*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::*: Bad "
                                  "request '*(something.end())'");
                        return **it;
                }

                const NsIntVar* operator->(void)const
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::const_"
                                                 "iterator::*: Uninitialized "
                                                 "'*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::*: Bad "
                                  "request '*(something.end())'");
                        return *it;
                }

                const_iterator& end(void)
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::const_"
                                                 "iterator::end: Uninitialized "
                                                 "'*this'");
                        it = PointArr->end();
                        return *this;
                }

                const_iterator& operator++(void)
                {
                        assert_Ns(PointArr != 0, "NsIntVarArray::const_"
                                                 "iterator::++: Uninitialized "
                                                 "'*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::end: Bad "
                                  "request '++(something.end())'");
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
                const_iterator iterEnd(PointArray);
                return iterEnd.end();
        }

        NsIntVar& operator[](const NsIndex i)
        {
                return *PointArray[i];
        }

        const NsIntVar& operator[](const NsIndex i) const
        {
                return *PointArray[i];
        }

        Ns_ExprVarArrElement operator[](NsIntVar& VarIndex)
        {
                return Ns_ExprVarArrElement(*this, VarIndex);
        }

        Ns_ExprVarArrElement operator[](const Ns_Expression& VarIndexExpr)
        {
                return (*this)[VarIndexExpr.post()];
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

void Ns_ternaryConstraintToGraphFile(std::ofstream& fileConstraintsGraph,
                                     const NsIntVar* VarX, const NsIntVar* VarY,
                                     const NsIntVar* VarZ,
                                     const Ns_Constraint* constr,
                                     const char* constrName,
                                     const bool sourceLabels);

void Ns_globalConstraintToGraphFile(std::ofstream& fileConstraintsGraph,
                                    const NsIntVar* VarX,
                                    const NsIntVarArray* VarArr,
                                    const Ns_Constraint* constr,
                                    const char* constrName);

void Ns_elementConstraintToGraphFile(std::ofstream& fileConstraintsGraph,
                                     const NsIntVar& VarX, const NsIntVar& VarY,
                                     const NsIntVarArray& VarArr,
                                     const Ns_Constraint* constr);

void Ns_arrayConstraintToGraphFile(std::ofstream& fileConstraintsGraph,
                                   const NsIntVarArray* VarArr,
                                   const Ns_Constraint* constr,
                                   const char* constrName);

/// Abstract class that represents a constraint between constrained variables
class Ns_Constraint {

    public:
        /// The 'timestamp' (current removal ID) when the last check of this
        /// constraint took place
        unsigned long lastConstraintCheckTime;

        /// Constructor
        Ns_Constraint(void)
          : lastConstraintCheckTime(0), revisionType(BOUNDS_CONSISTENCY)
        {
        }

        /// @{
        /// @name AC algorithm methods

        virtual void ArcCons(void) = 0;

        virtual void LocalArcCons(Ns_QueueItem& Qitem) = 0;

        /// @}

        /// The number of the variables involved in the constraint
        virtual int varsInvolvedIn(void) const = 0;

        /// Description of the consistency type that a 'revision' function for
        /// a constraint can impose
        enum ConsistencyType {

                /// The revision function needs to know the value that has been
                /// removed from the domain (e.g. see Ns_ConstrInverse).
                VALUE_CONSISTENCY,

                /// The revision function does not need to know the removed
                /// values (like VALUE_CONSISTENCY). It imposes
                /// bounds-consistency.
                BOUNDS_CONSISTENCY,

                /// Like BOUNDS_CONSISTENCY, but revision is imposed in both
                /// directions. E.g. Revision(i,j) is equivalent to
                /// Revision(j,i), where i, j are variables.
                BIDIRECTIONAL_CONSISTENCY
        };

        /// Description of the type of revision function (LocalArcCons) for the
        /// constraint
        ConsistencyType revisionType;

        /// Writes a constraint-edge representation into a graph file, with a
        /// format supported by Graphviz
        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                assert_Ns(
                    fileConstraintsGraph.good(),
                    "Ns_Constraint::toGraphFile: Problem writing to file");
                fileConstraintsGraph
                    << "\n\t"
                    << "// Unimplemented constraint representation\n";
        }

        /// The destructor of an abstract class should be virtual
        virtual ~Ns_Constraint(void)
        {
        }
};

class Ns_ConstrXlessthanY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        Ns_ConstrXlessthanY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXlessthanY::Ns_ConstrXlessthanY: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarX << " -> Var" << VarY
                                     << " [label=\"<\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXlesseqthanY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        Ns_ConstrXlesseqthanY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXlesseqthanY::Ns_ConstrXlesseqthanY: All "
                          "the variables of a constraint must belong to the "
                          "same NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarX << " -> Var" << VarY
                                     << " [label=\"<=\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYplusC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqYplusC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqYplusC::Ns_ConstrXeqYplusC: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"" << ((C >= 0) ? "+" : "")
                                     << C << "\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqCminusY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqCminusY(NsIntVar* X, const NsInt C_init, NsIntVar* Y)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqCminusY::Ns_ConstrXeqCminusY: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"" << C << "-y\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYtimesC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqYtimesC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqYtimesC::Ns_ConstrXeqYtimesC: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(C != 0, "Ns_ConstrXeqYtimesC::Ns_ConstrXeqYtimesC: "
                                  "If C==0 the constraint has no reason to "
                                  "exist");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"*" << ((C >= 0) ? "" : "(")
                                     << C << ((C >= 0) ? "" : ")") << "\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYplusZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrXeqYplusZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYplusZ::Ns_ConstrXeqYplusZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "+", false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYtimesZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrXeqYtimesZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYtimesZ::Ns_ConstrXeqYtimesZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "*", false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYdivZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrXeqYdivZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYdivZ::Ns_ConstrXeqYdivZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "/", false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYdivC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqYdivC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqYdivC::Ns_ConstrXeqYdivC: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(C != 0, "Ns_ConstrXeqYdivC::Ns_ConstrXeqYdivC: "
                                  "Special condition required: C != 0");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"/" << ((C >= 0) ? "" : "(")
                                     << C << ((C >= 0) ? "" : ")") << "\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqCdivY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqCdivY(NsIntVar* X, const NsInt C_init, NsIntVar* Y)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqCdivY::Ns_ConstrXeqCdivY: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                VarY->remove(0);
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"" << C << "/\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYmodZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrXeqYmodZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYmodZ::Ns_ConstrXeqYmodZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "%", false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYmodC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqYmodC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqYmodC::Ns_ConstrXeqYmodC: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(C != 0, "Ns_ConstrXeqYmodC::Ns_ConstrXeqYmodC: "
                                  "Special condition required: C != 0");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"%" << ((C >= 0) ? "" : "(")
                                     << C << ((C >= 0) ? "" : ")") << "\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqCmodY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrXeqCmodY(NsIntVar* X, const NsInt C_init, NsIntVar* Y)
          : VarX(X), VarY(Y), C(C_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqCmodY::Ns_ConstrXeqCmodY: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"" << ((C >= 0) ? "" : "(")
                                     << C << ((C >= 0) ? "" : ")") << "%\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlessthanC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYlessthanC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYlessthanC::Ns_"
                          "ConstrMetaXeqYlessthanC: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y<" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlessthanZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrMetaXeqYlessthanZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrMetaXeqYlessthanZ::Ns_"
                          "ConstrMetaXeqYlessthanZ: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "<", true);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlesseqthanC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYlesseqthanC(NsIntVar* X, NsIntVar* Y,
                                     const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYlesseqthanC::Ns_"
                          "ConstrMetaXeqYlesseqthanC: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y<=" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlesseqthanZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;

    public:
        Ns_ConstrMetaXeqYlesseqthanZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z)
          : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrMetaXeqYlesseqthanZ::Ns_"
                          "ConstrMetaXeqYlesseqthanZ: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(fileConstraintsGraph, VarX,
                                                VarY, VarZ, this, "<=", true);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYgreaterthanC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYgreaterthanC(NsIntVar* X, NsIntVar* Y,
                                      const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYgreaterthanC::Ns_"
                          "ConstrMetaXeqYgreaterthanC: All the variables of a "
                          "constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y>" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYgreatereqthanC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYgreatereqthanC(NsIntVar* X, NsIntVar* Y,
                                        const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYgreatereqthanC::Ns_"
                          "ConstrMetaXeqYgreatereqthanC: All the variables of "
                          "a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y>=" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYeqC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYeqC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYeqC::Ns_ConstrMetaXeqYeqC: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y=" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYeqZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        /// If neg == true, the constraint becomes 'Ns_ConstrMetaXeqY neq Z'
        const bool neg;

    public:
        Ns_ConstrMetaXeqYeqZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z,
                             const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrMetaXeqYeqZ::Ns_ConstrMetaXeqYeqZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(
                    fileConstraintsGraph, VarX, VarY, VarZ, this,
                    ((neg) ? "(y!=z)" : "(y=z)"), false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYneqC : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        NsInt C;

    public:
        Ns_ConstrMetaXeqYneqC(NsIntVar* X, NsIntVar* Y, const NsInt C_init)
          : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrMetaXeqYneqC::Ns_ConstrMetaXeqYneqC: All "
                          "the variables of a constraint must belong to the "
                          "same NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"(y!=" << C << ")\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYandZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        /// If neg == true, the constraint becomes 'Ns_ConstrXeqY nand Z'
        const bool neg;

    public:
        Ns_ConstrXeqYandZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z, const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYandZ::Ns_ConstrXeqYandZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(0 <= VarX->min() && VarX->max() <= 1 &&
                              0 <= VarY->min() && VarY->max() <= 1 &&
                              0 <= VarZ->min() && VarZ->max() <= 1,
                          "Ns_ConstrXeqYandZ::Ns_ConstrXeqYandZ: All the "
                          "variables should be \"boolean\"");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(
                    fileConstraintsGraph, VarX, VarY, VarZ, this,
                    ((neg) ? "(y NAND z)" : "(y AND z)"), false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYorZ : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY, *VarZ;
        /// If neg == true, the constraint becomes 'Ns_ConstrXeqY nor Z'
        const bool neg;

    public:
        Ns_ConstrXeqYorZ(NsIntVar* X, NsIntVar* Y, NsIntVar* Z, const bool pos)
          : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns(&VarX->manager() == &VarY->manager() &&
                              &VarY->manager() == &VarZ->manager(),
                          "Ns_ConstrXeqYorZ::Ns_ConstrXeqYorZ: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(0 <= VarX->min() && VarX->max() <= 1 &&
                              0 <= VarY->min() && VarY->max() <= 1 &&
                              0 <= VarZ->min() && VarZ->max() <= 1,
                          "Ns_ConstrXeqYorZ::Ns_ConstrXeqYorZ: All the "
                          "variables should be \"boolean\"");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 3;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_ternaryConstraintToGraphFile(
                    fileConstraintsGraph, VarX, VarY, VarZ, this,
                    ((neg) ? "(y NOR z)" : "(y OR z)"), false);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXorY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;
        /// If neg == true, the constraint becomes 'Constr not X and not Y'
        const bool neg;

    public:
        Ns_ConstrXorY(NsIntVar* X, NsIntVar* Y, const bool pos)
          : VarX(X), VarY(Y), neg(!pos)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXorY::Ns_ConstrXorY: All the variables of "
                          "a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(0 <= VarX->min() && VarX->max() <= 1 &&
                              0 <= VarY->min() && VarY->max() <= 1,
                          "Ns_ConstrXorY::Ns_ConstrXorY: All the variables "
                          "should be \"boolean\"");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarX << " -> Var" << VarY
                                     << " [label=\"" << ((neg) ? "NOR" : "OR")
                                     << "\", arrowhead=none];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqMin : public Ns_Constraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;

    public:
        Ns_ConstrXeqMin(NsIntVar* X, NsIntVarArray* VarArr_init);

        virtual int varsInvolvedIn(void) const
        {
                return (1 + VarArr->size());
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_globalConstraintToGraphFile(fileConstraintsGraph, VarX,
                                               VarArr, this, "min");
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqMax : public Ns_Constraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;

    public:
        Ns_ConstrXeqMax(NsIntVar* X, NsIntVarArray* VarArr_init);

        virtual int varsInvolvedIn(void) const
        {
                return (1 + VarArr->size());
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_globalConstraintToGraphFile(fileConstraintsGraph, VarX,
                                               VarArr, this, "max");
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqSum : public Ns_Constraint {

    private:
        NsIntVar* VarX;
        NsIntVarArray* VarArr;
        NsIndex start, length;

    public:
        Ns_ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init);
        Ns_ConstrXeqSum(NsIntVar* X, NsIntVarArray* VarArr_init,
                        const NsIndex start_init, const NsIndex length_init);

        virtual int varsInvolvedIn(void) const
        {
                return (1 + length);
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_globalConstraintToGraphFile(fileConstraintsGraph, VarX,
                                               VarArr, this, "sum");
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        Ns_ConstrXeqY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqY::Ns_ConstrXeqY: All the variables of "
                          "a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarX << " -> Var" << VarY
                                     << " [arrowhead=none];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXneqY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        Ns_ConstrXneqY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXneqY::Ns_ConstrXneqY: All the variables "
                          "of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarX << " -> Var" << VarY
                                     << " [label=\"!=\", arrowhead=none];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqAbsY : public Ns_Constraint {

    private:
        NsIntVar *VarX, *VarY;

    public:
        Ns_ConstrXeqAbsY(NsIntVar* X, NsIntVar* Y) : VarX(X), VarY(Y)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarX->manager() == &VarY->manager(),
                          "Ns_ConstrXeqAbsY::cononstrXeqAbsY: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarY << " -> Var" << VarX
                                     << " [label=\"abs\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrAllDiff : public Ns_Constraint {

    private:
        NsIntVarArray* VarArr;

    public:
        Ns_ConstrAllDiff(NsIntVarArray* VarArr_init);

        virtual int varsInvolvedIn(void) const
        {
                return VarArr->size();
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_arrayConstraintToGraphFile(fileConstraintsGraph, VarArr,
                                              this, "!=");
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrAllDiffStrong : public Ns_Constraint {

    public:
        /// Extends 'NsIntVar' by adding the information concerning the id of
        /// the group that the constrained variable belongs to
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

                group_t group(void)
                {
                        return vGroup.min();
                }

                void setGroup(const group_t groupVal)
                {
                        assert_Ns(vGroup.contains(groupVal - 1),
                                  "groupedNsIntVar::setGroup: 'groupVal-1' is "
                                  "not contained in 'vGroup'");
                        assert_Ns(
                            vGroup.removeRange(NsMINUS_INF, groupVal - 1, 0),
                            "groupedNsIntVar::setGroup: Could not change group "
                            "to 'groupVal'");
                        assert_Ns(group() == groupVal,
                                  "groupedNsIntVar::setGroup: Not succesful "
                                  "change of group to 'groupVal'");
                }

                bool removeDomain(const NsIntVar& V, const Ns_Constraint* c);
        };

    private:
        NsDeque<groupedNsIntVar> VarArr;

        typedef Ns_UNORDERED_MAP<Ns_pointer_t, groupedNsIntVar*>
            VarPointerGroup_t;

        VarPointerGroup_t VarPointerGroup;

        const unsigned long Capacity;

    public:
        Ns_ConstrAllDiffStrong(NsIntVarArray* VarArr_init,
                               const unsigned long Cap);

        virtual int varsInvolvedIn(void) const
        {
                return VarArr.size();
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrTable : public Ns_Constraint {

    private:
        NsIntVarArray& VarArr;
        const NsDeque<NsDeque<NsInt>>& table;
        const bool isSupportsTable;

    public:
        Ns_ConstrTable(NsIntVarArray& VarArr_init,
                       const NsDeque<NsDeque<NsInt>>& table_init,
                       const bool isSupportsTable_init);

        virtual int varsInvolvedIn(void) const
        {
                return VarArr.size();
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_arrayConstraintToGraphFile(fileConstraintsGraph, &VarArr,
                                              this, "table");
        }

        virtual void ArcCons(void);
        virtual void ArcConsSupports(void);
        virtual void ArcConsConflicts(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrElement : public Ns_Constraint {

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
                revisionType = VALUE_CONSISTENCY;
                assert_Ns(&VarIndex->manager() == &VarValue->manager(),
                          "Ns_ConstrElement::Ns_ConstrElement: All the "
                          "variables of a constraint must belong to the same "
                          "NsProblemManager");
                assert_Ns(
                    !intArray.empty(),
                    "Ns_ConstrElement::Ns_ConstrElement: Empty element array");
        }

        virtual int varsInvolvedIn(void) const
        {
                return 2;
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                fileConstraintsGraph << "\n\tVar" << VarValue << " -> Var"
                                     << VarIndex << " [label=\"element\"];\n";
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrVarArrElement : public Ns_Constraint {

    private:
        NsIntVarArray& VarArr;
        NsIntVar& VarIndex;
        NsIntVar& VarValue;

    public:
        Ns_ConstrVarArrElement(NsIntVarArray& VarArr_init,
                               NsIntVar& VarIndex_init, NsIntVar& VarValue_init)
          : VarArr(VarArr_init),
            VarIndex(VarIndex_init),
            VarValue(VarValue_init)
        {
                revisionType = BIDIRECTIONAL_CONSISTENCY;
                assert_Ns(&VarIndex.manager() == &VarValue.manager(),
                          "Ns_ConstrVarArrElement::Ns_ConstrVarArrElement: All "
                          "the variables of a constraint must belong to the "
                          "same NsProblemManager");
                for (NsIntVarArray::iterator V = VarArr.begin();
                     V != VarArr.end(); ++V) {
                        assert_Ns(&VarIndex.manager() == &V->manager(),
                                  "Ns_ConstrVarArrElement::Ns_"
                                  "ConstrVarArrElement: All the variables of a "
                                  "constraint must belong to the same "
                                  "NsProblemManager");
                }
                assert_Ns(!VarArr.empty(),
                          "Ns_ConstrVarArrElement::Ns_ConstrVarArrElement: "
                          "Empty element array");
        }

        virtual int varsInvolvedIn(void) const
        {
                return (VarArr.size() + 2);
        }

        virtual void toGraphFile(std::ofstream& fileConstraintsGraph) const
        {
                Ns_elementConstraintToGraphFile(fileConstraintsGraph, VarIndex,
                                                VarValue, VarArr, this);
        }

        virtual void ArcCons(void);
        virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ExprYplusC : public Ns_Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprYplusC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYplusC operator+(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprYplusC(Y, C);
}

inline Ns_ExprYplusC operator+(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() + C);
}

inline Ns_ExprYplusC operator+(const NsInt C, const Ns_Expression& Yexpr)
{
        return (Yexpr.post() + C);
}

inline Ns_ExprYplusC operator+(const NsInt C, NsIntVar& Y)
{
        return (Y + C);
}

inline Ns_ExprYplusC operator-(NsIntVar& Y, const NsInt C)
{
        return (Y + -C);
}

inline Ns_ExprYplusC operator-(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() - C);
}

class Ns_ExprYdivC : public Ns_Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprYdivC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYdivC operator/(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprYdivC(Y, C);
}

inline Ns_ExprYdivC operator/(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() / C);
}

class Ns_ExprYplusZ : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprYplusZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYplusZ operator+(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprYplusZ(Y, Z);
}

inline Ns_ExprYplusZ operator+(const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() + Z);
}

inline Ns_ExprYplusZ operator+(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y + Zexpr.post());
}

inline Ns_ExprYplusZ operator+(const Ns_Expression& Yexpr,
                               const Ns_Expression& Zexpr)
{
        return (Yexpr.post() + Zexpr.post());
}

class Ns_ExprYminusZ : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprYminusZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYminusZ operator-(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprYminusZ(Y, Z);
}

inline Ns_ExprYminusZ operator-(const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() - Z);
}

inline Ns_ExprYminusZ operator-(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y - Zexpr.post());
}

inline Ns_ExprYminusZ operator-(const Ns_Expression& Yexpr,
                                const Ns_Expression& Zexpr)
{
        return (Yexpr.post() - Zexpr.post());
}

class Ns_ExprCminusZ : public Ns_Expression {

    private:
        NsInt C;
        NsIntVar& VarY;

    public:
        Ns_ExprCminusZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarY(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprCminusZ operator-(const NsInt C, NsIntVar& Z)
{
        return Ns_ExprCminusZ(C, Z);
}

inline Ns_ExprCminusZ operator-(const NsInt C, const Ns_Expression& Zexpr)
{
        return (C - Zexpr.post());
}

class Ns_ExprYtimesZ : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprYtimesZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYtimesZ operator*(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprYtimesZ(Y, Z);
}

inline Ns_ExprYtimesZ operator*(const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() * Z);
}

inline Ns_ExprYtimesZ operator*(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y * Zexpr.post());
}

inline Ns_ExprYtimesZ operator*(const Ns_Expression& Yexpr,
                                const Ns_Expression& Zexpr)
{
        return (Yexpr.post() * Zexpr.post());
}

class Ns_ExprYtimesC : public Ns_Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprYtimesC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYtimesC operator*(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprYtimesC(Y, C);
}

inline Ns_ExprYtimesC operator*(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() * C);
}

inline Ns_ExprYtimesC operator*(const NsInt C, NsIntVar& Y)
{
        return (Y * C);
}

inline Ns_ExprYtimesC operator*(const NsInt C, const Ns_Expression& Yexpr)
{
        return (Yexpr.post() * C);
}

inline Ns_ExprCminusZ operator-(NsIntVar& Y)
{
        return (0 - Y);
}

inline Ns_ExprCminusZ operator-(const Ns_Expression& Yexpr)
{
        return (0 - Yexpr.post());
}

class Ns_ExprYdivZ : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprYdivZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYdivZ operator/(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprYdivZ(Y, Z);
}

inline Ns_ExprYdivZ operator/(const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() / Z);
}

inline Ns_ExprYdivZ operator/(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y / Zexpr.post());
}

inline Ns_ExprYdivZ operator/(const Ns_Expression& Yexpr,
                              const Ns_Expression& Zexpr)
{
        return (Yexpr.post() / Zexpr.post());
}

class Ns_ExprCdivZ : public Ns_Expression {

    private:
        NsInt C;
        NsIntVar& VarZ;

    public:
        Ns_ExprCdivZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprCdivZ operator/(const NsInt C, NsIntVar& Z)
{
        return Ns_ExprCdivZ(C, Z);
}

inline Ns_ExprCdivZ operator/(const NsInt C, const Ns_Expression& Zexpr)
{
        return (C / Zexpr.post());
}

class Ns_ExprYmodZ : public Ns_Expression {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprYmodZ(NsIntVar& Y, NsIntVar& Z) : VarY(Y), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYmodZ operator%(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprYmodZ(Y, Z);
}

inline Ns_ExprYmodZ operator%(const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return (Yexpr.post() % Z);
}

inline Ns_ExprYmodZ operator%(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y % Zexpr.post());
}

inline Ns_ExprYmodZ operator%(const Ns_Expression& Yexpr,
                              const Ns_Expression& Zexpr)
{
        return (Yexpr.post() % Zexpr.post());
}

class Ns_ExprYmodC : public Ns_Expression {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprYmodC(NsIntVar& Y, const NsInt C_init) : VarY(Y), C(C_init)
        {
                // (a % b) is equivalent to (a % (-b))
                if (C < 0)
                        C = -C;
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprYmodC operator%(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprYmodC(Y, C);
}

inline Ns_ExprYmodC operator%(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() % C);
}

class Ns_ExprCmodZ : public Ns_Expression {

    private:
        NsInt C;
        NsIntVar& VarZ;

    public:
        Ns_ExprCmodZ(const NsInt C_init, NsIntVar& Z) : C(C_init), VarZ(Z)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprCmodZ operator%(const NsInt C, NsIntVar& Z)
{
        return Ns_ExprCmodZ(C, Z);
}

inline Ns_ExprCmodZ operator%(const NsInt C, const Ns_Expression& Zexpr)
{
        return (C % Zexpr.post());
}

class Ns_ExprAbsY : public Ns_Expression {

    private:
        NsIntVar& VarY;

    public:
        Ns_ExprAbsY(NsIntVar& Y) : VarY(Y)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprAbsY NsAbs(NsIntVar& Y)
{
        return Ns_ExprAbsY(Y);
}

inline Ns_ExprAbsY NsAbs(const Ns_Expression& Yexpr)
{
        return NsAbs(Yexpr.post());
}

class Ns_ExprMin : public Ns_Expression {

    private:
        NsIntVarArray& VarArr;

    public:
        Ns_ExprMin(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprMin NsMin(NsIntVarArray& Arr)
{
        return Ns_ExprMin(Arr);
}

class Ns_ExprMax : public Ns_Expression {

    private:
        NsIntVarArray& VarArr;

    public:
        Ns_ExprMax(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprMax NsMax(NsIntVarArray& Arr)
{
        return Ns_ExprMax(Arr);
}

class Ns_ExprSum : public Ns_Expression {

    private:
        NsIntVarArray& VarArr;
        NsIndex start, length;

    public:
        Ns_ExprSum(NsIntVarArray& VarArr_init)
          : VarArr(VarArr_init), start(0), length(VarArr_init.size())
        {
        }

        Ns_ExprSum(NsIntVarArray& VarArr_init, const NsIndex start_init,
                   const NsIndex length_init)
          : VarArr(VarArr_init), start(start_init), length(length_init)
        {
                assert_Ns(start + length <= VarArr.size(),
                          "Ns_ExprSum::Ns_ExprSum: Wrong 'start' or 'length'");
        }

        virtual void post(NsIntVar& VarX) const;
        virtual NsIntVar& post(void) const;
};

inline Ns_ExprSum NsSum(NsIntVarArray& Arr)
{
        return Ns_ExprSum(Arr);
}

inline Ns_ExprSum NsSum(NsIntVarArray& Arr, const NsIndex start,
                        const NsIndex length)
{
        return Ns_ExprSum(Arr, start, length);
}

/// An abstract expression having to do with arrays of constrained variables
class Ns_ExpressionArray {

    public:
        /// Posts the constraint
        virtual void post(NsIntVarArray& VarArr) const = 0;

        /// The destructor of an abstract class should be virtual
        virtual ~Ns_ExpressionArray(void)
        {
        }
};

/// Ns_Expression subcategory describing constraints
///
/// The following abstract class represents the expressions
/// category that can be viewed both as a constraint (e.g. X < Y)
/// and as an expression/meta-constraint (e.g. Z == X < Y).
class Ns_ExprConstr : public Ns_Expression {

    protected:
        /// Positive or negative constraint declaration
        ///
        /// If isPositive == false, then the semantics
        /// of the constraint-expression is inverted.
        /// E.g. 'X < Y' becomes 'X >= Y'.
        bool isPositive;

    public:
        Ns_ExprConstr(const bool isPositive_init) : isPositive(isPositive_init)
        {
        }

        /// Final declaration and post of a constraint
        virtual Ns_Constraint* postConstraint(bool positively = true) const = 0;

        /// Uses postC(VarX, true) to post a constraint
        virtual void post(NsIntVar& VarX) const
        {
                postC(VarX, true);
        }

        /// Uses postC(true) to post a constraint
        virtual NsIntVar& post(void) const
        {
                return postC(true);
        }

        /// Extends Ns_Expression::post(NsIntVar &VarX) const in the way that it
        /// can inverse the semantics of the constraint
        virtual void postC(NsIntVar& VarX, bool positively = true) const = 0;

        /// Extends Ns_Expression::post() in the way that it can inverse the
        /// semantics of the constraint
        virtual NsIntVar& postC(bool positively = true) const = 0;
};

class Ns_ExprConstrYlessthanC : public Ns_ExprConstr {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprConstrYlessthanC(NsIntVar& Y, const NsInt C_init,
                                const bool isPositive_init)
          : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYlesseqthanC : public Ns_ExprConstr {

    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprConstrYlesseqthanC(NsIntVar& Y, const NsInt C_init,
                                  const bool isPositive_init)
          : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYeqC : public Ns_ExprConstr {
    private:
        NsIntVar& VarY;
        NsInt C;

    public:
        Ns_ExprConstrYeqC(NsIntVar& Y, const NsInt C_init,
                          const bool isPositive_init)
          : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYlessthanZ : public Ns_ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprConstrYlessthanZ(NsIntVar& Y, NsIntVar& Z)
          : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYlesseqthanZ : public Ns_ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprConstrYlesseqthanZ(NsIntVar& Y, NsIntVar& Z)
          : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYeqZ : public Ns_ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprConstrYeqZ(NsIntVar& Y, NsIntVar& Z, const bool isPositive_init)
          : Ns_ExprConstr(isPositive_init), VarY(Y), VarZ(Z)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYandZ : public Ns_ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprConstrYandZ(NsIntVar& Y, NsIntVar& Z)
          : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_Ns(0 <= VarY.min() && VarY.max() <= 1 &&
                              0 <= VarZ.min() && VarZ.max() <= 1,
                          "Ns_ExprConstrYandZ::Ns_ExprConstrYandZ: 'VarY' and "
                          "'VarZ' should be boolean");
        }

        virtual Ns_Constraint* postConstraint(bool /*positively*/) const
        {
                throw NsException("Ns_ExprConstrYandZ::postConstraint: Please, "
                                  "add the two constraints separately");
        }

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrYorZ : public Ns_ExprConstr {

    private:
        NsIntVar &VarY, &VarZ;

    public:
        Ns_ExprConstrYorZ(NsIntVar& Y, NsIntVar& Z)
          : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_Ns(0 <= VarY.min() && VarY.max() <= 1 &&
                              0 <= VarZ.min() && VarZ.max() <= 1,
                          "Ns_ExprConstrYorZ::Ns_ExprConstrYorZ: 'VarY' and "
                          "'VarZ' should be boolean");
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& VarX, bool positively) const;
        virtual NsIntVar& postC(bool positively) const;
};

class Ns_ExprConstrNegationY : public Ns_ExprConstr {

    private:
        const Ns_ExprConstr& Yexpr;

    public:
        Ns_ExprConstrNegationY(const Ns_ExprConstr& Y)
          : Ns_ExprConstr(true), Yexpr(Y)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const
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

class Ns_ExprConstrAllDiff : public Ns_ExprConstr {

    private:
        NsIntVarArray& VarArr;
        const unsigned long Capacity;

    public:
        Ns_ExprConstrAllDiff(NsIntVarArray& Arr, const unsigned long Cap)
          : Ns_ExprConstr(true), VarArr(Arr), Capacity(Cap)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& /*VarX*/, bool /*positively*/) const
        {
                throw NsException(
                    "NsAllDiff cannot be used as a meta-constraint");
        }
        virtual NsIntVar& postC(bool /*positively*/) const
        {
                throw NsException(
                    "NsAllDiff cannot be used as a meta-constraint");
        }
};

class Ns_ExprConstrTable : public Ns_ExprConstr {

    private:
        NsIntVarArray& VarArr;
        const NsDeque<NsDeque<NsInt>>& table;
        const bool isSupportsTable;

    public:
        Ns_ExprConstrTable(NsIntVarArray& Arr,
                           const NsDeque<NsDeque<NsInt>>& table_init,
                           const bool isSupportsTable_init)
          : Ns_ExprConstr(true),
            VarArr(Arr),
            table(table_init),
            isSupportsTable(isSupportsTable_init)
        {
        }

        virtual Ns_Constraint* postConstraint(bool positively) const;

        virtual void postC(NsIntVar& /*VarX*/, bool /*positively*/) const
        {
                throw NsException(
                    "A table constraint cannot be used as a meta-constraint");
        }
        virtual NsIntVar& postC(bool /*positively*/) const
        {
                throw NsException(
                    "A table constraint cannot be used as a meta-constraint");
        }
};

inline Ns_ExprConstrYlessthanC operator<(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYlessthanC(Y, C, true);
}

inline Ns_ExprConstrYlessthanC operator>(const NsInt C, NsIntVar& Y)
{
        return (Y < C);
}

inline Ns_ExprConstrYlessthanC operator<(const Ns_Expression& Yexpr,
                                         const NsInt C)
{
        return (Yexpr.post() < C);
}

inline Ns_ExprConstrYlessthanC operator>(const NsInt C,
                                         const Ns_Expression& Yexpr)
{
        return (Yexpr < C);
}

inline Ns_ExprConstrYlesseqthanC operator<=(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYlesseqthanC(Y, C, true);
}

inline Ns_ExprConstrYlesseqthanC operator>=(const NsInt C, NsIntVar& Y)
{
        return (Y <= C);
}

inline Ns_ExprConstrYlesseqthanC operator<=(const Ns_Expression& Yexpr,
                                            const NsInt C)
{
        return (Yexpr.post() <= C);
}

inline Ns_ExprConstrYlesseqthanC operator>=(const NsInt C,
                                            const Ns_Expression& Yexpr)
{
        return (Yexpr <= C);
}

inline Ns_ExprConstrYlesseqthanC operator>(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYlesseqthanC(Y, C, false);
}

inline Ns_ExprConstrYlesseqthanC operator<(const NsInt C, NsIntVar& Y)
{
        return (Y > C);
}

inline Ns_ExprConstrYlesseqthanC operator>(const Ns_Expression& Yexpr,
                                           const NsInt C)
{
        return (Yexpr.post() > C);
}

inline Ns_ExprConstrYlesseqthanC operator<(const NsInt C,
                                           const Ns_Expression& Yexpr)
{
        return (Yexpr > C);
}

inline Ns_ExprConstrYlessthanC operator>=(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYlessthanC(Y, C, false);
}

inline Ns_ExprConstrYlessthanC operator<=(const NsInt C, NsIntVar& Y)
{
        return (Y >= C);
}

inline Ns_ExprConstrYlessthanC operator>=(const Ns_Expression& Yexpr,
                                          const NsInt C)
{
        return (Yexpr.post() >= C);
}

inline Ns_ExprConstrYlessthanC operator<=(const NsInt C,
                                          const Ns_Expression& Yexpr)
{
        return (Yexpr >= C);
}

inline Ns_ExprConstrYeqC operator==(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYeqC(Y, C, true);
}

inline Ns_ExprConstrYeqC operator==(const NsInt C, NsIntVar& Y)
{
        return (Y == C);
}

inline Ns_ExprConstrYeqC operator==(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() == C);
}

inline Ns_ExprConstrYeqC operator==(const NsInt C, const Ns_Expression& Yexpr)
{
        return (Yexpr == C);
}

inline Ns_ExprConstrYeqC operator!=(NsIntVar& Y, const NsInt C)
{
        return Ns_ExprConstrYeqC(Y, C, false);
}

inline Ns_ExprConstrYeqC operator!=(const NsInt C, NsIntVar& Y)
{
        return (Y != C);
}

inline Ns_ExprConstrYeqC operator!=(const Ns_Expression& Yexpr, const NsInt C)
{
        return (Yexpr.post() != C);
}

inline Ns_ExprConstrYeqC operator!=(const NsInt C, const Ns_Expression& Yexpr)
{
        return (Yexpr != C);
}

inline Ns_ExprConstrYlessthanZ operator<(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprConstrYlessthanZ(Y, Z);
}

inline Ns_ExprConstrYlessthanZ operator>(NsIntVar& Z, NsIntVar& Y)
{
        return (Y < Z);
}

inline Ns_ExprConstrYlessthanZ operator<(NsIntVar& Y,
                                         const Ns_Expression& Zexpr)
{
        return (Y < Zexpr.post());
}

inline Ns_ExprConstrYlessthanZ operator<(const Ns_Expression& Yexpr,
                                         NsIntVar& Z)
{
        return (Yexpr.post() < Z);
}

inline Ns_ExprConstrYlessthanZ operator>(NsIntVar& Z,
                                         const Ns_Expression& Yexpr)
{
        return (Yexpr < Z);
}

inline Ns_ExprConstrYlessthanZ operator>(const Ns_Expression& Zexpr,
                                         NsIntVar& Y)
{
        return (Y < Zexpr);
}

inline Ns_ExprConstrYlessthanZ operator<(const Ns_Expression& Yexpr,
                                         const Ns_Expression& Zexpr)
{
        return (Yexpr.post() < Zexpr.post());
}

inline Ns_ExprConstrYlessthanZ operator>(const Ns_Expression& Zexpr,
                                         const Ns_Expression& Yexpr)
{
        return (Yexpr < Zexpr);
}

inline Ns_ExprConstrYlesseqthanZ operator<=(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprConstrYlesseqthanZ(Y, Z);
}

inline Ns_ExprConstrYlesseqthanZ operator>=(NsIntVar& Z, NsIntVar& Y)
{
        return (Y <= Z);
}

inline Ns_ExprConstrYlesseqthanZ operator<=(NsIntVar& Y,
                                            const Ns_Expression& Zexpr)
{
        return (Y <= Zexpr.post());
}

inline Ns_ExprConstrYlesseqthanZ operator<=(const Ns_Expression& Yexpr,
                                            NsIntVar& Z)
{
        return (Yexpr.post() <= Z);
}

inline Ns_ExprConstrYlesseqthanZ operator>=(NsIntVar& Z,
                                            const Ns_Expression& Yexpr)
{
        return (Yexpr <= Z);
}

inline Ns_ExprConstrYlesseqthanZ operator>=(const Ns_Expression& Zexpr,
                                            NsIntVar& Y)
{
        return (Y <= Zexpr);
}

inline Ns_ExprConstrYlesseqthanZ operator<=(const Ns_Expression& Yexpr,
                                            const Ns_Expression& Zexpr)
{
        return (Yexpr.post() <= Zexpr.post());
}

inline Ns_ExprConstrYlesseqthanZ operator>=(const Ns_Expression& Zexpr,
                                            const Ns_Expression& Yexpr)
{
        return (Yexpr <= Zexpr);
}

inline Ns_ExprConstrYeqZ operator==(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprConstrYeqZ(Y, Z, true);
}

inline Ns_ExprConstrYeqZ operator==(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y == Zexpr.post());
}

inline Ns_ExprConstrYeqZ operator==(const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return (Y == Zexpr);
}

inline Ns_ExprConstrYeqZ operator==(const Ns_Expression& Yexpr,
                                    const Ns_Expression& Zexpr)
{
        return (Yexpr.post() == Zexpr.post());
}

inline Ns_ExprConstrYeqZ operator!=(NsIntVar& Y, NsIntVar& Z)
{
        return Ns_ExprConstrYeqZ(Y, Z, false);
}

inline Ns_ExprConstrYeqZ operator!=(NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return (Y != Zexpr.post());
}

inline Ns_ExprConstrYeqZ operator!=(const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return (Y != Zexpr);
}

inline Ns_ExprConstrYeqZ operator!=(const Ns_Expression& Yexpr,
                                    const Ns_Expression& Zexpr)
{
        return (Yexpr.post() != Zexpr.post());
}

inline Ns_ExprConstrYandZ operator&&(const Ns_ExprConstr& Yexpr,
                                     const Ns_ExprConstr& Zexpr)
{
        return Ns_ExprConstrYandZ(Yexpr.post(), Zexpr.post());
}

inline Ns_ExprConstrYorZ operator||(const Ns_ExprConstr& Yexpr,
                                    const Ns_ExprConstr& Zexpr)
{
        return Ns_ExprConstrYorZ(Yexpr.post(), Zexpr.post());
}

inline Ns_ExprConstrNegationY operator!(const Ns_ExprConstr& Yexpr)
{
        return Ns_ExprConstrNegationY(Yexpr);
}

inline Ns_ExprConstrAllDiff NsAllDiff(NsIntVarArray& Arr,
                                      const unsigned long Capacity = 0)
{
        return Ns_ExprConstrAllDiff(Arr, Capacity);
}

inline Ns_ExprConstrTable NsSupports(NsIntVarArray& Arr,
                                     const NsDeque<NsDeque<NsInt>>& table)
{
        return Ns_ExprConstrTable(Arr, table, true);
}

inline Ns_ExprConstrTable NsConflicts(NsIntVarArray& Arr,
                                      const NsDeque<NsDeque<NsInt>>& table)
{
        return Ns_ExprConstrTable(Arr, table, false);
}

inline Ns_ExprConstrYorZ NsIfThen(const Ns_ExprConstr& Yexpr,
                                  const Ns_ExprConstr& Zexpr)
{
        // 'p => q' is equivalent to '!p || q'
        return (!Yexpr || Zexpr);
}

inline Ns_ExprConstrYeqZ NsEquiv(const Ns_ExprConstr& Yexpr,
                                 const Ns_ExprConstr& Zexpr)
{
        // 'p <=> q' is equivalent to 'p == q'
        return (Yexpr == Zexpr);
}

inline Ns_ExprConstrYeqZ NsElement(NsIntVar& VarIndex,
                                   const NsDeque<NsInt>& intArray,
                                   NsIntVar& VarValue)
{
        return (intArray[VarIndex] == VarValue);
}

inline Ns_ExprConstrYeqZ NsElement(const Ns_Expression& VarIndexExpr,
                                   const NsDeque<NsInt>& intArray,
                                   NsIntVar& VarValue)
{
        return NsElement(VarIndexExpr.post(), intArray, VarValue);
}

inline Ns_ExprConstrYeqZ NsElement(NsIntVar& VarIndex,
                                   const NsDeque<NsInt>& intArray,
                                   const Ns_Expression& VarValueExpr)
{
        return NsElement(VarIndex, intArray, VarValueExpr.post());
}

inline Ns_ExprConstrYeqZ NsElement(const Ns_Expression& VarIndexExpr,
                                   const NsDeque<NsInt>& intArray,
                                   const Ns_Expression& VarValueExpr)
{
        return NsElement(VarIndexExpr.post(), intArray, VarValueExpr.post());
}

/// An abstract class representing an (internal or constructed by user) goal
///
/// Each time a goal is executed by the solver, its method
/// GOAL() is called. This method can make assignments or
/// remove values from constrained variables.
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

        /// The destructor of an abstract class should be virtual
        virtual ~NsGoal(void)
        {
        }
};

/// The first kind of 'meta-goal' (i.e. goal used to combine two other goals)
class NsgAND : public NsGoal {

    private:
        NsGoal* firstSubGoal;
        NsGoal* secondSubGoal;

    public:
        NsgAND(NsGoal* firstSubGoal_init, NsGoal* secondSubGoal_init)
          : firstSubGoal(firstSubGoal_init), secondSubGoal(secondSubGoal_init)
        {
                assert_Ns(firstSubGoal != 0 && secondSubGoal != 0,
                          "NsgAND::NsgAND: A subgoal is zero");
        }

        virtual bool isGoalAND(void) const
        {
                return true;
        }

        virtual NsGoal* getFirstSubGoal(void) const
        {
                return firstSubGoal;
        }

        virtual NsGoal* getSecondSubGoal(void) const
        {
                return secondSubGoal;
        }

        NsGoal* GOAL(void)
        {
                throw NsException(
                    "NsgAND::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

/// The second--and last--kind of 'meta-goal'
class NsgOR : public NsGoal {

    private:
        NsGoal* firstSubGoal;
        NsGoal* secondSubGoal;

    public:
        NsgOR(NsGoal* firstSubGoal_init, NsGoal* secondSubGoal_init)
          : firstSubGoal(firstSubGoal_init), secondSubGoal(secondSubGoal_init)
        {
                assert_Ns(firstSubGoal != 0 && secondSubGoal != 0,
                          "NsgOR::NsgOR: A subgoal is zero");
        }

        virtual bool isGoalOR(void) const
        {
                return true;
        }

        virtual NsGoal* getFirstSubGoal(void) const
        {
                return firstSubGoal;
        }

        virtual NsGoal* getSecondSubGoal(void) const
        {
                return secondSubGoal;
        }

        NsGoal* GOAL(void)
        {
                throw NsException(
                    "NsgOR::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

/// A simple goal that assigns a value to a constrained variable
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
                Var.set(value);
                return 0;
        }
};

/// A goal that removes a value from the domain of a given constrained variable
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
                Var.remove(value);
                return 0;
        }
};

/// A goal that tries to instantiate a constrained variable
///
/// I.e. it assigns a value from the domain of the variable; if
/// this assignment results to an inconsistency, it removes the
/// value from the domain, and continues trying the rest of the
/// values.
class NsgInDomain : public NsGoal {

    private:
        NsIntVar& Var;

    public:
        NsgInDomain(NsIntVar& Var_init) : Var(Var_init)
        {
        }

        NsGoal* GOAL(void)
        {
                if (Var.isBound())
                        return 0;
                NsInt value = Var.min();
                return (new NsgOR(new NsgSetValue(Var, value),
                                  new NsgAND(new NsgRemoveValue(Var, value),
                                             new NsgInDomain(*this))));
        }
};

/// The generalization of NsgInDomain that applies to arrays
///
/// It uses NsgInDomain to iteratively instantiate each variable
/// of the array. The next variable that is chosen is the one
/// having the minimum domain size (according to the
/// 'first-fail' heuristic).
class NsgLabeling : public NsGoal {

    private:
        NsIntVarArray& VarArr;

    public:
        NsgLabeling(NsIntVarArray& VarArr_init) : VarArr(VarArr_init)
        {
        }

        NsGoal* GOAL(void)
        {
                NsIndex index = NsINDEX_INF;
                NsUInt minDom = NsUPLUS_INF;
                for (NsIndex i = 0; i < VarArr.size(); ++i) {
                        if (!VarArr[i].isBound() && VarArr[i].size() < minDom) {
                                minDom = VarArr[i].size();
                                index = i;
                        }
                }
                if (index == NsINDEX_INF)
                        return 0;
                return (new NsgAND(new NsgInDomain(VarArr[index]),
                                   new NsgLabeling(*this)));
        }
};

/// The type of the objects in the queue for the AC algorithm
///
/// When a value is removed from the domain of a variable, an
/// item is added into this queue.
class Ns_QueueItem {

    private:
        /// The domain of this variable has been modified
        NsIntVar* varFired;

        /// The index of the constraint to check
        NsDeque<NsIntVar::ConstraintAndFailure>::size_type currentConstr;

        struct RemovedValueRecord_t {

                /// The value that was taken from the domain of varFired. The
                /// 'w' of the AC-5 algorithm.
                const NsInt value;

                /// The constraint that fired the removal of the value
                /// removedValue from the domain of the variable varFired. If
                /// no constraint provoked the removal, then constrFired == 0.
                const Ns_Constraint* constrFired;

                /// Constructor
                RemovedValueRecord_t(const NsInt removedValue_init,
                                     const Ns_Constraint* constrFired_init)
                  : value(removedValue_init), constrFired(constrFired_init)
                {
                }
        };

        /// The index of the removed value that will be checked against
        /// constraints (that require arc consistency)
        NsDeque<RemovedValueRecord_t>::size_type currentRemovedValue;

        /// Holds information having to do with the modification (if any) of the
        /// bounds of varFired
        struct RemovedBoundRecord_t {

                /// True if the minimum or maximum of the domain of varFired has
                /// been changed
                bool removedBound;

                /// The constraint that fired the last modification of the
                /// bounds of varFired. If no constraint provoked the removal,
                /// then constrFired == 0.
                const Ns_Constraint* constrFired;

                /// The domain removal serial number
                unsigned long removalTime;

                /// Constructor
                RemovedBoundRecord_t(void) : removedBound(false)
                {
                }

                /// Records a bounds modification
                void boundChangedBy(const Ns_Constraint* constrFired_init,
                                    const unsigned long removalTime_init)
                {
                        constrFired = constrFired_init;
                        removedBound = true;
                        removalTime = removalTime_init;
                }
        };

        /// Used to record a modification of the bounds (if any)
        RemovedBoundRecord_t removedBoundRec;

        /// An array that records the values removed from the domain of varFired
        NsDeque<RemovedValueRecord_t> removedValues;

    public:
        /// Constructor
        Ns_QueueItem(NsIntVar* varFired_init)
          : varFired(varFired_init), currentConstr(0), currentRemovedValue(0)
        {
        }

        ~Ns_QueueItem(void)
        {
                if (varFired->queueItem == this)
                        varFired->queueItem = 0;
                // Is there any possibility of varFired->queueItem != this?
                // Yes, when there are two items in AC queue for 'varFired',
                // the first being examined now by the AC algorithm.
        }

        void boundChangedBy(const Ns_Constraint* constr);

        void add(const NsInt removedVal,
                 const Ns_Constraint* constrThatRemovedIt);

        Ns_Constraint* getNextConstraint(void);

        /// Returns the variable that fired the constraint propagation
        NsIntVar* getVarFired(void) const
        {
                return varFired;
        }

        /// Returns the value that has been removed from the variable
        NsInt getW(void) const
        {
                return removedValues[currentRemovedValue - 1].value;
        }

        /// When a constraint provokes an inconsistency, then its rank (index in
        /// the 'varFired->constraints' array) should be updated according to
        /// the current number of inconsistencies it provoked (according to a
        /// heuristic)
        void resortConstraints(const Ns_Constraint* constr) const
        {
                // The following statement 'corrects' currentConstr
                // by assigning the proper value to constrFailed.
                long constrFailed =
                    (constr->revisionType == Ns_Constraint::VALUE_CONSISTENCY)
                        ? currentConstr
                        : currentConstr - 1;
                ++varFired->constraints[constrFailed].failures;
                for (long c = constrFailed - 1; c >= 0; --c) {
                        if (varFired->constraints[c].failures <
                            varFired->constraints[constrFailed].failures) {
                                std::swap(varFired->constraints[c],
                                          varFired->constraints[constrFailed]);
                        }
                }
        }
};

/// Normally used for describing the stack holding AND-goals that have to be
/// satisfied. This stack is also called 'stackAND'.
class Ns_StackGoals : public NsStack<NsGoal*> {

    public:
        ~Ns_StackGoals(void);
};

struct Ns_SearchNode;

/// Contains all the Ns_SearchNode's
///
/// A Ns_SearchNode contains the current status of the problem;
/// it is pushed into the stack when we make a choice (e.g. when
/// we select a specific value to assign it to a variable) and
/// it is popped when we want to cancel this choice, and we want
/// to revert back to the previous problem status.
class Ns_StackSearch : public NsStack<Ns_SearchNode> {

    public:
        Ns_StackSearch(void);

        ~Ns_StackSearch(void);

        void clear(void);

        /// @{
        /// @name Provision of history ids to the search nodes

    private:
        /// History-IDs together with time statistics
        struct history_time_t {

                /// An ID for the current history level
                NsUInt validHistoryId;

                /// An ID for the current node number
                NsUInt searchTreeNodeNum;

                /// The time consumed all the nodes in this level
                double timeSum;

                /// The number of the descendants of all nodes in this level
                /// squared
                double descSum2;

                /// The mean number of the descendants of all nodes in this
                /// level
                double descMean;

                /// The sum of the weights of timeSum terms
                double timeWeights;

                /// The sum of the weights of the descSum terms
                double descWeights;

                /// Constructor
                history_time_t(void)
                  : validHistoryId(0),
                    timeSum(0.0),
                    descSum2(0.0),
                    descMean(0.0),
                    timeWeights(0.0),
                    descWeights(0.0)
                {
                }

                /// Augments the valid history ID and updates statistics
                void invalidate(const clock_t timeBorn,
                                const double timeSimChild,
                                const unsigned long descNow,
                                const unsigned long descBorn,
                                const double descSimChild)
                {
                        ++validHistoryId;
                        double timeNode = clock() - timeBorn + timeSimChild;
                        double descNode = descNow - descBorn + descSimChild;
                        double timeWeight =
                            (timeNode - timeSimChild + 1.0) / (timeNode + 1.0);
                        double descWeight =
                            (descNode - descSimChild + 1.0) / (descNode + 1.0);
                        timeSum += timeNode * timeWeight;
                        timeWeights += timeWeight;
                        // Compute descendants weighted mean and variance:
                        // temp = weight + sumweight
                        double temp = descWeight + descWeights;
                        // delta = x - mean
                        double delta = descNode - descMean;
                        // R = delta * weight / temp
                        double R = delta * descWeight / temp;
                        // mean = mean + R
                        descMean += R;
                        // M2 = M2 + sumweight * delta * R
                        descSum2 += descWeights * delta * R;
                        // descSum += descNode * descWeight;
                        // sumweight = temp
                        descWeights += descWeight;
                }

                /// The mean value of the time spent in this level
                double meanTime(void) const
                {
                        assert_Ns(validHistoryId != 0,
                                  "history_time_t::meanTime: Cannot get mean "
                                  "value of an empty set");
                        return (timeSum / timeWeights);
                }

                /// The mean value of the descendants of a node in this level
                double meanDesc(void) const
                {
                        assert_Ns(validHistoryId != 0,
                                  "history_time_t::meanDesc: Cannot get mean "
                                  "value of an empty set");
                        return descMean;
                }

                /// The mean value of the descendants of a node in this level
                /// plus the standard deviation
                double standardDeviationDesc(void) const
                {
                        assert_Ns(validHistoryId > 1,
                                  "history_time_t::standardDeviationDesc: "
                                  "Cannot get standard deviation");
                        // variance_n = M2 / sumweight
                        double variance_n = descSum2 / descWeights;
                        // variance = variance_n *
                        // len(dataWeightPairs) / (len(dataWeightPairs) - 1)
                        double variance =
                            variance_n * validHistoryId / (validHistoryId - 1);
                        return sqrt(variance);
                }

                /// The mean value of the descendants of a node in this level
                /// plus the standard deviation
                double meanDescPlusSD(void) const
                {
                        return (descMean + standardDeviationDesc());
                }
        };

        NsDeque<history_time_t> history_time;

        unsigned long nSearchTreeNodes;

    public:
        /// The mean value of the time spent in the next level
        double nextMeanTime(void) const
        {
                return history_time[size()].meanTime();
        }

        /// The mean value of the descendants in the next level
        double nextMeanDesc(void) const
        {
                return history_time[size()].meanDesc();
        }

        /// The mean value of the descendants in the next level plus the
        /// standard deviation
        double nextMeanDescPlusSD(void) const
        {
                return history_time[size()].meanDescPlusSD();
        }

        /// Decides whether the next level will be explored or simulated
        bool overrideNextLevel(void)
        {
                if (history_time.size() < size() + 1 ||
                    history_time[size()].validHistoryId < 2)
                        return false;
                // Simulation ratio corresponds to expected descendants
                double simRatio = pow(simulationRatio, nextMeanDescPlusSD());
                double random = rand() / (RAND_MAX + 1.0);
                return (random <= simRatio);
        }

        /// The search tree split to be explored starts from this node
        NsList<NsUInt> startNode;

    private:
        /// The search tree split to be explored ends up in this node
        NsDeque<NsUInt> endNode;

    public:
        bool readSplit(std::string& splitEnd);

        bool splitEnded(void);

        /// Tests whether splitEnded() tells the truth
        bool TEST_splitEnded(void) const;

    private:
        void TEST_CurrentVsEndNode(const_iterator it, NsUInt& depth,
                                   bool& equal, bool& greater) const;

    public:
        unsigned long numSearchTreeNodes(void) const
        {
                return (nSearchTreeNodes - 1);
        }

        bool push(const value_type& newNode);

        void pop(void);

        /// Restores the validHistoryId's state as it was before search began
        void reset(void)
        {
                history_time[0].validHistoryId = 0;
        }

        NsUInt getCurrentNodeNum(void) const
        {
                return history_time[size() - 1].searchTreeNodeNum;
        }

        Ns_HistoryId_t getCurrentHistoryId(void) const
        {
                Ns_HistoryId_t di;
                di.level = size() - 1;
                di.id = history_time[di.level].validHistoryId;
                return di;
        }

        bool isCurrentHistoryId(const Ns_HistoryId_t di) const
        {
                return (di.level == size() - 1 &&
                        di.id == history_time[di.level].validHistoryId);
        }

        bool isCurrentHistoryIdNextTo(const Ns_HistoryId_t di) const
        {
                return (di.level < size() - 1 || !isValidHistoryId(di));
        }

        bool isValidHistoryId(const Ns_HistoryId_t di) const
        {
                return (di.id == history_time[di.level].validHistoryId);
        }

        /// Virtual extra time used for simulation
        double timeSimulated;

        /// The percentage of the real search time vs total simulation time
        double simulationRatio;

        void currentPath(void) const
        {
                currentPathRec(begin());
        }

    private:
        void currentPathRec(const_iterator it) const;

    public:
        /// When endNode is changed, it is called to update the nodes
        /// matchesEndNode statuses
        void updateMatchesEndNode(void)
        {
                NsUInt depth;
                updateMatchesEndNodeRec(begin(), depth);
        }

    private:
        bool updateMatchesEndNodeRec(iterator it, NsUInt& depth);

        /// @}

        /// @{
        /// @name Representation of the search tree as a graph

    private:
        /// The mapper's ID
        int mapper;

        /// The mapper's current input line
        std::string mapperLine;

        /// The time the mapper started to process a line
        double mapperLineStartTime;

        /// File to store the MapReduce mapper's input
        std::ofstream fileMapperInput;

        /// File to store the search tree graph
        std::ofstream fileSearchGraph;

        /// True if it should write the objective value
        bool recordObjective;

        /// The last recorded objectiveValue
        NsInt objectiveValue;

    public:
        void searchToGraphFile(const char* fileName);

        void mapperInputToFile(const char* fileName, int mapperId);

        void solutionNode(const NsIntVar* vObjective);

        /// @}

        /// Iterates through all the goals in the current Ns_StackGoals and the
        /// Ns_StackGoals below it
        ///
        /// All of them consist a stack of Ns_StackGoals, named stackOfStacks.
        class goal_iterator {

            private:
                /// A stack containing the search nodes (so each frame of this
                /// stack contains a Ns_StackGoals)
                Ns_StackSearch* stackOfStacks;

                /// Points to the current search node
                Ns_StackSearch::iterator curr_Stack_it;

                /// Points to the current goal (in the Ns_StackGoals) of the
                /// current search node
                Ns_StackGoals::iterator curr_node_it;

            public:
                goal_iterator(void) : stackOfStacks(0)
                {
                }

                goal_iterator(Ns_StackSearch& stackOfStacks_init);

                bool operator==(const goal_iterator& b) const
                {
                        assert_Ns(stackOfStacks != 0, "Ns_StackSearch::goal_"
                                                      "iterator::==: "
                                                      "Uninitialized '*this'");
                        return (curr_Stack_it == b.curr_Stack_it &&
                                (curr_Stack_it == stackOfStacks->end() ||
                                 curr_node_it == b.curr_node_it));
                }

                bool operator!=(const goal_iterator& b) const
                {
                        return !(*this == b);
                }

                NsGoal* operator*(void)const
                {
                        assert_Ns(stackOfStacks != 0, "Ns_StackSearch::goal_"
                                                      "iterator::*: "
                                                      "Uninitialized '*this'");
                        assert_Ns(
                            curr_Stack_it != stackOfStacks->end(),
                            "Ns_StackSearch::goal_iterator::*: Bad request");
                        return *curr_node_it;
                }

                goal_iterator& end(void)
                {
                        assert_Ns(stackOfStacks != 0, "Ns_StackSearch::goal_"
                                                      "iterator::end: "
                                                      "Uninitialized '*this'");
                        curr_Stack_it = stackOfStacks->end();
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
                goal_iterator iterEnd(*this);
                return iterEnd.end();
        }
};

/// Describes a search node of the binary search tree
///
/// A critical type that can describe the current status of the problem.
struct Ns_SearchNode {

    public:
        /// Alternative goal to satisfy if search fails
        NsGoal* goalNextChoice;

        /// Pointer to the first goal of the previous search level that has not
        /// been yet satisfied
        Ns_StackSearch::goal_iterator delayedGoal;

        /// All the goals of this list should be satisfied
        Ns_StackGoals stackAND;

        /// Constructor
        Ns_SearchNode(NsGoal* goalNextChoice_init,
                      Ns_StackSearch::goal_iterator git,
                      const unsigned long descBorn_init)
          : goalNextChoice(goalNextChoice_init),
            delayedGoal(git),
            children(0),
            timeBorn(clock()),
            descBorn(descBorn_init),
            timeSimChild(0.0),
            descSimChild(0.0)
        {
        }

        /// Describes a tuple (BitsetDomainPointer, BitsetDomain)
        class BitsetCopy {

            private:
                /// Pointer to a bit-set domain
                Ns_BitSet* bitsetDomainPointer;

                /// A copy of the above instance--created for future
                /// backtracking reasons
                Ns_BitSet bitsetDomainCopy;

            public:
                /// Copy-constructor
                BitsetCopy(Ns_BitSet& bitsetDomain)
                  : bitsetDomainPointer(&bitsetDomain),
                    bitsetDomainCopy(bitsetDomain)
                {
                }

                /// Restores the copy back to the original domain place (i.e.
                /// pointer)
                void restore(void)
                {
                        *bitsetDomainPointer = bitsetDomainCopy;
                }
        };

    private:
        /// List to contain the saved bit-set domains
        class BitsetsStore : public NsStack<BitsetCopy> {

            public:
                /// Restores all the saved bit-set domains
                void restore(void)
                {
                        while (!empty()) {
                                top().restore();
                                pop();
                        }
                }
        };

    public:
        /// Store to keep the previous states of the modified bit-sets
        BitsetsStore bitsetsStore;

        /// The node's children number
        NsUInt children;

        /// True if the current search node and its predecessors match the
        /// endNode
        bool matchesEndNode;

        /// When the node was born?
        const clock_t timeBorn;

        /// How many tree nodes pushes existed when the node was born?
        const unsigned long descBorn;

        /// Virtual extra time, used in the simulation of the search tree
        /// exploration
        double timeSimChild;

        /// Virtual extra descendants, used in the simulation of the search tree
        /// exploration
        double descSimChild;
};

/// Constraint satisfaction problem manager and solver class
///
/// A problem manager keeps information for the constrained
/// variables, the constraints network, and the goal that will
/// be executed.
class NsProblemManager {

    private:
        /// The stack containing the choice points that have been appeared
        /// during search. Each choice point forms a binary search tree node.
        Ns_StackSearch searchNodes;

        /// AC algorithm event-queue type
        typedef NsQueue<Ns_QueueItem> Ns_Queue_t;

        /// AC algorithm event-queue
        Ns_Queue_t Q;

        /// True, if an inconsistency has been found
        bool foundInconsistency;

        bool arcConsistent(void);

        bool backtrack(void);

        /// The list of the soft (meta)constraints to be satisfied
        NsIntVarArray vSoftConstraintsTerms;

    public:
        /// If a fileNameSearchGraph is provided, then a file is created with
        /// the search tree in a Graphviz supported format.
        NsProblemManager(void)
          : foundInconsistency(false),
            vObjective(0),
            timeLim(0),
            firstNextSolution(true),
            calledTimeLimit(false),
            timeSplitLim(0),
            nFailures(0),
            nBacktracks(0),
            nGoals(0),
            nConstraintChecks(0),
            backtrackLim(0)
        {
                assert_Ns(sizeof(NsIntVar*) <= sizeof(Ns_pointer_t),
                          "NsProblemManager::NsProblemManager: Cannot run on "
                          "this machine, because a pointer does not fit in an "
                          "'size_t' (the 'unordered_set' type)");
                assert_Ns(searchNodes.push(Ns_SearchNode(
                              0, searchNodes.gbegin(), numSearchTreeNodes())),
                          "NsProblemManager::NsProblemManager: First push "
                          "should succeed");
        }

        ~NsProblemManager(void);

        /// Adds a goal to be executed/satisfied
        void addGoal(NsGoal* goal)
        {
                if (goal != 0)
                        searchNodes.top().stackAND.push(goal);
        }

        void add(const Ns_ExprConstr& expr);

        void add(const Ns_ExprConstr& expr, const NsInt weight);

        /// Returns the AC algorithm queue
        Ns_Queue_t& getQueue(void)
        {
                return Q;
        }

        /// Informs NsProblemManager than an inconsistency has been found
        void foundAnInconsistency(void)
        {
                foundInconsistency = true;
        }

        bool nextSolution(void);

        void restart(void);

        /// Writes to a file the (splits) input of a mapper
        void mapperInputToFile(const char* fileName, int mapper)
        {
                searchNodes.mapperInputToFile(fileName, mapper);
        }

        /// Writes to a file a view of the search tree in a Graphviz supported
        /// format
        void searchToGraphFile(const char* fileName)
        {
                searchNodes.searchToGraphFile(fileName);
        }

        /// @{
        /// @name Representation of the constraint network as a graph

    private:
        /// File to store the constraint network graph
        std::ofstream fileConstraintsGraph;

    public:
        /// Writes to a file a view of the constraint network in a Graphviz
        /// supported format
        void constraintsToGraphFile(const char* fileName);

        /// @}

        /// @{
        /// @name Optimization members

    private:
        /// Our objective is to minimize this constrained variable
        NsIntVar* vObjective;

        /// The minimum upper limit of 'vObjective' found so far (initially +oo)
        NsInt bestObjective;

    public:
        /// Exposes the objective variable
        const NsIntVar& getObjective(void) const
        {
                assert_Ns(vObjective != 0, "NsProblemManager::getObjective: "
                                           "Objective variable not set");
                return *vObjective;
        }

        void minimize(NsIntVar& VarObjective)
        {
                assert_Ns(vObjective == 0,
                          "minimize() cannot be called more than one time");
                vObjective = &VarObjective;
                bestObjective = NsPLUS_INF;
        }

        void minimize(const Ns_Expression& expr)
        {
                minimize(expr.post());
        }

        void objectiveUpperLimit(NsInt max)
        {
                assert_Ns(vObjective != 0, "NsProblemManager::"
                                           "objectiveUpperLimit: No cost "
                                           "variable exists");
                if (bestObjective > max + 1) {
                        bestObjective = max + 1;
                        vObjective->remove(bestObjective, NsPLUS_INF);
                }
        }

        /// @}

        /// @{
        /// @name Time management members

    private:
        /// True if the available time is real (i.e. not system time)
        bool isRealTime;

        /// The first (system) time that nextSolution() has been called
        clock_t startTime;

        /// The first time that nextSolution() has been called
        time_t startRealTime;

        /// Available time (for running the search process) in seconds
        unsigned long timeLim;

        /// True if nextSolution() has not been yet called
        bool firstNextSolution;

        /// True if a limit to the available time (for search process) has been
        /// set
        bool calledTimeLimit;

        /// The first (system) time that nextSolution() has been called for the
        /// current search space split
        clock_t startSplitTime;

        /// Available time ticks for running the search process for one search
        /// space split
        clock_t timeSplitLim;

        /// The previous recorded node by the splitting process
        NsUInt startNodeId;

        /// Flag set by nextSolution() when time is up
        bool timeIsUp;

    public:
        /// Sets the time limit. After this limit is exceeded, nextSolution()
        /// returns false.
        void realTimeLimit(const unsigned long secs)
        {
                calledTimeLimit = true;
                isRealTime = true;
                timeLim = secs;
        }

        /// Sets the CPU time limit. After this limit is exceeded,
        /// nextSolution() returns false.
        void timeLimit(const unsigned long secs)
        {
                calledTimeLimit = true;
                isRealTime = false;
                timeLim = secs;
        }

        /// Sets the limit to the backtracks that search process does. After
        /// this limit is exceeded, nextSolution() returns false.
        void backtrackLimit(const unsigned long lim)
        {
                backtrackLim = nBacktracks + lim;
        }

        /// @}

        /// @{
        /// @name Search tree splitting functions

        /// The first word of a line that descibes a search tree split
        static const char* SPLIT_HEADER;

        /// Prints out the header, e.g. the word "Split", in front of each line
        /// that designates a search tree split
        void splitHeader(void)
        {
                std::cout << SPLIT_HEADER << " ";
        }

        /// Sets the time ticks limit for each search space split
        void splitTimeLimit(const double secs, const double simulationRatio)
        {
                clock_t ticks = secs * CLOCKS_PER_SEC;
                assert_Ns(0.0 <= simulationRatio && simulationRatio <= 1.0,
                          "NsProblemManager::splitTimeLimit: 'simulationRatio' "
                          "must be between 0 and 1");
                timeSplitLim = ticks;
                searchNodes.simulationRatio = simulationRatio;
                if (timeSplitLim == 0)
                        return;
                startNodeId = getCurrentNodeNum();
                startSplitTime = clock();
                assert_Ns(startSplitTime != -1,
                          "Could not find time for 'splitTimeLimit'");
                splitHeader();
                searchNodes.currentPath();
        }

        /// Explore specific search tree splits described in standard input
        bool readSplit(void)
        {
                return searchNodes.readSplit(splitEnd);
        }

    private:
        /// Contains a string describing the last path of the current search
        /// tree split
        std::string splitEnd;

    public:
        void simulate(const double splitTime, const double simulationRatio);

        /// @}

        /// @{
        /// @name Statistic members

        NsDeque<const NsIntVar*> vars;

        unsigned long nFailures, nBacktracks, nGoals, nConstraintChecks,
            backtrackLim;

    public:
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
                return nBacktracks;
        }

        NsStack<const NsIntVar*>::size_type numVars(void) const
        {
                return vars.size();
        }

        Ns_constraints_array_t::size_type numConstraints(void) const
        {
                return constraints.size();
        }

        unsigned long numConstraintChecks(void) const
        {
                return nConstraintChecks;
        }

        unsigned long numSearchTreeNodes(void) const
        {
                return searchNodes.numSearchTreeNodes();
        }

        /// Collects the constrained variables of the problem
        void addVar(const NsIntVar* Var)
        {
                vars.push_back(Var);
        }

        /// Removes the last variable pointer recorded, to correct a problem
        /// created by Ns_Expression::post(Var)
        void removeLastVar(void)
        {
                vars.resize(vars.size() - 1);
        }

        /// Prints statistics about the CSP parameters
        void printCspParameters(void) const
        {
                NsUInt domainsSizeMax = 0;
                double domainsSizeSum = 0.0;
                for (NsDeque<const NsIntVar*>::const_iterator v = vars.begin();
                     v != vars.end(); ++v) {
                        if ((*v)->size() > domainsSizeMax)
                                domainsSizeMax = (*v)->size();
                        domainsSizeSum += (*v)->size();
                }
                std::cout << numVars() << "\t" << domainsSizeMax << "\t"
                          << domainsSizeSum / numVars() << "\t"
                          << numConstraints() << "\n";
        }

        /// @}

        /// @{
        /// @name Arrays of variables and constraints used for memory management

    private:
        NsDeque<NsIntVar*> intermediateVars;

        Ns_constraints_array_t constraints;

    public:
        void recordIntermediateVar(NsIntVar* Var)
        {
                intermediateVars.push_back(Var);
        }

        void recordConstraint(Ns_Constraint* newConstr)
        {
                constraints.push_back(newConstr);
        }

        /// @}

        /// @{
        /// @name Provision of history ids to the search nodes

        unsigned long getCurrentNodeNum(void) const
        {
                return searchNodes.getCurrentNodeNum();
        }

        Ns_HistoryId_t getCurrentHistoryId(void) const
        {
                return searchNodes.getCurrentHistoryId();
        }

        bool isCurrentHistoryId(const Ns_HistoryId_t di) const
        {
                return searchNodes.isCurrentHistoryId(di);
        }

        bool isCurrentHistoryIdNextTo(const Ns_HistoryId_t di) const
        {
                return searchNodes.isCurrentHistoryIdNextTo(di);
        }

        bool isValidHistoryId(const Ns_HistoryId_t di) const
        {
                return searchNodes.isValidHistoryId(di);
        }

        /// @}

        /// Saves the bitsetDomain for future backtracking purposes
        void saveBitsetDomain(Ns_BitSet& bitsetDomain)
        {
                searchNodes.top().bitsetsStore.push(
                    Ns_SearchNode::BitsetCopy(bitsetDomain));
                bitsetDomain.lastSaveHistoryId() = getCurrentHistoryId();
        }
};

} // end namespace

#endif // Ns_NAXOS_MINI_H

/// @example nqueens.cpp
/// How to use Naxos to solve the N-queens problem.
///
/// The N-queens puzzle is the problem of putting eight chess
/// queens on an N x N chessboard such that none of them is able
/// to capture any other using the standard chess queen's moves.
/// The queens must be placed in such a way that no two queens
/// would be able to attack each other. Thus, a solution
/// requires that no two queens share the same row, column, or
/// diagonal. [Description source: Wikipedia]

/// @example send_more_money.cpp
/// How to use Naxos to solve SEND + MORE = MONEY cryptarithm.
///
/// Verbal arithmetic, also known as alphametics,
/// cryptarithmetic, crypt-arithmetic, or cryptarithm, is a type
/// of mathematical game consisting of a mathematical equation
/// among unknown numbers, whose digits are represented by
/// letters. The goal is to identify the value of each letter.
/// The name can be extended to puzzles that use non-alphabetic
/// symbols instead of letters. This is often a technique used
/// by teachers to get students excited about math subjects such
/// as algebra 1 honors. Supposedly, students will think it is
/// cool to add, subtract, multiply, and divide their favorite
/// words and letters.
///
/// The equation is typically a basic operation of arithmetic,
/// such as addition, multiplication, or division. The classic
/// example, published in the July 1924 issue of Strand Magazine
/// by Henry Dudeney, is:
///
/// @code
///     S E N D
/// +   M O R E
/// = M O N E Y
/// @endcode
///
/// The solution to this puzzle is O = 0, M = 1, Y = 2, E = 5,
/// N = 6, D = 7, R = 8, and S = 9.
///
/// Traditionally, each letter should represent a different
/// digit, and (as in ordinary arithmetic notation) the leading
/// digit of a multi-digit number must not be zero. A good
/// puzzle should have a unique solution, and the letters should
/// make up a cute phrase (as in the example above).
/// [Description source: Wikipedia]
