///  \file
///  \brief  Definitions of internal and public classes of <span style="font-variant: small-caps;"> Naxos. </span>
///
///  This file is part of
///   <span style="font-variant: small-caps;"> Naxos Solver: </span>
///   A Constraint Programming Library. \n
///   Copyright © 2007-2015  Nikolaos Pothitos.
///
///  See ../license/LICENSE for the license of the library.

//   In the comments, words beginning with a backslash `\' should be
//    ignored by the reader, because they are used by the `doxygen' program
//    that generates a source code reference manual.

///  \mainpage
///
///  <span style="font-variant: small-caps;"> Naxos </span> is a Constraint
///   Satisfaction Problem (CSP) solver.  For a general idea and
///   information on how to use it, read \c naxos.pdf first (written in
///   Greek and normally included in the source code package).  Those pages
///   and comments can be only used for internal developing purposes.
///
///  \note
///   The names of <span style="font-variant: small-caps;"> Naxos </span>
///   classes and other declarations begin with \c `Ns'.  So it is a good
///   practice not to put this prefix to our classes/data-types.
///   (Moreover, prefix \c `Nsg' is used for
///   <span style="font-variant: small-caps;"> Naxos </span> goal classes,
///   and prefix \c `Ns_' is used for internal classes that should not be
///   used by the users of <span style="font-variant: small-caps;">
///   Naxos</span>.)
///
///  \todo  Make more types/classes internal to other classes.
///
///  \todo  Overload the operator `NsIntVar = NsIntVar' in order to create
///          a constraint.
///
///  \todo  Reduce assertions.
///
///  \todo  O(1) removeRange().

//#define Ns_AllDiff_Test

#ifndef Ns_NAXOS_H
#define Ns_NAXOS_H

//#define Ns_LOCAL_SEARCH

#include <iostream>
#include <climits>
#include <ctime>
#include <deque>
#include <queue>
#include <list>

#ifdef  __GNUC__
        #if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
                #define  Ns_OLD_UNORDERED_SET
        #endif
#endif							 // __GNUC__

#ifdef Ns_OLD_UNORDERED_SET
        #include <ext/hash_set>
        #include <ext/hash_map>
        #define  Ns_UNORDERED_SET  __gnu_cxx::hash_set
        #define  Ns_UNORDERED_MAP  __gnu_cxx::hash_map
#else
        #include <unordered_set>
        #include <unordered_map>
        #define  Ns_UNORDERED_SET  std::unordered_set
        #define  Ns_UNORDERED_MAP  std::unordered_map
#endif

#include <stdexcept>
#include <string>
#include <fstream>
#include <algorithm>

#include <cmath>

///  The namespace for <span style="font-variant: small-caps;"> Naxos Solver. </span>

namespace  naxos {

///  <span style="font-variant: small-caps;"> Naxos </span> methods throw this type of exception.

///  \internal

class NsException : public std::logic_error {

    public:

        NsException (const std::string& error_message)
                : logic_error("Naxos: " + error_message)  {    }
};

///  Asserts that the \a condition is \c true.  If it is not, it throws a NsException.

///  Here we used the type \a char* for \a errorMessage, instead of
///   \a string, plainly for time-performance reasons.
///   \internal

inline void
assert_Ns (const bool condition, const char *errorMessage)
{
        if ( ! condition )
                throw  NsException(errorMessage);
}

///  @{
///  \name  Definitions of Naxos Solver types and their limits

typedef           long  NsInt;
typedef  unsigned long  NsUInt;

const NsInt   NsMINUS_INF =  LONG_MIN;
const NsInt    NsPLUS_INF =  LONG_MAX;
const NsUInt  NsUPLUS_INF = ULONG_MAX;

///  @}

class  NsIntVar;

template <class TemplType>
class  NsDeque;

///  Abstract class that represents an (algebraic etc.) expression between constrained variables.

///  \internal

class  Ns_Expression {

    public:

        ///  Posts the constraint \a VarX \c == \c *this
        ///   (\c *this represents an Ns_Expression).
        virtual void       post (NsIntVar& VarX)  const = 0;

        ///  Produces/returns a variable to represent the
        ///   Ns_Expression (\c *this).
        virtual NsIntVar&  post (void)  const = 0;

        //{  throw  NsException("Ns_Expression::post: this is an \"abstract\" function");  }

        // The destructor of an abstract class should be virtual.
        virtual ~Ns_Expression (void)  {   }
};

class Ns_ExprElement : public Ns_Expression {

    private:

        NsIntVar&  VarIndex;
        const NsDeque<NsInt>&  intArray;
        //NsIntVar&  VarValue;

    public:

        Ns_ExprElement (NsIntVar& VarIndex_init,
                        const NsDeque<NsInt>& intArray_init
                        /*,
                        					NsIntVar& VarValue_init*/
                       )
                : VarIndex(VarIndex_init),
                  intArray(intArray_init)
                  /*,
                  			VarValue(VarValue_init)*/
        {
        }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

///  A flexible array data structure (like \c std::deque).

///  A safer version of \c std::deque that throws an exception when an \c
///   out_of_range request is being made.  It takes advantage of the
///   internal \c at() method that does this work.
template <class TemplType>
class  NsDeque : public std::deque<TemplType> {

    public:

        NsDeque (void)
        {    }

        NsDeque (const typename std::deque<TemplType>::size_type n)
                : std::deque<TemplType>(n)
        {    }

        typename std::deque<TemplType>::reference
        operator  [] (const typename std::deque<TemplType>::size_type i)
        {
                return  this->at(i);
        }

        typename std::deque<TemplType>::const_reference
        operator  [] (const typename std::deque<TemplType>::size_type i)  const
        {
                return  this->at(i);
        }

        Ns_ExprElement
        operator  [] (NsIntVar& VarIndex)  const
        {
                return  Ns_ExprElement(VarIndex, *this);
        }

        Ns_ExprElement
        operator  [] (const Ns_Expression& VarIndexExpr)  const
        {
                return  (*this)[VarIndexExpr.post()];
        }

        typename std::deque<TemplType>::reference
        front (void)
        {
                if ( std::deque<TemplType>::empty() )
                        throw  std::out_of_range("NsDeque::front: Empty deque");
                return  std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::const_reference
        front (void)  const
        {
                if ( std::deque<TemplType>::empty() )
                        throw  std::out_of_range("NsDeque::front: Empty deque");
                return  std::deque<TemplType>::front();
        }

        typename std::deque<TemplType>::reference
        back (void)
        {
                if ( std::deque<TemplType>::empty() )
                        throw  std::out_of_range("NsDeque::back: Empty deque");
                return  std::deque<TemplType>::back();
        }

        typename std::deque<TemplType>::const_reference
        back (void)  const
        {
                if ( std::deque<TemplType>::empty() )
                        throw  std::out_of_range("NsDeque::back: Empty deque");
                return  std::deque<TemplType>::back();
        }
};

///  \c std::queue with exceptions enabled.

template <class TemplType>
class  NsQueue : public std::queue<TemplType> {

    public:

        void  clear (void)
        {
                while ( ! std::queue<TemplType>::empty() )
                        std::queue<TemplType>::pop();
        }

        typename std::queue<TemplType>::reference
        front (void)
        {
                if ( std::queue<TemplType>::empty() )
                        throw  std::out_of_range("NsQueue::front: Empty queue");
                return  std::queue<TemplType>::front();
        }

        typename std::queue<TemplType>::const_reference
        front (void)  const
        {
                if ( std::queue<TemplType>::empty() )
                        throw  std::out_of_range("NsQueue::front: Empty queue");
                return  std::queue<TemplType>::front();
        }

        typename std::queue<TemplType>::reference
        back (void)
        {
                if ( std::queue<TemplType>::empty() )
                        throw  std::out_of_range("NsQueue::back: Empty queue");
                return  std::queue<TemplType>::back();
        }

        typename std::queue<TemplType>::const_reference
        back (void)  const
        {
                if ( std::queue<TemplType>::empty() )
                        throw  std::out_of_range("NsQueue::back: Empty queue");
                return  std::queue<TemplType>::back();
        }

        void  pop (void)
        {
                if ( std::queue<TemplType>::empty() )
                        throw  std::out_of_range("NsQueue::pop: Empty queue");
                std::queue<TemplType>::pop();
        }
};

///  \c std::list with exceptions enabled.

template <class TemplType>
class  NsList : public std::list<TemplType> {

    public:

        NsList (void)
        {
        }

        NsList (TemplType item)
        {
                this->push_back(item);
        }

        NsList (TemplType item1, TemplType item2)
        {
                this->push_back(item1);
                this->push_back(item2);
        }

        typename std::list<TemplType>::reference
        front (void)
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::front: Empty list");
                return  std::list<TemplType>::front();
        }

        typename std::list<TemplType>::const_reference
        front (void)  const
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::front: Empty list");
                return  std::list<TemplType>::front();
        }

        typename std::list<TemplType>::reference
        back (void)
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::back: Empty list");
                return  std::list<TemplType>::back();
        }

        typename std::list<TemplType>::const_reference
        back (void)  const
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::back: Empty list");
                return  std::list<TemplType>::back();
        }

        void  pop_front (void)
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::pop_front: Empty list");
                std::list<TemplType>::pop_front();
        }

        void  pop_back (void)
        {
                if ( std::list<TemplType>::empty() )
                        throw  std::out_of_range("NsList::pop_back: Empty list");
                std::list<TemplType>::pop_back();
        }
};

///  An \c unordered_set.
template <class TemplType>
class  NsSet : public Ns_UNORDERED_SET<TemplType> {

};

#include "stack.h"

typedef  size_t  Ns_pointer_t;

typedef  NsDeque<NsIntVar *>    Ns_PointArray_t;

///  Array index type.
typedef  Ns_PointArray_t::size_type  NsIndex;

///  Array index maximum size.
const NsIndex   NsINDEX_INF = Ns_PointArray_t().max_size();

///  The Ns_HistoryId_t of an object can be used to see whether it is valid.

///  Each frame of the Ns_StackSearch NsProblemManager::searchNodes is
///   represented by its \a level.  Each \a level has its own valid \a id
///   that stops being valid when the frame is popped.
///   \internal
struct  Ns_HistoryId_t {

        ///  The depth of the node in the search tree.
        NsIndex  level;

        ///  The identity of the search node.
        NsUInt   id;
};

class NsProblemManager;
//class NsProblemManager::NsListOfVars;

class Ns_QueueItem;

///  Class describing the domain of a constrained variable as a bit-set.

///  A bit-set is used to hold its values.  If the <em>i</em>-th bit is
///   active, then the domain contains the value \a minDom + \a i.
///   \internal
class  Ns_BitSet {

    private:

        ///  The problem manager to which the domain belongs to.
        NsProblemManager  *pm;

        ///  The initial minimum value of the domain.
        NsInt  minDom;

        ///  Minimum value of the domain.
        NsInt  minVal;

        ///  Maximum value of the domain.
        NsInt  maxVal;

        ///  Number of the bits (active or inactive) of the bit-set.
        NsUInt  nBits;

        ///  Number of the active bits (values) of the domain.
        NsUInt  setCount;

        ///  An array consisting of machine words.  It contains the bits for the bitset.
        NsDeque<size_t>  machw;

        ///  The number of bits that a machine word can hold.
        static const NsUInt  MW_BITS = CHAR_BIT * sizeof(size_t);

        ///  The `timestamp' that can be used in chronological backtracking.
        Ns_HistoryId_t  lastSaveId;

    public:

        ///  Returns the `lastSaveId'.
        Ns_HistoryId_t&
        lastSaveHistoryId (void)
        {
                return  lastSaveId;
        }

        /////  Points to the item in the AC queue that refers to the variable.  If there is no such item, the pointer is null.
        //Ns_QueueItem  *queueItem;

        Ns_BitSet (void)
        {    }

        Ns_BitSet (NsProblemManager& pm_init,
                   const NsInt minDom_init, const NsInt maxDom_init);

        //Ns_IntDomain*
        Ns_BitSet *
        clone (void)
        {
                return  new Ns_BitSet(*this);
        }

        NsUInt
        size (void)  const
        {
                return  setCount;
                //return  domainStore.currentDomain().setCount;
        }

        bool
        isBound (void)  const
        {
                return  ( min()  ==  max() );
        }

        NsInt  previous (const NsInt toVal)  const;

        NsInt  next (const NsInt toVal)  const;

        NsInt  nextGap (const NsInt toVal)  const;

        NsInt
        max (void)  const
        {
                return  maxVal;
                //return  domainStore.currentDomain().maxVal;
        }

        NsInt
        min (void)  const
        {
                return  minVal;
                //return  domainStore.currentDomain().minVal;
        }

        bool  containsRange (const NsInt rangeMin, const NsInt rangeMax)  const;

        //bool  contains (const NsInt val);

        bool  removeRange (NsInt rangeMin, NsInt rangeMax);

        ///  Generic function to print a domain.
        friend std::ostream&  operator  << (std::ostream& os, const Ns_BitSet& domain);
        //friend std::ostream&  operator  << (std::ostream& os, Ns_IntDomain& domain);

        //friend class const_iterator;

        ///  Iterates through all the values of the domain, without changing them (\a const_iterator).

        class  const_iterator {

            private:

                const Ns_BitSet  *domain;
                NsInt  currVal;

            public:

                const_iterator (void)
                        : domain(0)
                {    }

                const_iterator (const Ns_BitSet&  domain_init)
                        : domain(&domain_init), currVal(domain->min())
                {    }

                bool  operator == (const const_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::==: Uninitialized `*this'");
                        return  ( currVal  ==  b.currVal );
                }

                bool  operator != (const const_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::!=: Uninitialized `*this'");
                        return  ! ( *this  ==  b );
                }

                NsInt  operator * (void)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::*: Uninitialized `*this'");
                        assert_Ns(currVal != NsPLUS_INF,
                                  "Ns_BitSet::const_iterator::*: Bad request `*(something.end())'");
                        return  currVal;
                }

                const_iterator&  end (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::end: Uninitialized `*this'");
                        currVal = NsPLUS_INF;
                        return  *this;
                }

                const_iterator&  operator ++ (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::++: Uninitialized `*this'");
                        currVal = domain->next(currVal);
                        return  *this;
                }

                const_iterator&  operator -- (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_iterator::--: Uninitialized `*this'");
                        currVal = domain->previous(currVal);
                        return  *this;
                }
        };

        ///  Iterates through all the values of the domain, without changing them, in reverse order (\a const_iterator).

        //friend class const_reverse_iterator;

        class  const_reverse_iterator {

            private:

                const Ns_BitSet  *domain;
                NsInt  currVal;

            public:

                const_reverse_iterator (void)
                        : domain(0)
                {    }

                const_reverse_iterator (const Ns_BitSet&  domain_init)
                        : domain(&domain_init), currVal(domain->max())
                {    }

                bool  operator == (const const_reverse_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::==: Uninitialized `*this'");
                        return  ( currVal  ==  b.currVal );
                }

                bool  operator != (const const_reverse_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::!=: Uninitialized `*this'");
                        return  ! ( *this  ==  b );
                }

                NsInt  operator * (void)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::*: Uninitialized `*this'");
                        assert_Ns(currVal != NsPLUS_INF,
                                  "Ns_BitSet::const_reverse_iterator::*: Bad request `*(something.end())'");
                        return  currVal;
                }

                const_reverse_iterator&  end (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::end: Uninitialized `*this'");
                        currVal = NsMINUS_INF;
                        return  *this;
                }

                const_reverse_iterator&  operator ++ (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::++: Uninitialized `*this'");
                        currVal = domain->previous(currVal);
                        return  *this;
                }

                const_reverse_iterator&  operator -- (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_reverse_iterator::--: Uninitialized `*this'");
                        currVal = domain->next(currVal);
                        return  *this;
                }
        };

        //friend class const_gap_iterator;

        ///  Iterates through all the missing values (gaps) of the domain [\a min..\a max], without changing them (\a const_iterator).

        class  const_gap_iterator {

            private:

                const Ns_BitSet  *domain;
                NsInt  currGap;

            public:

                const_gap_iterator (void)
                        : domain(0)
                {    }

                const_gap_iterator (const Ns_BitSet&  domain_init)
                        : domain(&domain_init), currGap(domain->nextGap(domain->min()))
                {    }

                bool  operator == (const const_gap_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_gap_iterator::==: Uninitialized `*this'");
                        return  ( currGap  ==  b.currGap );
                }

                bool  operator != (const const_gap_iterator& b)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_gap_iterator::!=: Uninitialized `*this'");
                        return  ! ( *this  ==  b );
                }

                NsInt  operator * (void)  const
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_gap_iterator::*: Uninitialized `*this'");
                        assert_Ns(currGap != NsPLUS_INF,
                                  "Ns_BitSet::const_gap_iterator::*: Bad request `*(something.end())'");
                        return  currGap;
                }

                const_gap_iterator&  end (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_gap_iterator::end: Uninitialized `*this'");
                        currGap = NsPLUS_INF;
                        return  *this;
                }

                const_gap_iterator&  operator ++ (void)
                {
                        assert_Ns(domain != 0,
                                  "Ns_BitSet::const_gap_iterator::++: Uninitialized `*this'");
                        currGap = domain->nextGap(currGap);
                        return  *this;
                }
        };

        const const_iterator
        begin (void)  const
        {
                return  const_iterator(*this);
        }

        const const_iterator
        end (void)  const
        {
                const_iterator  iterEnd(*this);
                return  iterEnd.end();
        }

        const const_reverse_iterator
        rbegin (void)  const
        {
                return  const_reverse_iterator(*this);
        }

        const const_reverse_iterator
        rend (void)  const
        {
                const_reverse_iterator  iterEnd(*this);
                return  iterEnd.end();
        }

        const const_gap_iterator
        gap_begin (void)  const
        {
                return  const_gap_iterator(*this);
        }

        const const_gap_iterator
        gap_end (void)  const
        {
                const_gap_iterator  iterEnd(*this);
                return  iterEnd.end();
        }
};

#if 0

///  A data structure to store the domain of a constrained variable.

///  In the previous version of <span style="font-variant: small-caps;">
///   Naxos </span> we used a BitSet in order to describe the domain of a
///   constrained variable.  Now we use this data structure that consists
///   of a binary search tree with intervals in its nodes.  Those intervals
///   represent gaps in the domain.  E.g.\ when we have the domain [9..17
///   44..101], it can be seen as three separate gaps:  [-oo..8], [18..43]
///   and [102..+oo].
///
///  More precisely, each node of the binary tree includes a stack of gap
///   intervals.  This stack shows the history of the gap interval of the
///   node during the search process.  E.g.\ the evolution of a gap
///   interval can be [18..43] -> [18..44] -> [10..100] -> ... (it is
///   increased).   It is used for backtracking reasons.  The top of this
///   stack should contain the gap that applies to the current `history
///   period'.
///
///  \attention
///
///  The gap interval binary tree has a completely different usage from the
///   search tree of the solver.  The first stores the domain of a variable
///   and the latter stores the problem states, so that we can backtrack to
///   one previous state.
///
///  However, Ns_GapIntervals use the solver's search tree in order to give
///   Ns_HistoryId_t 's to the gaps.  Using this property, we can see which
///   gaps are valid at a specific problem state (time).
///
///  \note
///  We focused on gaps, because as the search process for a solution to
///   the problem, we insert new gaps in the domains of the constrained
///   variables.
///   \internal

class  Ns_GapIntervals : public Ns_IntDomain {

    public:

        ///  A node of the binary tree of intervals.

        class  GapTreeNode_t {

            private:

                ///  A gap interval represented by its edges.

                struct  Gap_t {

                        ///  The `timestamp' for the gap that can be used in chronological backtracking.
                        Ns_HistoryId_t  historyId;

                        Gap_t (const NsProblemManager& pm, const NsInt start, const NsInt end);

                        ///  The beginning of the gap.
                        NsInt  gapStart;

                        ///  The end of the gap.
                        NsInt  gapEnd;
                };

                const NsProblemManager&  pm;

                NsStack<Gap_t>  gapStack;

                GapTreeNode_t  *leftNode, *rightNode;

            public:

                ///  Constructs an empty GapTreeNode_t.
                GapTreeNode_t (const NsProblemManager& pm_init)
                        : pm(pm_init), leftNode(0), rightNode(0)
                {   }

                ~GapTreeNode_t (void)
                {
                        if ( leftChild()   !=  0 )
                                delete  leftChild();
                        if ( rightChild()  !=  0 )
                                delete  rightChild();
                }

                ///  The left child of the node.
                GapTreeNode_t  *&leftChild (void)
                {
                        return  leftNode;
                }

                ///  The right child of the node.
                GapTreeNode_t  *&rightChild (void)
                {
                        return  rightNode;
                }

                ///  The left child of the node.
                GapTreeNode_t  *leftChild (void)  const
                {
                        return  leftNode;
                }

                ///  The right child of the node.
                GapTreeNode_t  *rightChild (void)  const
                {
                        return  rightNode;
                }

            private:

                void  refresh (void);

            public:

                bool  empty (void);

                NsInt  start (void)  const;

                NsInt  end (void)  const;

                void  insert (const NsInt gapStart, const NsInt gapEnd);
        };

    private:

        ///  Keeps track of the previous Ns_GapIntervals::size() calls, in order to accelerate the next size() calls.

        class  DomainSizeCache_t {

            private:

                ///  Describes the type of the frames that the stack of sizes of DomainSizeCache_t contains.

                struct  SizeFrame_t {

                        ///  The `timestamp' for the size that can be used in chronological backtracking.
                        Ns_HistoryId_t  historyId;

                        SizeFrame_t (const NsProblemManager& pm, const NsUInt size_init);

                        ///  The corresponding size to the historyId.
                        NsUInt  size;
                };

                const NsProblemManager&  pm;

                NsStack<SizeFrame_t>  sizeStack;

            public:

                ///  Constructs a DomainSizeCache_t.
                DomainSizeCache_t (const NsProblemManager& pm_init)
                        : pm(pm_init)
                {   }

                /////  Constructs a DomainSizeCache_t, with an uninitialized \a pm.
                //DomainSizeCache_t (void)
                //	: pm(0)
                //{	}

            private:

                void  refresh (void);

            public:

                bool  empty (void);

                NsUInt  cachedSize (void)  const;

                void  insert (NsUInt size);

                ///  When cachedSize() returns this constant, it means that the cache is dirty, i.e.\ it contains no valid size.
                static const NsUInt  DIRTY_SIZE = NsUPLUS_INF;
        };

        const NsProblemManager&  pm;

        ///  The root of the binary tree including the gaps.
        GapTreeNode_t  *rootGap;

        DomainSizeCache_t  domainSizeCache;

        GapTreeNode_t  *searchGap (GapTreeNode_t **gapNode,
                                   NsInt newStart,
                                   NsInt newEnd,
                                   const bool removeInterval,
                                   NsInt& nextGapValue);

        GapTreeNode_t  *searchGap (const NsInt newStart,
                                   const NsInt newEnd,
                                   const bool removeInterval);

    public:

        Ns_GapIntervals (const NsProblemManager& pm_init,
                         const NsInt min_init, const NsInt max_init);

        /////  Constructs an uninitialized instance.
        //Ns_GapIntervals (void)
        //	: pm(0), rootGap(0)
        //{	}

        /////  Copy-constructor that uses the = operator.
        //Ns_GapIntervals (const Ns_GapIntervals& domainOther)
        //	: Ns_IntDomain()
        //{
        //	*this  =  domainOther;
        //}

        Ns_IntDomain  *clone (void);

        Ns_GapIntervals&  operator = (const Ns_GapIntervals& domainOther);

        ~Ns_GapIntervals (void)
        {
                if ( rootGap  !=  0 )
                        delete  rootGap;
        }

        //  The domain size.
        NsUInt  size (void);

        ///  Returns \c true if the domain is empty.
        bool
        empty (void)
        {
                return  ( min()  ==  NsPLUS_INF );
        }

        ///  Returns \c true if the domain is bound, i.e.\ if it contains only one value.
        bool
        isBound (void)
        {
                return  ( min()  ==  max() );
        }

        ///  The minimum variable of the domain.
        NsInt
        min (void)
        {
                return  next(NsMINUS_INF);
        }

        ///  The maximum variable of the domain.
        NsInt
        max (void)
        {
                return  previous(NsPLUS_INF);
        }

        ///  The biggest value in the domain that is less than \a toVal.
        NsInt  previous (const NsInt toVal);

        ///  The smallest value in the domain that is greater than \a toVal.
        NsInt  next  (const NsInt toVal);

        /////  The biggest value \b not in the domain that is less than \a toVal.
        //NsInt  previousGap (const NsInt toVal);

        ///  The smallest value \b not in the domain that is greater than \a toVal.
        NsInt  nextGap (const NsInt toVal);

        bool  removeRange (NsInt rangeMin, NsInt rangeMax);

        bool  containsRange (const NsInt rangeMin, const NsInt rangeMax);

        void  toGraphFile (const char *fileName);

        const const_iterator  begin (void)
        {
                return  const_iterator(*this);
        }

        const const_iterator  end (void)
        {
                const_iterator  iterEnd(*this);
                return  iterEnd.end();
        }

        const const_reverse_iterator  rbegin (void)
        {
                return  const_reverse_iterator(*this);
        }

        const const_reverse_iterator  rend (void)
        {
                const_reverse_iterator  iterEnd(*this);
                return  iterEnd.end();
        }

        const const_gap_iterator  gap_begin (void)
        {
                return  const_gap_iterator(*this);
        }

        const const_gap_iterator  gap_end (void)
        {
                const_gap_iterator  iterEnd(*this);
                return  iterEnd.end();
        }
};
#endif

///////  Contains a list of constrained variables and the list of the pointers pointing to this.
//
/////  Contains a pointer to a list of constrained variables.
//
//struct  NsListOfVars  {
//
//	///  The list of the variables.
//	NsList<NsIntVar*>  variables;
//
//	///  List of pointers pointing to this list.
//	NsList< NsList<NsListOfVars>::iterator >  variablePointersToThis;
//
//	///  The pointer to the list.
//	NsList<NsListOfVars>::iterator  varsList;
//
//
//	///  An accessor to variables.
//	const NsList<NsIntVar*>&
//	vars (void)  const
//	{
//		return  ( ( variables.empty() ) ?
//				  varsList->vars()
//				: variables );
//	}
//};

class Ns_Constraint;
typedef  NsDeque<Ns_Constraint *>  Ns_constraints_array_t;

class Ns_Expression;

///  Represents a constrained variable.

class  NsIntVar {

    private:

        ///  Each constrained variable belongs to a specific NsProblemManager.
        NsProblemManager  *pm;

        ///  The domain of the constrained variable.
        Ns_BitSet  domain;

    public:

        ///  @{
        ///  \name  Iterators

        typedef  Ns_BitSet::const_iterator  const_iterator;

        const const_iterator
        begin (void)  const
        {
                return  const_iterator(domain);
        }

        const const_iterator
        end (void)  const
        {
                const_iterator  iterEnd(domain);
                return  iterEnd.end();
        }

        typedef  Ns_BitSet::const_reverse_iterator  const_reverse_iterator;

        const const_reverse_iterator
        rbegin (void)  const
        {
                return  const_reverse_iterator(domain);
        }

        const const_reverse_iterator
        rend (void)  const
        {
                const_reverse_iterator  iterEnd(domain);
                return  iterEnd.end();
        }

        typedef  Ns_BitSet::const_gap_iterator  const_gap_iterator;

        const const_gap_iterator
        gap_begin (void)  const
        {
                return  const_gap_iterator(domain);
        }

        const const_gap_iterator
        gap_end (void)  const
        {
                const_gap_iterator  iterEnd(domain);
                return  iterEnd.end();
        }

        ///  @}

        ///  Dummy constructor that allow the Solver's programmer to declare uninitialized NsIntVar 's.
        NsIntVar (void)
                : pm(0),
                  //domain(0),
                  arcsConnectedTo(0),
                  constraintNeedsRemovedValues(false),
                  queueItem(0)
#ifdef  Ns_LOCAL_SEARCH
                ,
                  lsVal(NsMINUS_INF),
                  lsIdx(NsINDEX_INF)
                  //lsInUnassignQueue(false)
#endif				 // Ns_LOCAL_SEARCH
        {   }

        NsIntVar (NsProblemManager& pm_init, const NsInt min_init, const NsInt max_init);

        NsIntVar (const Ns_Expression& expr);

        NsIntVar&  operator = (const Ns_Expression& expr);

        //  Methods that remove values from the domain follow...

        void  removeAll (void);

        void
        remove (const NsInt val)
        {
                return  remove(val, val);
                //removeSingle(val, 0);
        }

        ///  To remove a value, plus recording the constraint that made this removal.  If c==0 no constraint is recorded.
        bool
        removeSingle (const NsInt val, const Ns_Constraint *c)
        {
                return  removeRange(val, val, c);
        }

        void
        remove (const NsInt first, const NsInt last)
        {
                removeRange(first, last, 0);
        }

        bool
        removeRange (const NsInt first, const NsInt last, const Ns_Constraint *c)
        {
                bool  modifiedFoo;
                return  removeRange(first, last, c, modifiedFoo);
        }

        bool  removeRange (const NsInt first, const NsInt last, const Ns_Constraint *c, bool& modified);

        ///  Assigns a value to the constrained variable.
        void
        set (const NsInt val)
        {
                assert_Ns( val != NsMINUS_INF  &&  val != NsPLUS_INF ,
                           "NsIntVar::set: Cannot assign infinity");
                remove(NsMINUS_INF, val-1);
                remove(val+1,       NsPLUS_INF);
        }

        NsUInt
        size (void)  const
        {
                return  domain.size();
        }

        NsInt
        previous (const NsInt toVal)  const
        {
                return  domain.previous(toVal);
        }

        NsInt
        next (const NsInt toVal)  const
        {
                return  domain.next(toVal);
        }

        NsInt
        min (void)  const
        {
                return  domain.min();
        }

        NsInt
        max (void)  const
        {
                return  domain.max();
        }

        ///  If the constrained variable is instantiated, the method returns its value.
        NsInt
        value (void)  const
        {
                assert_Ns( isBound() ,
                           "NsIntVar::value: `*this': Not a bound NsIntVar");
                return  domain.min();
        }

        bool
        isBound (void)  const
        {
                return  domain.isBound();
        }

        bool
        contains (const NsInt val)  const
        {
                return  contains(val, val);
        }

        bool
        contains (const NsInt first, const NsInt last)  const
        {
                return  domain.containsRange(first, last);
        }

        friend std::ostream&  operator  << (std::ostream& os, const NsIntVar& Var);

        ///  The NsProblemManager that the variable belongs to.
        NsProblemManager&
        manager (void)  const
        {
                return  *pm;
        }

        /////  Graph of the current domain representation.
        //	void
        //toGraphFile (const char *fileName)  const
        //{
        //	domain.toGraphFile(fileName);
        //}

        ///  @{
        ///  \name  Auxiliary AC algorithm data-members

    private:

        ///  Pair of a constraint and the inconsistencies that has provoked.

        struct  ConstraintAndFailure {

                ///  The constraint.
                Ns_Constraint  *constr;

                ///  The inconsistencies that constr provoked.
                unsigned long   failures;

                ///  Constructor.
                ConstraintAndFailure (Ns_Constraint *constr_init)
                        : constr(constr_init), failures(0)
                {   }
        };

    public:

        ///  An array of the constraints that the variable is involved in.
        NsDeque<ConstraintAndFailure>  constraints;

        /////  An array of the constraints that the variable is involved in.  The constraints impose Bounds Consistency.
        //Ns_constraints_array_t  constraintsBoundsCons;

        /////  An array of the constraints that the variable is involved in.  The constraints impose Arc Consistency, instead of Bounds Consistency.  (Arc Consistency is stronger than Bounds Consistency.)
        //Ns_constraints_array_t  constraintsArcCons;

    private:

        ///  The number of the variables connected to this instance, via constraints.
        int  arcsConnectedTo;

        ///  True, if the variable is involved in an `Inverse' constraint, or another constraint that needs to know the values that have been removed from the variable (the w 's in the AC-5 Algorithm).
        bool  constraintNeedsRemovedValues;

    public:

        ///  Returns true if the variable is involved in an `Inverse' constraint, or another constraint that needs to know the values that have been removed from the variable.
        bool
        storeRemovedValues (void)  const
        {
                return  constraintNeedsRemovedValues;
                //return  ( ! constraintsArcCons.empty() );
        }

        ///  Adds a constraint to the collection of constraints of the variable.
        void  addConstraint (Ns_Constraint *c);

        /////  A start point for the constraints of the variable iterator.
        //const Ns_constraints_array_t::const_iterator
        //constraints_begin (void)  const
        //{
        //	return  constraints.begin();
        //}

        /////  An end point for the constraints of the variable iterator.
        //const Ns_constraints_array_t::const_iterator
        //constraints_end (void)  const
        //{
        //	return  constraints.end();
        //}

        ///  @}

        /////  @{
        /////  \name  Auxiliary AC algorithm data-members
        //
        //
        /////  True, if the variable has been already put into the AC queue.
        //bool  markedInAcQueue;

        ///  Points to the item in the AC queue that refers to the variable.  If there is no such item, the pointer is null.
        Ns_QueueItem  *queueItem;

        ///  The item in the AC queue that refers to the variable.  If there is no such item, the pointer is null.  This variable is stored in the domain for backtracking reasons.
        //	Ns_QueueItem*&
        //queueItem (void)
        //{
        //	return  domain.queueItem;
        //}

#ifdef  Ns_LOCAL_SEARCH

        ///  @{
        ///  \name  Local Search

    private:

        ///  The value assigned to the variable by Local Search.  lsVal==NsMINUS_INF means that the variable is not assigned.
        NsInt  lsVal;

        ///  The position of the variable in the array to be instantiated by Local Search.
        NsIndex  lsIdx;

        ///  True, if the variable is an auxiliary/intermediate variable.
        bool
        lsIsIntermediate (void)  const
        {
                return  ( lsIdx  ==  NsINDEX_INF );
        }

        /////  Used to mark the time we fixed the variable.
        //Ns_HistoryId_t  lsTimeWhenFixed;

        struct  VarPointerToPointer_t;

        ///  Describes a Variable-List_Iterator pair.

        struct  VarPointer_t {

                ///  Pointer to a constrained variable.
                NsIntVar  *Var;

                ///  Pointer to the item of the list Var->lsPointersToSupportTuples that points to this.
                NsList<VarPointerToPointer_t>::iterator  pointerToPointersToSupportTuples;

                ///  Constructor.
                VarPointer_t (NsIntVar *Var_init)
                        : Var(Var_init),
                          pointerToPointersToSupportTuples(
                                  --Var->lsPointersToSupportTuples.end() )
                {   }
        };

        /////  A list with the variables-values pairs that support (via constraints) the current assignment.
        //NsList<VarPointer_t>  lsSupportTuple;

        ///  A list with the support tuples that support a variable assignment.
        NsList< NsList<VarPointer_t> >  lsSupportTuples;

        void  findSupporters (NsList< NsList<NsIntVar *> >& supportTuples);

        ///  Describes a Variable-List_Iterator pair.

        struct  VarPointerToPointer_t {

                ///  Pointer to a constrained variable.
                NsIntVar  *Var;

                ///  Pointer to the item of the list Var->lsSupportTuples that points to this.
                NsList< NsList<VarPointer_t> >::iterator  pointerToSupportTuples;

                ///  Constructor.
                VarPointerToPointer_t (NsIntVar *Var_init)
                        : Var(Var_init),
                          pointerToSupportTuples(
                                  --Var->lsSupportTuples.end() )
                {   }
        };

        ///  A list with pointers to lsSupportTuples of other (intermediate) variables that are supported by this.
        NsList<VarPointerToPointer_t>  lsPointersToSupportTuples;

        ///  Pointer to the item of the list NsProblemManager::lsConflictingVars, if there is any that concerns `*this.'
        NsList<NsIntVar *>::iterator  lsPointerToConflictingVariablesItem;

    public:

        ///  Contains a list with pointers to items of lists that concern a specific (violated) constraint.

        struct  LsPointersToTuples_t {

                ///  Contains pointers to items of lists that concern a specific (violated) constraint.

                struct  LsPointersToTuplesOfVar_t {

                        ///  Pointer to a variables list item.
                        NsList< NsList<NsIntVar *> >::iterator  varsListItem;

                        ///  Pointer to a pointers list item.
                        NsList< NsList<LsPointersToTuples_t>::iterator >::iterator  pointersListItem;

                        ///  Constructor.
                        LsPointersToTuplesOfVar_t (
                                NsList< NsList<NsIntVar *> >::iterator  varsListItem_init,
                                NsList< NsList<LsPointersToTuples_t>::iterator >::iterator  pointersListItem_init)
                                : varsListItem(varsListItem_init),
                                  pointersListItem(pointersListItem_init)
                        {   }
                };

                ///  A list containing pointers (to lists that are included in NsIntVar's).
                NsList<LsPointersToTuplesOfVar_t>  tuple;

                ///  Pointer to a variables list item (of NsProblemManager).
                NsList< NsList<NsIntVar *> >::iterator  varsListItem;

                /////  The ID of the violated constraint.
                //NsIndex  constraint_ID;
        };

    private:

        ///  A list containing the violated constraints (a violated constraint is practically the list of its variables) that `*this' takes part in.
        NsList< NsList<NsIntVar *> >  lsViolatedConstrs;

        ///  Auxiliary list for lsViolatedConstraints: contains pointers to a list with other entities that are involved in the corresponding violated constraints.
        NsList< NsList<LsPointersToTuples_t>::iterator >  lsPointersToTuples;

        /////  A list containing the IDs of the violated constraints that `*this' takes part in.
        //NsSet<NsIndex>  lsViolatedConstrsIDs;

        void  lsDeleteSupportTuples (void);

        void  lsDeleteAssignmentDependencies (void);

    public:

        ///  An accessor to lsViolatedConstrs.
        const NsList< NsList<NsIntVar *> >&
        lsViolatedConstraints (void)  const
        {
                return  lsViolatedConstrs;
        }

        /////  An accessor to lsViolatedConstrsIDs.
        //const NsSet<NsIndex>&
        //lsViolatedConstraintsIDs (void)  const
        //{
        //	return  lsViolatedConstrs;
        //}

        ///  True, if there has been assigned a value (by Local Search) to the variable.
        bool
        lsIsBound (void)  const
        {
                return  ( lsVal  !=  NsMINUS_INF );
        }

        ///  If the constrained variable is instantiated, the method returns its value.
        NsInt
        lsValue (void)  const
        {
                assert_Ns( lsIsBound() ,
                           "NsIntVar::lsValue: `*this': Not a bound NsIntVar" );
                return  lsVal;
        }

        ///  Used when we want to declare that the variable is \b not an auxiliary/intermediate one.
        void
        lsNotIntermediate (const NsIndex index)
        {
                lsIdx  =  index;
        }

        ///  Returns the position of the variable in the array to be instantiated by Local Search.
        NsIndex
        lsIndex (void)  const
        {
                assert_Ns( ! lsIsIntermediate() ,
                           "NsIntVar::lsIndex: Variable is intermediate" );
                return  lsIdx;
        }

        /////  True if the variable is going to be unassigned.
        //bool  lsInUnassignQueue;

        //void  lsSet (const NsInt val);

        void  lsSet (const NsInt val,
                     const NsList<NsIntVar *> supportTuple=
                             NsList<NsIntVar *>(),
                     const Ns_Constraint *constrFired=0);

        ///  Undo the assignment of the variable.
        void  lsUnset (void);

        //void  lsUnsetCommit (void);

        /////  Retains the current value of the variable, or un-assigns it if it is not supported.
        //	void
        //lsRefresh (void)
        //{
        //	for (NsList<VarPointer_t>::const_iterator var_val=lsSupportTuple.begin();
        //			var_val != lsSupportTuple.end();
        //			++var_val)
        //	{
        //		if ( var_val->Var->lsValue()  !=  var_val->value )
        //		{
        //			lsUnsetCommit();
        //			return;
        //		}
        //	}
        //}

        ///  @}
#endif					 // Ns_LOCAL_SEARCH

        void  transparent (void);
};

inline std::ostream&
operator  << (std::ostream& os, const NsIntVar& Var)
{
        return  (os << Var.domain);
}

class Ns_ExpressionArray;

///  A flexible array data type, to hold constrained variables (NsIntVar 's).

class  NsIntVarArray {

    private:

        Ns_PointArray_t  PointArray;

        bool  addedConstraint;

    public:

        NsIntVarArray (void)
                : addedConstraint(false)
        {    }

        NsIntVarArray (const Ns_ExpressionArray& expr);

        NsIntVarArray& operator = (const Ns_ExpressionArray& expr);

        ///  Signifies that a constraint has been imposed on the array.
        void
        addConstraint (void)
        {
                addedConstraint = true;
        }

        void  push_front (const NsIntVar& Var);

        void  push_front (NsIntVar& Var)
        {
                assert_Ns( ! addedConstraint ,  "NsIntVarArray::push_front: Cannot add another variable, because a constraint has been already imposed on the array");
                PointArray.push_front( &Var );
        }

        void  push_front (const Ns_Expression& expr);

        void  push_back (const NsIntVar& Var);

        void  push_back (NsIntVar& Var)
        {
                assert_Ns( ! addedConstraint ,  "NsIntVarArray::push_back: Cannot add another variable, because a constraint has been already imposed on the array");
                PointArray.push_back( &Var );
        }

        void  push_back (const Ns_Expression& expr);

        class  const_iterator;

        ///  Iterator that iterates through the constrained variables of the array.
        class  iterator {

            private:

                Ns_PointArray_t  *PointArr;

                Ns_PointArray_t::iterator  it;

            public:

                friend class const_iterator;

                iterator (void)
                        : PointArr(0)
                {    }

                iterator (Ns_PointArray_t& PointArr_init)
                        : PointArr(&PointArr_init),
                          it(PointArr->begin())
                {    }

                bool  operator == (const iterator& b)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::iterator::==: Uninitialized `*this'");
                        return  ( it  ==  b.it );
                }

                bool  operator != (const iterator& b)  const
                {
                        return  ! ( *this  ==  b );
                }

                NsIntVar&  operator * (void)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::iterator::*: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::*: Bad request `*(something.end())'");
                        return  **it;
                }

                NsIntVar  *operator -> (void)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::iterator::*: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::*: Bad request `*(something.end())'");
                        return  *it;
                }

                iterator&  end (void)
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::iterator::end: Uninitialized `*this'");
                        it = PointArr->end();
                        return  *this;
                }

                iterator&  operator ++ (void)
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::iterator::++: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::iterator::end: Bad request `++(something.end())'");
                        ++it;
                        return  *this;
                }
        };

        iterator
        begin (void)
        {
                return  iterator(PointArray);
        }

        iterator
        end (void)
        {
                iterator  iterEnd(PointArray);
                return  iterEnd.end();
        }

        ///  Iterator that iterates through the constrained variables of the array (without modifying them).
        class  const_iterator {

            private:

                const Ns_PointArray_t  *PointArr;

                Ns_PointArray_t::const_iterator  it;

            public:

                const_iterator (void)
                        : PointArr(0)
                {    }

                const_iterator (const Ns_PointArray_t& PointArr_init)
                        : PointArr(&PointArr_init),
                          it(PointArr->begin())
                {    }

                const_iterator (const iterator iter)
                        : PointArr(iter.PointArr),
                          it(iter.it)
                {    }

                const_iterator&
                operator = (const iterator iter)
                {
                        PointArr = iter.PointArr;
                        it = iter.it;
                        return  *this;
                }

                bool  operator == (const const_iterator& b)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::const_iterator::==: Uninitialized `*this'");
                        return  ( it == b.it );
                }

                bool  operator != (const const_iterator& b)  const
                {
                        return  ! ( *this == b );
                }

                const NsIntVar&  operator * (void)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");
                        return  **it;
                }

                const NsIntVar  *operator -> (void)  const
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");
                        return  *it;
                }

                const_iterator&  end (void)
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::const_iterator::end: Uninitialized `*this'");
                        it = PointArr->end();
                        return  *this;
                }

                const_iterator&  operator ++ (void)
                {
                        assert_Ns(PointArr != 0,
                                  "NsIntVarArray::const_iterator::++: Uninitialized `*this'");
                        assert_Ns(it != PointArr->end(),
                                  "NsIntVarArray::const_iterator::end: Bad request `++(something.end())'");
                        ++it;
                        return  *this;
                }
        };

        const_iterator
        begin (void)  const
        {
                return  const_iterator(PointArray);
        }

        const_iterator
        end (void)  const
        {
                const_iterator  iterEnd(PointArray);
                return  iterEnd.end();
        }

        NsIntVar&
        operator  [] (const NsIndex i)
        {
                return  *PointArray[i];
        }

        const NsIntVar&
        operator  [] (const NsIndex i)  const
        {
                return  *PointArray[i];
        }

        NsIntVar&
        front (void)
        {
                return  *PointArray.front();
        }

        NsIntVar&
        back (void)
        {
                return  *PointArray.back();
        }

        NsIndex
        size (void)  const
        {
                return  PointArray.size();
        }

        bool
        empty (void)  const
        {
                return  PointArray.empty();
        }

#ifdef  Ns_LOCAL_SEARCH

        ///  @{
        ///  \name  Local Search

        ///  Signifies that the variables belonging to the array will be instantiated by a Local Search method.
        void
        lsLabeling (void)
        {
                for (NsIndex i=0;  i < size();  ++i)
                        (*this)[i].lsNotIntermediate(i);
                //  `Lock' the array in order to disallow the
                //   addition of new variables to it.
                addConstraint();
        }

        ///  @}
#endif  // Ns_LOCAL_SEARCH
};

std::ostream&
operator  << (std::ostream& os, const NsIntVarArray& VarArr);

void  Ns_ternaryConstraintToGraphFile (std::ofstream& fileConstraintsGraph,
                                       const NsIntVar *VarX,
                                       const NsIntVar *VarY,
                                       const NsIntVar *VarZ,
                                       const Ns_Constraint *constr,
                                       const char *constrName,
                                       const bool sourceLabels);

void  Ns_globalConstraintToGraphFile (std::ofstream& fileConstraintsGraph,
                                      const NsIntVar *VarX,
                                      const NsIntVarArray *VarArr,
                                      const Ns_Constraint *constr,
                                      const char *constrName);

void  Ns_arrayConstraintToGraphFile (std::ofstream& fileConstraintsGraph,
                                     const NsIntVarArray *VarArr,
                                     const Ns_Constraint *constr,
                                     const char *constrName);

void  Ns_inverseConstraintToGraphFile (std::ofstream& fileConstraintsGraph,
                                       const NsIntVarArray *VarArr,
                                       const NsIntVarArray *VarArrInv,
                                       const Ns_Constraint *constr);

///  Abstract class that represents a constraint between constrained variables.

///  \internal
class  Ns_Constraint {

    public:

        ///  The `timestamp' (current removal ID) when the last check of this constraint took place.
        unsigned long  lastConstraintCheckTime;

        ///  Constructor.
        Ns_Constraint (void)
                : lastConstraintCheckTime(0),
                  revisionType(BOUNDS_CONSISTENCY)
        {   }

        ///  @{
        ///  \name  AC algorithm methods

        virtual void  ArcCons      (void) = 0;

        virtual void  LocalArcCons (Ns_QueueItem& Qitem) = 0;

#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons (NsIntVar *varFired)
        {
                // to suppress warnings
                varFired  =  varFired;
                throw  NsException("Ns_Constraint::lsFixedCons: Unimplemented");
        }
#endif  // Ns_LOCAL_SEARCH

        ///  @}

        ///  The number of the variables involved in the constraint.
        virtual int   varsInvolvedIn (void)  const = 0;

        ///  Description of the consistency type that a `revision' function for a constraint can impose.
        enum  ConsistencyType {

                ///  The revision function needs to know the value that has been removed from the domain (e.g.\ see Ns_ConstrInverse).
                VALUE_CONSISTENCY=1,

                ///  The revision function does not need to know the removed values (like VALUE_CONSISTENCY).  It imposes bounds-consistency.
                BOUNDS_CONSISTENCY,

                ///  Like BOUNDS_CONSISTENCY, but revision is imposed in both directions.  E.g.\ Revision(i,j) is equivalent to Revision(j,i), where i, j are variables.
                BIDIRECTIONAL_CONSISTENCY
        };

        ///  Description of the type of revision function (LocalArcCons) for the constraint.
        ConsistencyType  revisionType;

        ///  Writes a constraint-edge representation into a graph file, with a format supported by Graphviz.
        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                assert_Ns( fileConstraintsGraph ,  "Ns_Constraint::toGraphFile: Problem writing to file");
                fileConstraintsGraph << "\n\t"
                                     << "//  Unimplemented constraint representation\n";
        }

        // The destructor of an abstract class should be virtual.
        virtual ~Ns_Constraint (void)  {   }

        /////  The priority of the constraint inside the propagation queue.
        //const NsIndex  PRIORITY;
};

class Ns_ConstrXinDomain : public Ns_Constraint {

    private:

        NsIntVar  *VarX;

        NsInt  min;
        NsDeque<NsInt>  *domainPrevious;
        NsDeque<NsInt>  *domainNext;
        bool  arraysAllocated;

    public:

        Ns_ConstrXinDomain (NsIntVar *X,
                            const NsDeque<NsInt>& domain,
                            NsDeque<NsInt> *domainPrevious_init,
                            NsDeque<NsInt> *domainNext_init);

        virtual  ~Ns_ConstrXinDomain (void)
        {
                if ( arraysAllocated ) {
                        delete  domainPrevious;
                        delete  domainNext;
                }
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 1;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\t//Var"
                                     << VarX << " -> Dom"
                                     << &domainPrevious
                                     << "   [label=\"dom\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXlessthanY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;

    public:

        Ns_ConstrXlessthanY (NsIntVar *X, NsIntVar *Y)
                : VarX(X), VarY(Y)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXlessthanY::Ns_ConstrXlessthanY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarX << " -> Var" << VarY
                                     << "   [label=\"<\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif  // Ns_LOCAL_SEARCH
};

class Ns_ConstrXlesseqthanY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;

    public:

        Ns_ConstrXlesseqthanY (NsIntVar *X, NsIntVar *Y)
                : VarX(X), VarY(Y)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXlesseqthanY::Ns_ConstrXlesseqthanY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarX << " -> Var" << VarY
                                     << "   [label=\"<=\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

class Ns_ConstrXeqYplusC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqYplusC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqYplusC::Ns_ConstrXeqYplusC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\""
                                     << ( (C >= 0) ? "+" : "" ) << C << "\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

class Ns_ConstrXeqCminusY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqCminusY (NsIntVar *X, const NsInt C_init, NsIntVar *Y)
                : VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqCminusY::Ns_ConstrXeqCminusY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"" << C << "-y\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYtimesC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqYtimesC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : /*Ns_Constraint(1),*/ VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqYtimesC::Ns_ConstrXeqYtimesC: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns( C != 0,  "Ns_ConstrXeqYtimesC::Ns_ConstrXeqYtimesC: If C==0 the constraint has no reason to exist");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"*"
                                     << ( (C >= 0) ? "" : "(" )
                                     << C
                                     << ( (C >= 0) ? "" : ")" )
                                     << "\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

//  The following constraint is somehow `stronger' than the simple `X == Y + C*Z'.
//   It requires some special conditions, that allow the efficient application of
//   the pure arc-consistency--i.e. not only bounds consistency.

class Ns_ConstrXeqYplusCZspecial : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;
        NsInt  C;

    public:

        Ns_ConstrXeqYplusCZspecial (NsIntVar *X, NsIntVar *Y, const NsInt C_init, NsIntVar *Z)
                : VarX(X), VarY(Y), VarZ(Z), C(C_init)
        {
                revisionType  =  VALUE_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrXeqYplusCZspecial::Ns_ConstrXeqYplusCZspecial: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns(X->min() >= 0,
                          "Ns_ConstrXeqYplusCZspecial::Ns_ConstrXeqYplusCZspecial: Special condition required:  X >= 0");
                assert_Ns(0 <= Y->min()  &&  Y->max() < C,
                          "Ns_ConstrXeqYplusCZspecial::Ns_ConstrXeqYplusCZspecial: Special condition required:  0 <= Y < C");
                assert_Ns(C > 0,
                          "Ns_ConstrXeqYplusCZspecial::Ns_ConstrXeqYplusCZspecial: Condition required:  C > 0");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        //virtual bool  needsRemovedValues (void)  const
        //{
        //	return  true;
        //}

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this,
                        "y+c*z special", false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYplusZ: public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;

    public:

        Ns_ConstrXeqYplusZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
                : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrXeqYplusZ::Ns_ConstrXeqYplusZ: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this, "+", false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

class Ns_ConstrXeqYtimesZ: public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;

    public:

        Ns_ConstrXeqYtimesZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
                : VarX(X), VarY(Y), VarZ(Z)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrXeqYtimesZ::Ns_ConstrXeqYtimesZ: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this, "*", false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYdivC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqYdivC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqYdivC::Ns_ConstrXeqYdivC: All the variables of a constraint must belong to the same NsProblemManager");
                //assert_Ns(Y->min() >= 0,
                //		"Ns_ConstrXeqYdivC::Ns_ConstrXeqYdivC: Special condition required:  Y >= 0");
                assert_Ns(C != 0,
                          "Ns_ConstrXeqYdivC::Ns_ConstrXeqYdivC: Special condition required:  C != 0");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"/"
                                     << ( (C >= 0) ? "" : "(" )
                                     << C
                                     << ( (C >= 0) ? "" : ")" )
                                     << "\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqCdivY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqCdivY (NsIntVar *X, const NsInt C_init, NsIntVar *Y)
                : VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager() ,  "Ns_ConstrXeqCdivY::Ns_ConstrXeqCdivY: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns(Y->min() >= 0,
                          "Ns_ConstrXeqCdivY::Ns_ConstrXeqYdivC: Special condition required:  Y > 0");
                assert_Ns(C > 0,
                          "Ns_ConstrXeqCdivY::Ns_ConstrXeqCdivY: Special condition required:  C > 0");
                VarY->remove( 0 );
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"" << C << "/y\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYmodC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrXeqYmodC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                revisionType  =  VALUE_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqYmodC::Ns_ConstrXeqYmodC: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns(Y->min() >= 0,
                          "Ns_ConstrXeqYmodC::Ns_ConstrXeqYmodC: Special condition required:  Y >= 0");
                assert_Ns(C > 0,
                          "Ns_ConstrXeqYmodC::Ns_ConstrXeqYmodC: Special condition required:  C > 0");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        //virtual bool  needsRemovedValues (void)  const
        //{
        //	return  true;
        //}

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"%"
                                     << ( (C >= 0) ? "" : "(" )
                                     << C
                                     << ( (C >= 0) ? "" : ")" )
                                     << "\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlessthanC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYlessthanC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYlessthanC::Ns_ConstrMetaXeqYlessthanC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y<" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlessthanZ : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;

    public:

        Ns_ConstrMetaXeqYlessthanZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
                : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrMetaXeqYlessthanZ::Ns_ConstrMetaXeqYlessthanZ: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this, "<", true);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlesseqthanC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYlesseqthanC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYlesseqthanC::Ns_ConstrMetaXeqYlesseqthanC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y<=" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYlesseqthanZ : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;

    public:

        Ns_ConstrMetaXeqYlesseqthanZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
                : VarX(X), VarY(Y), VarZ(Z)
        {
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrMetaXeqYlesseqthanZ::Ns_ConstrMetaXeqYlesseqthanZ: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this, "<=", true);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYgreaterthanC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYgreaterthanC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYgreaterthanC::Ns_ConstrMetaXeqYgreaterthanC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y>" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYgreatereqthanC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYgreatereqthanC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYgreatereqthanC::Ns_ConstrMetaXeqYgreatereqthanC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y>=" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYeqC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYeqC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYeqC::Ns_ConstrMetaXeqYeqC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y=" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYeqZ : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;
        const bool  neg;
        // If `neg==true' the constraint becomes 'Ns_ConstrMetaXeqY neq Z'.

    public:

        Ns_ConstrMetaXeqYeqZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
                : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrMetaXeqYeqZ::Ns_ConstrMetaXeqYeqZ: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this,
                        ( ( neg ) ? "(y!=z)" : "(y=z)" ),
                        false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrMetaXeqYneqC : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        NsInt  C;

    public:

        Ns_ConstrMetaXeqYneqC (NsIntVar *X, NsIntVar *Y, const NsInt C_init)
                : VarX(X), VarY(Y), C(C_init)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrMetaXeqYneqC::Ns_ConstrMetaXeqYneqC: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"(y!=" << C << ")\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

//class Ns_ConstrMetaXeqYneqZ : public Ns_Constraint  {
//	private:
//		NsIntVar  *VarX, *VarY, *VarZ;
//
//	public:
//		Ns_ConstrMetaXeqYneqZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
//			: VarX(X), VarY(Y), VarZ(Z)
//		{
//			assert_Ns( VarX->manager() == VarY->manager()  &&  VarY->manager() == VarZ->manager(),  "Ns_ConstrMetaXeqYneqZ::Ns_ConstrMetaXeqYneqZ: All the variables of a constraint must belong to the same NsProblemManager");
//		}
//
//		virtual int   varsInvolvedIn (void)  const    {  return 3;  }
//
//		virtual void  ArcCons      (void)  const;
//		virtual void  LocalArcCons (Ns_QueueItem& Qitem)  const;
//};

class Ns_ConstrXeqYandZ : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;
        const bool  neg;
        // If `neg==true' the constraint becomes 'Ns_ConstrXeqY nand Z'.

    public:

        Ns_ConstrXeqYandZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
                : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrXeqYandZ::Ns_ConstrXeqYandZ: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1  &&  0 <= VarZ->min() && VarZ->max() <= 1,  "Ns_ConstrXeqYandZ::Ns_ConstrXeqYandZ: All the variables should be \"boolean\"");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this,
                        ( ( neg ) ? "(y NAND z)" : "(y AND z)" ),
                        false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYorZ : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY, *VarZ;
        const bool  neg;
        // If `neg==true' the constraint becomes 'Ns_ConstrXeqY nor Z'.

    public:

        Ns_ConstrXeqYorZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
                : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
        {
                assert_Ns( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager(),  "Ns_ConstrXeqYorZ::Ns_ConstrXeqYorZ: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1  &&  0 <= VarZ->min() && VarZ->max() <= 1,  "Ns_ConstrXeqYorZ::Ns_ConstrXeqYorZ: All the variables should be \"boolean\"");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 3;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_ternaryConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarY, VarZ, this,
                        ( ( neg ) ? "(y NOR z)" : "(y OR z)" ),
                        false);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXorY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;
        const bool  neg;
        // If `neg==true' the constraint becomes 'Constr not X and not Y'.

    public:

        Ns_ConstrXorY (NsIntVar *X, NsIntVar *Y, const bool pos)
                : VarX(X), VarY(Y), neg(!pos)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXorY::Ns_ConstrXorY: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1,  "Ns_ConstrXorY::Ns_ConstrXorY: All the variables should be \"boolean\"");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarX << " -> Var" << VarY
                                     << "   [label=\""
                                     << ( ( neg ) ? "NOR" : "OR" )
                                     << "\", arrowhead=none];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqMin : public Ns_Constraint {

    private:

        NsIntVar  *VarX;
        NsIntVarArray  *VarArr;

    public:

        Ns_ConstrXeqMin (NsIntVar *X, NsIntVarArray *VarArr_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return  (1 + VarArr->size());
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_globalConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarArr, this,
                        "min");
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqMax : public Ns_Constraint {

    private:

        NsIntVar  *VarX;
        NsIntVarArray  *VarArr;

    public:

        Ns_ConstrXeqMax (NsIntVar *X, NsIntVarArray *VarArr_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return  (1 + VarArr->size());
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_globalConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarArr, this,
                        "max");
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqSum : public Ns_Constraint {

    private:

        NsIntVar  *VarX;
        NsIntVarArray  *VarArr;
        NsIndex  start, length;

    public:

        Ns_ConstrXeqSum (NsIntVar *X, NsIntVarArray *VarArr_init);
        Ns_ConstrXeqSum (NsIntVar *X, NsIntVarArray *VarArr_init,
                         const NsIndex start_init, const NsIndex length_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return  (1 + length);
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_globalConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarX, VarArr, this,
                        "sum");
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;

    public:

        Ns_ConstrXeqY (NsIntVar *X, NsIntVar *Y)
                : VarX(X), VarY(Y)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqY::Ns_ConstrXeqY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarX << " -> Var" << VarY
                                     << "   [arrowhead=none];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXneqY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;

    public:

        Ns_ConstrXneqY (NsIntVar *X, NsIntVar *Y)
                : VarX(X), VarY(Y)
        {
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXneqY::Ns_ConstrXneqY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarX << " -> Var" << VarY
                                     << "   [label=\"!=\", arrowhead=none];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

class Ns_ConstrXeqAbsY : public Ns_Constraint {

    private:

        NsIntVar  *VarX, *VarY;

    public:

        Ns_ConstrXeqAbsY (NsIntVar *X, NsIntVar *Y)
                : VarX(X), VarY(Y)
        {
                revisionType  =  BIDIRECTIONAL_CONSISTENCY;
                assert_Ns( &VarX->manager() == &VarY->manager(),  "Ns_ConstrXeqAbsY::cononstrXeqAbsY: All the variables of a constraint must belong to the same NsProblemManager");
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarY << " -> Var" << VarX
                                     << "   [label=\"abs\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrAllDiff : public Ns_Constraint {

    private:

        NsIntVarArray  *VarArr;

    public:

        Ns_ConstrAllDiff (NsIntVarArray *VarArr_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return VarArr->size();
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_arrayConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarArr, this, "!=");
        }

        //virtual bool  needsRemovedValues (void)  const
        //{
        //	return  true;
        //}

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
#ifdef  Ns_LOCAL_SEARCH
        virtual void  lsFixedCons  (NsIntVar *varFired);
#endif					 // Ns_LOCAL_SEARCH
};

class Ns_ConstrAllDiffStrong : public Ns_Constraint {

    public:

        //  `groupedNsIntVar', as the name suggests, is a class that
        //   extends `NsIntVar', by adding the information concerning
        //   the id of the group taht the constrained variable belongs to.

        class  groupedNsIntVar {

            public:

                NsIntVar&  Var;

            private:

                NsIntVar  vGroup;

            public:

                typedef  NsInt  group_t;

                static const group_t  FIRST_GROUP  =  NsMINUS_INF + 1;

                groupedNsIntVar (NsIntVar& Var_init)
                        : Var(Var_init), vGroup(Var.manager(), FIRST_GROUP, NsPLUS_INF-1)  {   }

                group_t
                group (void)
                {
                        return  vGroup.min();
                }

                void
                setGroup (const group_t groupVal)
                {
                        assert_Ns( vGroup.contains(groupVal-1),  "groupedNsIntVar::setGroup: `groupVal-1' is not contained in`vGroup'" );
                        assert_Ns( vGroup.removeRange(NsMINUS_INF, groupVal-1, 0),  "groupedNsIntVar::setGroup: Could not change group to `groupVal'" );
                        assert_Ns( group() == groupVal,  "groupedNsIntVar::setGroup: Not succesful change of group to `groupVal'" );
                }

                bool  removeDomain (const NsIntVar& V, const Ns_Constraint *c);
        };

    private:

        NsDeque<groupedNsIntVar>  VarArr;

        typedef  Ns_UNORDERED_MAP<Ns_pointer_t,groupedNsIntVar *>  VarPointerGroup_t;

        VarPointerGroup_t  VarPointerGroup;

        //NsIntVarArray  VarArrGroup;
        //NsIntVar       vGroupCounter;

        const unsigned long   Capacity;

    public:

        Ns_ConstrAllDiffStrong (NsIntVarArray *VarArr_init, const unsigned long Cap);

        virtual int   varsInvolvedIn (void)  const
        {
                return VarArr.size();
        }

        //	virtual void
        //toGraphFile (std::ofstream& fileConstraintsGraph)  const
        //{
        //	Ns_arrayConstraintToGraphFile(
        //		fileConstraintsGraph,
        //		&VarArr, this, "!= strong");
        //}

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrCount : public Ns_Constraint {

    private:

        NsIntVarArray  *VarArr;
        //const NsDeque<NsInt>&  Values;
        //const NsDeque<NsInt>&  Occurrences;

        /////  vCount[i].max() is the number of the occurrences of Values[i] in VarArr.
        //NsIntVarArray  vCount;

        /////  vMinValueIndex.min() is the minimum index i in a sorted array Values[i], with the property that the occurrences of the corresponding value is greater than zero.
        //NsIntVar  vMinValueIndex;

        /////  vMaxValueIndex.max() is the vMinValueIndex.min() maximum equivalent.
        //NsIntVar  vMaxValueIndex;

        typedef  Ns_UNORDERED_MAP<Ns_pointer_t,NsIndex>  VarIndex_t;

        VarIndex_t  VarIndex;

    public:
        typedef  Ns_UNORDERED_MAP<NsInt,NsIndex>  ValueIndex_t;

    private:
        ValueIndex_t  ValueIndex;

    public:
        struct  ValueOccurrence_t {

                NsInt  value;

                NsInt  occurrence;

                const NsDeque<NsIndex>  *split_positions;

                NsIntVarArray  vCount;

                ValueOccurrence_t (const NsInt value_init)
                        : value(value_init)
                {
                }

                ValueOccurrence_t (const NsInt value_init,
                                   const NsInt occurrence_init,
                                   NsProblemManager& pm)
                        : value(value_init),
                          occurrence(occurrence_init),
                          split_positions(0)
                {
                        assert_Ns( occurrence >= 0 , "Ns_ConstrCount::ValueOccurrence_t::ValueOccurrence_t: negative value in `occurrence'");
                        vCount.push_back( NsIntVar(pm, 0, occurrence) );
                }

                ValueOccurrence_t (const NsInt value_init,
                                   const NsInt occurrence_init,
                                   NsProblemManager& pm,
                                   const NsDeque<NsIndex>& split_positions_init,
                                   const NsIndex Split)
                        : value(value_init),
                          occurrence(occurrence_init),
                          split_positions(&split_positions_init)
                {
                        assert_Ns( occurrence >= 0 , "Ns_ConstrCount::ValueOccurrence_t::ValueOccurrence_t: negative value in `occurrence'");
                        for (NsIndex i=0;  i < split_positions->size();  ++i)
                                vCount.push_back( NsIntVar(pm, 0, Split) );
                        vCount.push_back( NsIntVar(pm, 0, occurrence%Split) );
                        if ( split_positions->size()  ==  0 )
                                split_positions  =  0;
                }

                NsIndex
                splitIndexForPosition (NsIndex i)
                {
                        if ( split_positions  ==  0 )
                                return  0;
                        NsDeque<NsIndex>::const_iterator  cit =
                                lower_bound(
                                        split_positions->begin(),
                                        split_positions->end(),
                                        i);
                        return  ( cit - split_positions->begin() );
                }

                bool
                operator  <  (const ValueOccurrence_t& Y)  const
                {
                        return  ( value  <  Y.value );
                }
        };

    private:
        NsDeque<ValueOccurrence_t>  ValuesOccurrences;
        const NsIndex  Split;
        const NsIndex  Dwin;

    public:

        Ns_ConstrCount (NsIntVarArray *VarArr_init,
                        const NsDeque<NsInt>& Values,
                        const NsDeque<NsInt>& Occurrences,
                        const NsDeque< NsDeque<NsIndex> >& SplitPositions,
                        const NsIndex Split_init,
                        const NsIndex Dwin_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return VarArr->size();
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_arrayConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarArr, this, "count");
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrInverse : public Ns_Constraint {

    private:

        NsIntVarArray  *VarArrInv, *VarArr;

        NsIntVarArray&  VArrInv, &VArr;

        struct  ArrayIndex_t {

                const bool  InInverse;

                const NsIndex  index;

                ArrayIndex_t (const bool InInverse_init,
                              const NsIndex index_init)
                        : InInverse(InInverse_init),
                          index(index_init)
                {   }
        };

        typedef  Ns_UNORDERED_MAP<Ns_pointer_t,ArrayIndex_t>  VarArrayIndex_t;

        VarArrayIndex_t  VarArrayIndex;

    public:

        Ns_ConstrInverse (NsIntVarArray *VarArrInv_init, NsIntVarArray *VarArr_init);

        virtual int   varsInvolvedIn (void)  const
        {
                return  (VarArrInv->size() + VarArr->size());
        }

        //virtual bool  needsRemovedValues (void)  const
        //{
        //	return  true;
        //}

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                Ns_inverseConstraintToGraphFile(
                        fileConstraintsGraph,
                        VarArr, VarArrInv, this);
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrElement : public Ns_Constraint {

    private:

        NsIntVar  *VarIndex;
        const NsDeque<NsInt>&  intArray;
        NsIntVar  *VarValue;

    public:

        Ns_ConstrElement (NsIntVar *VarIndex_init,
                          const NsDeque<NsInt>& intArray_init,
                          NsIntVar *VarValue_init)
                : VarIndex(VarIndex_init),
                  intArray(intArray_init),
                  VarValue(VarValue_init)
        {
                revisionType  =  VALUE_CONSISTENCY;
                assert_Ns( &VarIndex->manager() == &VarValue->manager(),  "Ns_ConstrElement::Ns_ConstrElement: All the variables of a constraint must belong to the same NsProblemManager");
                assert_Ns( ! intArray.empty() ,  "Ns_ConstrElement::Ns_ConstrElement: Empty element array" );
        }

        virtual int   varsInvolvedIn (void)  const
        {
                return 2;
        }

        virtual void
        toGraphFile (std::ofstream& fileConstraintsGraph)  const
        {
                fileConstraintsGraph << "\n\tVar"
                                     << VarValue << " -> Var" << VarIndex
                                     << "   [label=\"element\"];\n";
        }

        virtual void  ArcCons      (void);
        virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ExprYplusC : public Ns_Expression {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprYplusC (NsIntVar& Y, const NsInt C_init)
                : VarY(Y), C(C_init)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYplusC
operator +  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprYplusC( Y,  C );
}

inline  Ns_ExprYplusC
operator +  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post() +  C );
}

inline  Ns_ExprYplusC
operator +  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr.post() +  C );
}

inline  Ns_ExprYplusC
operator +  (const NsInt C, NsIntVar& Y)
{
        return  ( Y + C );
}

inline  Ns_ExprYplusC
operator -  (NsIntVar& Y, const NsInt C)
{
        return  ( Y + -C );
}

inline  Ns_ExprYplusC
operator -  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post() - C );
}

class Ns_ExprYdivC : public Ns_Expression {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprYdivC (NsIntVar& Y, const NsInt C_init)
                : VarY(Y), C(C_init)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYdivC
operator /  (NsIntVar& Y, const NsInt C)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (Y / -C) );
        return  Ns_ExprYdivC( Y, C );
}

inline  Ns_ExprYdivC
operator /  (const Ns_Expression& Yexpr, const NsInt C)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (Yexpr / -C) );
        return  ( Yexpr.post() / C );
}

class Ns_ExprYplusCZspecial : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;
        NsInt  C;

    public:

        Ns_ExprYplusCZspecial (NsIntVar& Y, const NsInt C_init, NsIntVar& Z)
                : VarY(Y), VarZ(Z), C(C_init)   {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYplusCZspecial
NsYplusCZspecial  (NsIntVar& Y, const NsInt C, NsIntVar& Z)
{
        return  Ns_ExprYplusCZspecial( Y, C, Z );
}

class Ns_ExprYplusZ : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprYplusZ (NsIntVar& Y, NsIntVar& Z)
                : VarY(Y), VarZ(Z)  {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYplusZ
operator +  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprYplusZ( Y, Z );
}

inline  Ns_ExprYplusZ
operator +  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post() + Z );
}

inline  Ns_ExprYplusZ
operator +  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y + Zexpr.post() );
}

inline  Ns_ExprYplusZ
operator +  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post() + Zexpr.post() );
}

class Ns_ExprYminusZ : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprYminusZ (NsIntVar& Y, NsIntVar& Z)
                : VarY(Y), VarZ(Z)  {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYminusZ
operator -  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprYminusZ( Y, Z );
}

inline  Ns_ExprYminusZ
operator -  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post() - Z );
}

inline  Ns_ExprYminusZ
operator -  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y - Zexpr.post() );
}

inline  Ns_ExprYminusZ
operator -  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post() - Zexpr.post() );
}

class Ns_ExprCminusZ : public Ns_Expression {

    private:

        NsInt  C;
        NsIntVar&  VarY;

    public:

        Ns_ExprCminusZ (const NsInt C_init, NsIntVar& Z)
                : C(C_init), VarY(Z)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprCminusZ
operator -  (const NsInt C, NsIntVar& Z)
{
        return  Ns_ExprCminusZ( C, Z );
}

inline  Ns_ExprCminusZ
operator -  (const NsInt C, const Ns_Expression& Zexpr)
{
        return  ( C - Zexpr.post() );
}

class Ns_ExprYtimesZ : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprYtimesZ (NsIntVar& Y, NsIntVar& Z)
                : VarY(Y), VarZ(Z)  {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYtimesZ
operator *  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprYtimesZ( Y, Z );
}

inline  Ns_ExprYtimesZ
operator *  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post() * Z );
}

inline  Ns_ExprYtimesZ
operator *  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y * Zexpr.post() );
}

inline  Ns_ExprYtimesZ
operator *  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post() * Zexpr.post() );
}

class Ns_ExprYtimesC : public Ns_Expression {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprYtimesC (NsIntVar& Y, const NsInt C_init)
                : VarY(Y), C(C_init)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYtimesC
operator *  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprYtimesC( Y, C );
}

inline  Ns_ExprYtimesC
operator *  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post() * C );
}

inline  Ns_ExprYtimesC
operator *  (const NsInt C, NsIntVar& Y)
{
        return  ( Y * C );
}

inline  Ns_ExprYtimesC
operator *  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr.post() * C );
}

inline  Ns_ExprCminusZ
operator -  (NsIntVar& Y)
{
        return  ( 0 - Y );
}

inline  Ns_ExprCminusZ
operator -  (const Ns_Expression& Yexpr)
{
        return  ( 0 - Yexpr.post() );
}

class Ns_ExprYdivZ : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprYdivZ (NsIntVar& Y, NsIntVar& Z)
                : VarY(Y), VarZ(Z)  {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYdivZ
operator /  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprYdivZ( Y, Z );
}

inline  Ns_ExprYdivZ
operator /  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post() / Z );
}

inline  Ns_ExprYdivZ
operator /  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y / Zexpr.post() );
}

inline  Ns_ExprYdivZ
operator /  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post() / Zexpr.post() );
}

class Ns_ExprCdivZ : public Ns_Expression {

    private:

        NsInt  C;
        NsIntVar&  VarZ;

    public:

        Ns_ExprCdivZ (const NsInt C_init, NsIntVar& Z)
                : C(C_init), VarZ(Z)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprCdivZ
operator /  (const NsInt C, NsIntVar& Z)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (-C / Z) );
        return  Ns_ExprCdivZ( C, Z );
}

inline  Ns_ExprCdivZ
operator /  (const NsInt C, const Ns_Expression& Zexpr)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (-C / Zexpr) );
        return  ( C / Zexpr.post() );
}

class Ns_ExprYmodZ : public Ns_Expression {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprYmodZ (NsIntVar& Y, NsIntVar& Z)
                : VarY(Y), VarZ(Z)  {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYmodZ
operator %  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprYmodZ( Y, Z );
}

inline  Ns_ExprYmodZ
operator %  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post() % Z );
}

inline  Ns_ExprYmodZ
operator %  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y % Zexpr.post() );
}

inline  Ns_ExprYmodZ
operator %  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post() % Zexpr.post() );
}

class Ns_ExprYmodC : public Ns_Expression {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprYmodC (NsIntVar& Y, const NsInt C_init)
                : VarY(Y), C(C_init)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprYmodC
operator %  (NsIntVar& Y, const NsInt C)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (Y % -C) );
        return  Ns_ExprYmodC( Y, C );
}

inline  Ns_ExprYmodC
operator %  (const Ns_Expression& Yexpr, const NsInt C)
{
        //if (C < 0)
        //// In order to conform to the requirements of the constraint implementation
        //	return  ( - (Yexpr % -C) );
        return  ( Yexpr.post() % C );
}

class Ns_ExprCmodZ : public Ns_Expression {

    private:

        NsInt  C;
        NsIntVar&  VarZ;

    public:

        Ns_ExprCmodZ (const NsInt C_init, NsIntVar& Z)
                : C(C_init), VarZ(Z)    {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprCmodZ
operator %  (const NsInt C, NsIntVar& Z)
{
        return  Ns_ExprCmodZ( C, Z );
}

inline  Ns_ExprCmodZ
operator %  (const NsInt C, const Ns_Expression& Zexpr)
{
        return  ( C % Zexpr.post() );
}

class Ns_ExprAbsY : public Ns_Expression {

    private:

        NsIntVar&  VarY;

    public:

        Ns_ExprAbsY (NsIntVar& Y)
                : VarY(Y)   {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprAbsY
NsAbs  (NsIntVar& Y)
{
        return  Ns_ExprAbsY( Y );
}

inline  Ns_ExprAbsY
NsAbs  (const Ns_Expression& Yexpr)
{
        return  NsAbs( Yexpr.post() );
}

class Ns_ExprInDomain : public Ns_Expression {

    private:

        NsProblemManager&  pm;
        NsDeque<NsInt>&  domain;
        NsDeque<NsInt>  *domainPrevious;
        NsDeque<NsInt>  *domainNext;

    public:

        Ns_ExprInDomain (NsProblemManager& pm_init,
                         NsDeque<NsInt>& domain_init,
                         NsDeque<NsInt> *domainPrevious_init,
                         NsDeque<NsInt> *domainNext_init)
                : pm(pm_init),
                  domain(domain_init),
                  domainPrevious(domainPrevious_init),
                  domainNext(domainNext_init)
        {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprInDomain
NsInDomain  (NsProblemManager& pm, NsDeque<NsInt>& domain)
{
        return  Ns_ExprInDomain( pm, domain, 0, 0 );
}

inline  Ns_ExprInDomain
NsInDomain  (NsProblemManager& pm, NsDeque<NsInt>& domain,
             NsDeque<NsInt>& domainPrevious, NsDeque<NsInt>& domainNext)
{
        return  Ns_ExprInDomain( pm, domain, &domainPrevious, &domainNext );
}

class Ns_ExprMin : public Ns_Expression {

    private:

        NsIntVarArray&  VarArr;

    public:

        Ns_ExprMin (NsIntVarArray&  VarArr_init)
                : VarArr(VarArr_init)   {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprMin
NsMin  (NsIntVarArray& Arr)
{
        return  Ns_ExprMin( Arr );
}

class Ns_ExprMax : public Ns_Expression {

    private:

        NsIntVarArray&  VarArr;

    public:

        Ns_ExprMax (NsIntVarArray&  VarArr_init)
                : VarArr(VarArr_init)   {   }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprMax
NsMax  (NsIntVarArray& Arr)
{
        return  Ns_ExprMax( Arr );
}

class Ns_ExprSum : public Ns_Expression {

    private:

        NsIntVarArray&  VarArr;
        NsIndex  start, length;

    public:

        Ns_ExprSum (NsIntVarArray&  VarArr_init)
                : VarArr(VarArr_init), start(0), length(VarArr_init.size()) {    }

        Ns_ExprSum (NsIntVarArray&  VarArr_init,
                    const NsIndex start_init, const NsIndex length_init)
                : VarArr(VarArr_init), start(start_init), length(length_init)
        {
                //&&  length >= 0,
                assert_Ns(start+length <= VarArr.size(),
                          "Ns_ExprSum::Ns_ExprSum: Wrong `start' or `length'");
        }

        virtual void       post (NsIntVar& VarX)  const;
        virtual NsIntVar&  post (void)  const;
};

inline  Ns_ExprSum
NsSum  (NsIntVarArray& Arr)
{
        return  Ns_ExprSum( Arr );
}

inline  Ns_ExprSum
NsSum  (NsIntVarArray& Arr, const NsIndex start, const NsIndex length)
{
        return  Ns_ExprSum( Arr, start, length );
}

///  Abstract class that represents an expression having to do with arrays of constrained variables.

///  \internal

class  Ns_ExpressionArray {

    public:

        ///  Posts the constraint.
        virtual void       post (NsIntVarArray& VarArr)  const = 0;

        // The destructor of an abstract class should be virtual.
        virtual   ~Ns_ExpressionArray (void)    {   }
};

class Ns_ExprInverse : public Ns_ExpressionArray {

    private:

        NsIntVarArray&  VarArr;
        NsInt  MaxDom;

    public:

        Ns_ExprInverse (NsIntVarArray&  VarArr_init, const NsInt MaxDom_init=-1)
                : VarArr(VarArr_init), MaxDom(MaxDom_init)  {   }

        virtual void       post (NsIntVarArray& VarArrInv)  const;
};

inline  Ns_ExprInverse
NsInverse  (NsIntVarArray& Arr)
{
        return  Ns_ExprInverse( Arr );
}

inline  Ns_ExprInverse
NsInverse  (NsIntVarArray& Arr, const NsInt MaxDom)
{
        return  Ns_ExprInverse( Arr, MaxDom );
}

///  Ns_Expression subcategory describing constraints.

///  The following abstract class represents the expressions category that
///   can be viewed both as a constraint (e.g.\ \a X < \a Y) and as an
///   expression/meta-constraint (e.g.\ \a Z == \a X < \a Y).
///   \internal

class Ns_ExprConstr : public Ns_Expression {

    protected:

        ///  Positive or negative constraint declaration.

        ///  If \a isPositive \c == \c false then the semantics of
        ///   the constraint-expression is inverted.
        ///   E.g.\ `\a X < \a Y' becomes `\a X >= \a Y'.
        bool  isPositive;

    public:

        Ns_ExprConstr (const bool isPositive_init)
                : isPositive(isPositive_init)   {   }

        ///  Final declaration and post of a constraint.
        virtual Ns_Constraint  *postConstraint (bool positively=true)  const = 0;

        ///  Uses \c postC(\a VarX, \c true) to post a constraint.
        virtual void       post (NsIntVar& VarX)  const
        {
                postC(VarX, true);
        }

        ///  Uses \c postC(true) to post a constraint.
        virtual NsIntVar&  post (void)  const
        {
                return  postC(true);
        }

        ///  Extends Ns_Expression::post(NsIntVar &VarX) const in the way that it can inverse the semantics of the constraint.
        virtual void       postC (NsIntVar& VarX, bool positively=true)  const = 0;

        ///  Extends Ns_Expression::post() in the way that it can inverse the semantics of the constraint.
        virtual NsIntVar&  postC (bool positively=true)  const = 0;
};

class Ns_ExprConstrYlessthanC : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprConstrYlessthanC (NsIntVar& Y, const NsInt C_init, const bool isPositive_init)
                : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)    {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrYlesseqthanC : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprConstrYlesseqthanC (NsIntVar& Y, const NsInt C_init, const bool isPositive_init)
                : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)    {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

//class Ns_ExprConstrYgreaterthanC : public Ns_ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		Ns_ExprConstrYgreaterthanC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual Ns_Constraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};
//
//
//
//
//
//class Ns_ExprConstrYgreatereqthanC : public Ns_ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		Ns_ExprConstrYgreatereqthanC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual Ns_Constraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};

class Ns_ExprConstrYeqC : public Ns_ExprConstr {
    private:

        NsIntVar&  VarY;
        NsInt  C;

    public:

        Ns_ExprConstrYeqC (NsIntVar& Y, const NsInt C_init, const bool isPositive_init)
                : Ns_ExprConstr(isPositive_init), VarY(Y), C(C_init)    {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

//class Ns_ExprConstrYneqC : public Ns_ExprConstr  {
//	private:
//		NsIntVar  &VarY;
//		NsInt  C;
//
//	public:
//		Ns_ExprConstrYneqC (NsIntVar& Y, const NsInt C_init)
//			: VarY(Y), C(C_init)	{   }
//
//		virtual Ns_Constraint*  postConstraint (void)  const;
//
//		virtual void      post (NsIntVar *VarX)  const;
//		virtual NsIntVar&  post (void)  const;
//};

class Ns_ExprConstrYlessthanZ : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprConstrYlessthanZ (NsIntVar& Y, NsIntVar& Z)
                : Ns_ExprConstr(true), VarY(Y), VarZ(Z) {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrYlesseqthanZ : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprConstrYlesseqthanZ (NsIntVar& Y, NsIntVar& Z)
                : Ns_ExprConstr(true), VarY(Y), VarZ(Z) {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrYeqZ : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprConstrYeqZ (NsIntVar& Y, NsIntVar& Z, const bool isPositive_init)
                : Ns_ExprConstr(isPositive_init), VarY(Y), VarZ(Z)  {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrYandZ : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprConstrYandZ (NsIntVar& Y, NsIntVar& Z)
                : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_Ns(0 <= VarY.min()  &&  VarY.max() <= 1  &&  0 <= VarZ.min()  &&  VarZ.max() <= 1,
                          "Ns_ExprConstrYandZ::Ns_ExprConstrYandZ: `VarY' and `VarZ' should be boolean");
        }

        virtual Ns_Constraint  *postConstraint (bool positively)  const
        {
                // to suppress warnings
                positively = positively;
                throw  NsException("Ns_ExprConstrYandZ::postConstraint: Please, add the two constraints separately");
        }

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrYorZ : public Ns_ExprConstr {

    private:

        NsIntVar&  VarY, &VarZ;

    public:

        Ns_ExprConstrYorZ (NsIntVar& Y, NsIntVar& Z)
                : Ns_ExprConstr(true), VarY(Y), VarZ(Z)
        {
                assert_Ns(0 <= VarY.min()  &&  VarY.max() <= 1  &&  0 <= VarZ.min()  &&  VarZ.max() <= 1,
                          "Ns_ExprConstrYorZ::Ns_ExprConstrYorZ: `VarY' and `VarZ' should be boolean");
        }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const;
        virtual NsIntVar&  postC (bool positively)  const;
};

class Ns_ExprConstrNegationY : public Ns_ExprConstr {

    private:

        const Ns_ExprConstr& Yexpr;

    public:

        Ns_ExprConstrNegationY (const Ns_ExprConstr& Y)
                : Ns_ExprConstr(true), Yexpr(Y) {    }

        virtual Ns_Constraint  *postConstraint (bool positively)  const
        {
                return  Yexpr.postConstraint(!positively);
        }

        virtual void       postC (NsIntVar& VarX, bool positively)  const
        {
                Yexpr.postC(VarX, !positively);
        }
        virtual NsIntVar&  postC (bool positively)  const
        {
                return  Yexpr.postC(!positively);
        }
};

class Ns_ExprConstrAllDiff : public Ns_ExprConstr {

    private:

        NsIntVarArray&  VarArr;
        const unsigned long   Capacity;

    public:

        Ns_ExprConstrAllDiff (NsIntVarArray& Arr, const unsigned long Cap)
                : Ns_ExprConstr(true), VarArr(Arr), Capacity(Cap)   {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const
        {
                VarX.size();	 // to suppress warnings
                // to suppress warnings
                positively = positively;
                throw  NsException("Ns_ExprConstrAllDiff::postC: NsAllDiff cannot be used as a meta-constraint");
        }
        virtual NsIntVar&  postC (bool positively)  const
        {
                // to suppress warnings
                positively = positively;
                throw  NsException("Ns_ExprConstrAllDiff::postC: NsAllDiff cannot be used as a meta-constraint");
        }
};

class Ns_ExprConstrCount : public Ns_ExprConstr {

    private:

        NsIntVarArray&  VarArr;
        const NsDeque<NsInt>&  Values;
        const NsDeque<NsInt>&  Occurrences;
        const NsDeque< NsDeque<NsIndex> >&  SplitPositions;
        const NsIndex  Split;
        const NsIndex  Dwin;

    public:

        Ns_ExprConstrCount (NsIntVarArray& Arr,
                            const NsDeque<NsInt>& Values_init,
                            const NsDeque<NsInt>& Occurrences_init,
                            const NsDeque< NsDeque<NsIndex> >& SplitPositions_init,
                            const NsIndex Split_init,
                            const NsIndex Dwin_init)
                : Ns_ExprConstr(true), VarArr(Arr),
                  Values(Values_init),
                  Occurrences(Occurrences_init),
                  SplitPositions(SplitPositions_init),
                  Split(Split_init),
                  Dwin(Dwin_init)
        {   }

        virtual Ns_Constraint  *postConstraint (bool positively)  const;

        virtual void       postC (NsIntVar& VarX, bool positively)  const
        {
                VarX.size();	 // to suppress warnings
                // to suppress warnings
                positively = positively;
                throw  NsException("Ns_ExprConstrCount::postC: NsCount cannot be used as a meta-constraint");
        }
        virtual NsIntVar&  postC (bool positively)  const
        {
                // to suppress warnings
                positively = positively;
                throw  NsException("Ns_ExprConstrCount::postC: NsCount cannot be used as a meta-constraint");
        }
};

inline  Ns_ExprConstrYlessthanC
operator <  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYlessthanC( Y, C, true );
}

inline  Ns_ExprConstrYlessthanC
operator >  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  <  C );
}

inline  Ns_ExprConstrYlessthanC
operator <  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  <  C );
}

inline  Ns_ExprConstrYlessthanC
operator >  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  <  C );
}

inline  Ns_ExprConstrYlesseqthanC
operator <=  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYlesseqthanC( Y, C, true );
}

inline  Ns_ExprConstrYlesseqthanC
operator >=  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  <=  C );
}

inline  Ns_ExprConstrYlesseqthanC
operator <=  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  <=  C );
}

inline  Ns_ExprConstrYlesseqthanC
operator >=  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  (Yexpr  <=  C);
}

inline  Ns_ExprConstrYlesseqthanC
operator >  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYlesseqthanC( Y, C, false );
}

inline  Ns_ExprConstrYlesseqthanC
operator <  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  >  C );
}

inline  Ns_ExprConstrYlesseqthanC
operator >  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  >  C );
}

inline  Ns_ExprConstrYlesseqthanC
operator <  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  >  C );
}

inline  Ns_ExprConstrYlessthanC
operator >=  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYlessthanC( Y, C, false );
}

inline  Ns_ExprConstrYlessthanC
operator <=  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  >=  C );
}

inline  Ns_ExprConstrYlessthanC
operator >=  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  >=  C );
}

inline  Ns_ExprConstrYlessthanC
operator <=  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  >=  C );
}

inline  Ns_ExprConstrYeqC
operator ==  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYeqC( Y, C, true );
}

inline  Ns_ExprConstrYeqC
operator ==  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  ==  C );
}

inline  Ns_ExprConstrYeqC
operator ==  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  ==  C );
}

inline  Ns_ExprConstrYeqC
operator ==  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  ==  C );
}

inline  Ns_ExprConstrYeqC
operator !=  (NsIntVar& Y, const NsInt C)
{
        return  Ns_ExprConstrYeqC( Y, C, false );
}

inline  Ns_ExprConstrYeqC
operator !=  (const NsInt C, NsIntVar& Y)
{
        return  ( Y  !=  C );
}

inline  Ns_ExprConstrYeqC
operator !=  (const Ns_Expression& Yexpr, const NsInt C)
{
        return  ( Yexpr.post()  !=  C );
}

inline  Ns_ExprConstrYeqC
operator !=  (const NsInt C, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  !=  C );
}

inline  Ns_ExprConstrYlessthanZ
operator <  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprConstrYlessthanZ( Y, Z );
}

inline  Ns_ExprConstrYlessthanZ
operator >  (NsIntVar& Z, NsIntVar& Y)
{
        return  ( Y  <  Z );
}

inline  Ns_ExprConstrYlessthanZ
operator <  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y  <  Zexpr.post() );
}

inline  Ns_ExprConstrYlessthanZ
operator <  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post()  <  Z );
}

inline  Ns_ExprConstrYlessthanZ
operator >  (NsIntVar& Z, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  <  Z );
}

inline  Ns_ExprConstrYlessthanZ
operator >  (const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return  ( Y  <  Zexpr );
}

inline  Ns_ExprConstrYlessthanZ
operator <  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post()  <  Zexpr.post() );
}

inline  Ns_ExprConstrYlessthanZ
operator >  (const Ns_Expression& Zexpr, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  <  Zexpr );
}

inline  Ns_ExprConstrYlesseqthanZ
operator <=  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprConstrYlesseqthanZ( Y, Z );
}

inline  Ns_ExprConstrYlesseqthanZ
operator >=  (NsIntVar& Z, NsIntVar& Y)
{
        return  ( Y  <=  Z );
}

inline  Ns_ExprConstrYlesseqthanZ
operator <=  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y  <=  Zexpr.post() );
}

inline  Ns_ExprConstrYlesseqthanZ
operator <=  (const Ns_Expression& Yexpr, NsIntVar& Z)
{
        return  ( Yexpr.post()  <=  Z );
}

inline  Ns_ExprConstrYlesseqthanZ
operator >=  (NsIntVar& Z, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  <=  Z );
}

inline  Ns_ExprConstrYlesseqthanZ
operator >=  (const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return  ( Y  <=  Zexpr );
}

inline  Ns_ExprConstrYlesseqthanZ
operator <=  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post()  <=  Zexpr.post() );
}

inline  Ns_ExprConstrYlesseqthanZ
operator >=  (const Ns_Expression& Zexpr, const Ns_Expression& Yexpr)
{
        return  ( Yexpr  <=  Zexpr );
}

inline  Ns_ExprConstrYeqZ
operator ==  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprConstrYeqZ( Y, Z, true );
}

inline  Ns_ExprConstrYeqZ
operator ==  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y  ==  Zexpr.post() );
}

inline  Ns_ExprConstrYeqZ
operator ==  (const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return  ( Y  ==  Zexpr );
}

inline  Ns_ExprConstrYeqZ
operator ==  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post()  ==  Zexpr.post() );
}

inline  Ns_ExprConstrYeqZ
operator !=  (NsIntVar& Y, NsIntVar& Z)
{
        return  Ns_ExprConstrYeqZ( Y, Z, false );
}

inline  Ns_ExprConstrYeqZ
operator !=  (NsIntVar& Y, const Ns_Expression& Zexpr)
{
        return  ( Y  !=  Zexpr.post() );
}

inline  Ns_ExprConstrYeqZ
operator !=  (const Ns_Expression& Zexpr, NsIntVar& Y)
{
        return  ( Y  !=  Zexpr );
}

inline  Ns_ExprConstrYeqZ
operator !=  (const Ns_Expression& Yexpr, const Ns_Expression& Zexpr)
{
        return  ( Yexpr.post()  !=  Zexpr.post() );
}

inline  Ns_ExprConstrYandZ
operator &&  (const Ns_ExprConstr& Yexpr, const Ns_ExprConstr& Zexpr)
{
        return  Ns_ExprConstrYandZ( Yexpr.post(), Zexpr.post() );
}

inline  Ns_ExprConstrYorZ
operator ||  (const Ns_ExprConstr& Yexpr, const Ns_ExprConstr& Zexpr)
{
        return  Ns_ExprConstrYorZ( Yexpr.post(), Zexpr.post() );
}

inline  Ns_ExprConstrNegationY
operator !  (const Ns_ExprConstr& Yexpr)
{
        return  Ns_ExprConstrNegationY( Yexpr );
}

inline  Ns_ExprConstrAllDiff
NsAllDiff  (NsIntVarArray& Arr, const unsigned long Capacity=0)
{
        return  Ns_ExprConstrAllDiff(Arr, Capacity);
}

inline  Ns_ExprConstrCount
NsCount  (NsIntVarArray& Arr,
          const NsDeque<NsInt>& Values,
          const NsDeque<NsInt>& Occurrences,
          const NsDeque< NsDeque<NsIndex> >& SplitPositions = NsDeque< NsDeque<NsIndex> >(),
          const NsIndex Split=0,
          const NsIndex Dwin=1)
{
        return  Ns_ExprConstrCount(Arr, Values, Occurrences,
                                   SplitPositions, Split, Dwin);
}

inline  Ns_ExprConstrYorZ
NsIfThen  (const Ns_ExprConstr& Yexpr, const Ns_ExprConstr& Zexpr)
{
        //  "p => q"  is equivalent to  "!p || q".
        return  ( ! Yexpr  ||  Zexpr );
}

inline  Ns_ExprConstrYeqZ
NsEquiv  (const Ns_ExprConstr& Yexpr, const Ns_ExprConstr& Zexpr)
{
        //  "p <=> q"  is equivalent to  "p == q".
        return  ( Yexpr  ==  Zexpr );
}

inline  Ns_ExprConstrYeqZ
NsElement  (NsIntVar& VarIndex,
            const NsDeque<NsInt>& intArray,
            NsIntVar& VarValue)
{
        return  ( intArray[VarIndex]  ==  VarValue );
}

inline  Ns_ExprConstrYeqZ
NsElement  (const Ns_Expression& VarIndexExpr,
            const NsDeque<NsInt>& intArray,
            NsIntVar& VarValue)
{
        return  NsElement(VarIndexExpr.post(), intArray, VarValue);
}

inline  Ns_ExprConstrYeqZ
NsElement  (NsIntVar& VarIndex,
            const NsDeque<NsInt>& intArray,
            const Ns_Expression& VarValueExpr)
{
        return  NsElement(VarIndex, intArray,
                          VarValueExpr.post());
}

inline  Ns_ExprConstrYeqZ
NsElement  (const Ns_Expression& VarIndexExpr,
            const NsDeque<NsInt>& intArray,
            const Ns_Expression& VarValueExpr)
{
        return  NsElement(VarIndexExpr.post(), intArray,
                          VarValueExpr.post());
}

///  An abstract class representing an (internal or constructed by user) goal.

///  Each time a goal is executed by the solver, its method \c GOAL() is
///   called.  This method can make assignments or remove values from
///   constrained variables.
class  NsGoal {

    public:

        virtual bool  isGoalAND (void)  const
        {
                return  false;
        }

        virtual bool  isGoalOR  (void)  const
        {
                return  false;
        }

        virtual NsGoal  *getFirstSubGoal  (void)  const
        {
                throw  NsException("NsGoal::getFirstSubGoal: *this: Not an AND/OR goal");
        }

        virtual NsGoal  *getSecondSubGoal (void)  const
        {
                throw  NsException("NsGoal::getSecondSubGoal: *this: Not an AND/OR goal");
        }

        virtual NsGoal  *GOAL (void) = 0;

        // The destructor of an abstract class should be virtual.
        virtual ~NsGoal (void)  {   }
};

///  The first kind of `meta-goal' (i.e.\ goal used to combine two other goals).
class NsgAND : public NsGoal {

    private:

        NsGoal  *firstSubGoal;
        NsGoal  *secondSubGoal;

    public:

        NsgAND (NsGoal *firstSubGoal_init, NsGoal *secondSubGoal_init)
                : firstSubGoal(firstSubGoal_init), secondSubGoal(secondSubGoal_init)
        {
                assert_Ns( firstSubGoal != 0  &&  secondSubGoal != 0 ,  "NsgAND::NsgAND: A subgoal is zero");
        }

        virtual bool  isGoalAND (void)  const
        {
                return  true;
        }

        virtual NsGoal  *getFirstSubGoal  (void)  const
        {
                return  firstSubGoal;
        }

        virtual NsGoal  *getSecondSubGoal (void)  const
        {
                return  secondSubGoal;
        }

        NsGoal  *GOAL (void)
        {
                throw  NsException("NsgAND::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

///  The second--and last--kind of `meta-goal' (i.e.\ goal used to combine two other goals).
class NsgOR : public NsGoal {

    private:

        NsGoal  *firstSubGoal;
        NsGoal  *secondSubGoal;

    public:

        NsgOR (NsGoal *firstSubGoal_init, NsGoal *secondSubGoal_init)
                : firstSubGoal(firstSubGoal_init), secondSubGoal(secondSubGoal_init)
        {
                assert_Ns( firstSubGoal != 0  &&  secondSubGoal != 0 ,  "NsgOR::NsgOR: A subgoal is zero");
        }

        virtual bool  isGoalOR (void)  const
        {
                return  true;
        }

        virtual NsGoal  *getFirstSubGoal  (void)  const
        {
                return  firstSubGoal;
        }

        virtual NsGoal  *getSecondSubGoal (void)  const
        {
                return  secondSubGoal;
        }

        NsGoal  *GOAL (void)
        {
                throw  NsException("NsgOR::GOAL: This is a meta-goal (i.e. not a real goal)");
        }
};

///  A simple goal that assigns a value to a constrained variable.
class NsgSetValue : public NsGoal {

    private:

        NsIntVar&  Var;
        const NsInt  value;

    public:

        NsgSetValue (NsIntVar& Var_init, const NsInt value_init)
                : Var(Var_init), value(value_init)    {    }

        NsGoal  *GOAL (void)
        {
                //std::cout << "  NsgSetValue:   ";
                Var.set(value);
                return  0;
        }
};

///  A goal that removes a value from the domain of a given constrained variable.
class NsgRemoveValue : public NsGoal {

    private:

        NsIntVar&  Var;
        const NsInt  value;

    public:

        NsgRemoveValue (NsIntVar& Var_init, const NsInt value_init)
                : Var(Var_init), value(value_init)    {    }

        NsGoal  *GOAL (void)
        {
                //std::cout << "  NsgRemoveValue:";
                Var.remove(value);
                return  0;
        }
};

///  A goal that tries to instantiate a constrained variable.

///  I.e.\ it assigns a value from the domain of the variable;
///   if this assignment results to an inconsistency, it removes the value
///   from the domain, and continues trying the rest of the values.
class NsgInDomain : public NsGoal {

    private:

        NsIntVar&  Var;

    public:

        NsgInDomain (NsIntVar& Var_init)
                : Var(Var_init)    {    }

        NsGoal  *GOAL (void)
        {
                //std::cout << "  NsgInDomain:    ";
                if (Var.isBound())
                        return  0;
                NsInt  value = Var.min();
                return  ( new NsgOR( new NsgSetValue(Var,value) ,
                                     new NsgAND( new NsgRemoveValue(Var,value) ,
                                                 new NsgInDomain(*this) ) ) );
        }
};

///  The generalization of NsgInDomain that applies to arrays.

///  It uses NsgInDomain to iteratively instantiate each variable of the
///   array.  The next variable that is chosen is the one having the
///   minimum domain size (according to the `first-fail' heuristic).
class NsgLabeling : public NsGoal {

    private:

        NsIntVarArray&  VarArr;

    public:

        NsgLabeling (NsIntVarArray& VarArr_init)
                : VarArr(VarArr_init)    {    }

        NsGoal  *GOAL (void)
        {
                //std::cout << "  NsgLabeling:       ";
                NsIndex  index = NsINDEX_INF;
                NsUInt  minDom = NsUPLUS_INF;
                for (NsIndex i = 0;   i < VarArr.size();   ++i) {
                        if ( !VarArr[i].isBound()   &&   VarArr[i].size() < minDom ) {
                                minDom = VarArr[i].size();
                                index = i;
                        }
                }
                if (index == NsINDEX_INF)
                        return  0;
                return  ( new NsgAND( new NsgInDomain(VarArr[index]) , new NsgLabeling(*this) ) );
        }
};

///  The type of the objects in the queue for the AC algorithm.

///  When a value is removed from the domain of a variable, an item is
///   added into this queue.
///   \internal
class  Ns_QueueItem {

    private:

        ///  The domain of this variable has been modified.
        NsIntVar  *varFired;

        ///  The index of the constraint to check.
        NsDeque<NsIntVar::ConstraintAndFailure>::size_type  currentConstr;

        struct  RemovedValueRecord_t;

        ///  The index of the removed value that will be checked against constraints (that require arc consistency).
        NsDeque<RemovedValueRecord_t>::size_type  currentRemovedValue;

        ///  Holds information having to do with the modification (if any) of the bounds of varFired.

        struct  RemovedBoundRecord_t {

                ///  True if the minimum or maximum of the domain of varFired has been changed.
                bool  removedBound;

                ///  The constraint that fired the last modification of the bounds of varFired.  If no constraint provoked the removal, then constrFired==0.
                const Ns_Constraint  *constrFired;

                ///  The domain removal serial number.
                unsigned long  removalTime;

                ///  Constructor.
                RemovedBoundRecord_t (void)
                        : removedBound(false)
                {    }

                ///  Records a bounds modification.
                void
                boundChangedBy (const Ns_Constraint *constrFired_init ,
                                const unsigned long  removalTime_init )
                {
                        constrFired   =  constrFired_init;
                        removedBound  =  true;
                        removalTime   =  removalTime_init;
                }
        };

        ///  Used to record a modification of the bounds (if any).
        RemovedBoundRecord_t  removedBoundRec;

        ///  Describes the removal of the member removedValue from the domain of varFired.  (Contains the value that was removed, and which constraint did it.)

        struct  RemovedValueRecord_t {

                ///  The value that was taken from the domain of varFired.  (The `w' of the AC-5 algorithm.)
                const NsInt  value;

                ///  The constraint that fired the removal of the value removedValue from the domain of the variable varFired.  If no constraint provoked the removal, then constrFired==0.
                const Ns_Constraint  *constrFired;

                ///  Constructor.
                RemovedValueRecord_t (const NsInt removedValue_init,
                                      const Ns_Constraint *constrFired_init)
                        : value(removedValue_init),
                          constrFired(constrFired_init)
                {    }
        };

        ///  An array that records the values removed from the domain of varFired.
        NsDeque<RemovedValueRecord_t>  removedValues;

    public:

        ///  Constructor.
        Ns_QueueItem (NsIntVar *varFired_init)
                : varFired(varFired_init),
                  currentConstr(0),
                  currentRemovedValue(0)
        {   }

        ~Ns_QueueItem (void)
        {
                if ( varFired->queueItem == this )
                        varFired->queueItem = 0;
                //  Is there any possibility of varFired->queueItem !=
                //   this ?  Yes, when there are two items in AC queue for
                //   `varFired', the first being examined now by the AC
                //   algorithm.
        }

        void  boundChangedBy (const Ns_Constraint *constr);

        void  add (const NsInt removedVal,
                   const Ns_Constraint *constrThatRemovedIt);

        Ns_Constraint  *getNextConstraint (void);

        ///  Returns the variable that fired the constraint propagation.
        NsIntVar *
        getVarFired (void)  const
        {
                return  varFired;
        }

        ///  Returns the value that has been removed from the variable.
        NsInt
        getW (void)  const
        {
                return  removedValues[currentRemovedValue-1].value;
        }

        ///  When a constraint provokes an inconsistency, then its rank (index in the `varFired->constraints' array) should be updated according to the current number of inconsistencies it provoked (according to a heuristic).
        void
        resortConstraints (const Ns_Constraint *constr)  const
        {
                //  The following statement `corrects' currentConstr
                //   by assigning the proper value to constrFailed.
                long  constrFailed =
                        ( constr->revisionType == Ns_Constraint::VALUE_CONSISTENCY ) ?
                        currentConstr
                        : currentConstr - 1 ;
                ++varFired->constraints[constrFailed].failures;
                for (long c=constrFailed-1;   c >= 0;   --c) {
                        if ( varFired->constraints[c].failures
                             < varFired->constraints[constrFailed].failures ) {
                                std::swap(varFired->constraints[c],
                                          varFired->constraints[constrFailed]);
                        }
                }
        }
};

///  Normally used for describing the stack holding AND-goals that have to be satisfied.  This stack is also called `stackAND'.

///  \internal
class Ns_StackGoals : public NsStack<NsGoal *> {

    public:

        ~Ns_StackGoals (void);
};


class Ns_SearchNode;

///  Contains all the Ns_SearchNode 's.

///  A Ns_SearchNode contains the current status of the problem; it is
///   pushed into the stack when we make a choice (e.g.\ when we select a
///   specific value to assign it to a variable) and it is popped when we
///   want to cancel this choice, and we want to revert back to the
///   previous problem status.
///   \internal
class Ns_StackSearch : public NsStack<Ns_SearchNode> {

    public:

        Ns_StackSearch (void);

        ~Ns_StackSearch (void);

        void  clear (void);

        ///  @{
        ///  \name  Provision of history ids to the search nodes

    private:

        ///  History-ids together with time statistics.
        struct  history_time_t {

                ///  An ID for the current history level.
                NsUInt  validHistoryId;

                ///  An ID for the current node number.
                NsUInt  searchTreeNodeNum;

                ///  The time consumed all the nodes in this level.
                double  timeSum;

                ///  The simulation time for the children of all the nodes in this level.
                double  timeSimChildSum;

                ///  The plain simulation time for all the nodes in this level.
                double  timeSimSum;

                ///  The sum of the weights of timeSum terms.
                double  weightsSum;

                ///  The current desired simulation to real time ratio.
                double  simulationRatio;

                ///  Constructor.
                history_time_t (void)
                        : validHistoryId(0),
                          timeSum(0.0),
                          timeSimChildSum(0.0),
                          timeSimSum(0.0),
                          simulationRatio(1.0)
                {    }

                ///  Augments the valid history ID.
                void
                invalidate (clock_t timeBorn, double timeSimChild)
                {
                        ++validHistoryId;
                        double  timeChild =
                                clock() - timeBorn + timeSimChild;
                        double  weight = validHistoryId *
                                         (timeChild - timeSimChild + 1.0) /
                                         (timeChild + 1.0);
                        timeSum  +=  timeChild * weight;
                        timeSimChildSum  +=
                                timeSimChild * weight;
                        weightsSum  +=  weight;
                }

                ///  The mean value of the time spent in this level.
                double
                mean (void)
                {
                        assert_Ns( validHistoryId != 0 ,
                                   "history_time_t::mean: Cannot get mean value of an empty set" );
                        double  duration = timeSum / weightsSum;
                        timeSimSum += duration;
                        return  duration;
                }
        };

        NsDeque<history_time_t>  history_time;

        unsigned long  nSearchTreeNodes;

    public:

        ///  The mean value of the time spent in the next level.
        double
        nextMean (void)
        {
                return  history_time[size()].mean();
        }

        ///  Decides whether the next level will be explored or simulated.
        bool
        overrideNextLevel (void)
        {
                if ( history_time.size() < size() + 1 )
                        return  false;
                double&  simRatio = history_time[size()].simulationRatio;
                double  random = rand() / (RAND_MAX+1.0);
                bool  override = ( random > simRatio );
                simRatio = std::max(simulationRatioMin,
                                    simRatio - simulationRatioStep);
                return  override;
        }

        ///  The search tree split to be explored starts from this node.
        NsList<NsUInt>  startNode;

    private:

        ///  The search tree split to be explored ends up in this node.
        NsDeque<NsUInt>  endNode;

        ///  True if there was an unused readSplit call.
        bool  alreadyReadSplit;

    public:

        bool  readSplit (bool& startMatchesPreviousEnd);

        bool  splitEnded (void);

        unsigned long
        numSearchTreeNodes (void)  const
        {
                return  ( nSearchTreeNodes - 1 );
        }

        bool  push (const value_type& newNode);

        void  pop (void);

        ///  Restores the validHistoryId 's state as it was before search began.
        void
        reset (void)
        {
                history_time[0].validHistoryId = 0;
        }

        NsUInt
        getCurrentNodeNum (void)  const
        {
                return  history_time[size()-1].searchTreeNodeNum;
        }

        Ns_HistoryId_t
        getCurrentHistoryId (void)  const
        {
                Ns_HistoryId_t  di;
                di.level  =  size() - 1;
                di.id     =  history_time[di.level].validHistoryId;
                return  di;
        }

        bool
        isCurrentHistoryId (const Ns_HistoryId_t di)  const
        {
                return  ( di.level == size() - 1
                          && di.id == history_time[di.level].validHistoryId );
        }

        bool
        isCurrentHistoryIdNextTo (const Ns_HistoryId_t di)  const
        {
                return  ( di.level < size() - 1
                          || ! isValidHistoryId(di) );
        }

        bool
        isValidHistoryId (const Ns_HistoryId_t di)  const
        {
                return  ( di.id == history_time[di.level].validHistoryId );
        }

        ///  Virtual extra time used for simulation.
        double  timeSimulated;

        ///  The minimum percentage of the real search time vs total simulation time.
        double  simulationRatioMin;

        ///  How much to step from 100% towards `simulationRatioMin'.
        double  simulationRatioStep;

        void
        currentNodeId (void)  const
        {
                currentNodeIdRec(begin());
        }

    private:

        void
        currentNodeIdRec (const_iterator it)  const;

    public:

        ///  When endNode is changed, it is called to update the nodes matchesEndNode statuses.
        void
        updateMatchesEndNode (void)
        {
                NsUInt  depth;
                updateMatchesEndNodeRec(begin(), depth);
        }

    private:

        bool
        updateMatchesEndNodeRec (iterator it, NsUInt& depth);

        ///  @}

        ///  @{
        ///  \name  Representation of the search tree as a graph

    private:

        ///  File to store the search tree graph.
        std::ofstream  fileSearchGraph;

        ///  True if it should write the objective value.
        bool  recordObjective;

        ///  The last recorded objectiveValue.
        NsInt  objectiveValue;

    public:

        void  searchToGraphFile (const char *fileName);

        void  solutionNode (const NsIntVar *vObjective);

        ///  @}

        //friend class goal_iterator;

        ///  Iterates through all the goals in the current Ns_StackGoals and the Ns_StackGoals below it.

        ///  (All of them consist a stack of Ns_StackGoals, named
        ///   stackOfStacks.)
        class  goal_iterator {

            private:

                ///  A stack containing the search nodes (so each frame of this stack contains a Ns_StackGoals).
                Ns_StackSearch  *stackOfStacks;

                ///  Points to the current search node.
                Ns_StackSearch::iterator  curr_Stack_it;

                ///  Points to the current goal (in the Ns_StackGoals) of the current search node.
                Ns_StackGoals::iterator   curr_node_it;

            public:

                goal_iterator (void)
                        : stackOfStacks(0)
                {    }

                goal_iterator (Ns_StackSearch& stackOfStacks_init);

                bool  operator == (const goal_iterator& b)  const
                {
                        assert_Ns(stackOfStacks != 0,
                                  "Ns_StackSearch::goal_iterator::==: Uninitialized `*this'");
                        return  ( curr_Stack_it == b.curr_Stack_it
                                  &&  (  curr_Stack_it == stackOfStacks->end()
                                         || curr_node_it == b.curr_node_it ) );
                }

                bool
                operator != (const goal_iterator& b)  const
                {
                        return  ! ( *this == b );
                }

                NsGoal *
                operator * (void)  const
                {
                        assert_Ns( stackOfStacks != 0 ,
                                   "Ns_StackSearch::goal_iterator::*: "
                                   "Uninitialized `*this'");
                        assert_Ns( curr_Stack_it != stackOfStacks->end() ,
                                   "Ns_StackSearch::goal_iterator::*: "
                                   "Bad request");
                        return  *curr_node_it;
                }

                goal_iterator&
                end (void)
                {
                        assert_Ns( stackOfStacks != 0 ,
                                   "Ns_StackSearch::goal_iterator::end: "
                                   "Uninitialized `*this'");
                        curr_Stack_it = stackOfStacks->end();
                        return  *this;
                }

                goal_iterator&  operator ++ (void);
        };

        goal_iterator
        gbegin (void)
        {
                return  goal_iterator(*this);
        }

        goal_iterator
        gend (void)
        {
                goal_iterator  iterEnd(*this);
                return  iterEnd.end();
        }
};

///  Describes a search node of the binary search tree.

///  A critical type that can describe the current status of the problem.
///   \internal
struct  Ns_SearchNode {

    public:

        ///  Alternative goal to satisfy if search fails.
        NsGoal  *goalNextChoice;

        ///  Pointer to the first goal of the previous search level that has not been yet satisfied.
        Ns_StackSearch::goal_iterator  delayedGoal;

        ///  All the goals of this list should be satisfied.
        Ns_StackGoals   stackAND;

        ///  Constructor.
        Ns_SearchNode (NsGoal *goalNextChoice_init,
                       Ns_StackSearch::goal_iterator git)
                : goalNextChoice(goalNextChoice_init),
                  delayedGoal(git),
                  children(0),
                  timeBorn(clock()),
                  timeSimChild(0.0)
        {    }

        ///  Describes a tuple (BitsetDomainPointer, BitsetDomain).
        class  BitsetCopy {

            private:

                ///  Pointer to a bit-set domain.
                Ns_BitSet  *bitsetDomainPointer;

                ///  A copy of the above instance--created for future backtracking reasons.
                Ns_BitSet   bitsetDomainCopy;

            public:

                ///  Copy-constructor.
                BitsetCopy (Ns_BitSet& bitsetDomain)
                        : bitsetDomainPointer(&bitsetDomain),
                          bitsetDomainCopy(bitsetDomain)
                {    }

                ///  Restores the copy back to the original domain place (i.e.\ pointer).
                void
                restore (void)
                {
                        *bitsetDomainPointer  =
                                bitsetDomainCopy;
                }
        };

    private:

        ///  List to contain the saved bit-set domains.

        class BitsetsStore : public NsStack<BitsetCopy> {

            public:

                ///  Restores all the saved bit-set domains.
                void
                restore (void)
                {
                        while ( ! empty() ) {
                                top().restore();
                                pop();
                        }
                }
        };

    public:

        ///  Store to keep the previous states of the modified bit-sets.
        BitsetsStore  bitsetsStore;

        ///  The node's children number.
        NsUInt  children;

        ///  True if the current search node and its predecessors match the endNode.
        bool  matchesEndNode;

        ///  When the node was born?
        clock_t  timeBorn;

        ///  Virtual extra time, used in the simulation of the search tree exploration.
        double  timeSimChild;
};

///  Constraint satisfaction problem manager and solver class.

///  A problem manager keeps information for the constrained variables, the
///   constraints network, and the goal that will be executed.
class  NsProblemManager {

    private:

        ///  The stack containing the choice points that have been appeared during search.  Each choice point forms a binary search tree node.
        Ns_StackSearch  searchNodes;

        ///  AC algorithm event-queue type.
        typedef  NsQueue<Ns_QueueItem>  Ns_Queue_t;

        ///  AC algorithm event-queue.
        Ns_Queue_t  Q;

        ///  True, if an inconsistency has been found.
        bool  foundInconsistency;

        bool  arcConsistent (void);

        bool  backtrack (void);

        ///  The list of the soft (meta)constraints to be satisfied.
        NsIntVarArray  vSoftConstraintsTerms;

    public:

        ///  If a fileNameSearchGraph is provided, then a file is created with the search tree in a Graphviz supported format.

        NsProblemManager (void)
                :  foundInconsistency(false),
                   vMinObj(0),
                   timeLim(0),
                   firstNextSolution(true),
                   calledTimeLimit(false),
                   timeSplitLim(0),
                   domainsSizeMax(0),
                   nFailures(0),
                   nBacktracks(0),
                   nGoals(0),
                   nConstraintChecks(0),
                   backtrackLim(0)
        {
                assert_Ns( sizeof(NsIntVar *)  <=  sizeof(Ns_pointer_t) ,
                           "NsProblemManager::NsProblemManager: Cannot run on this machine, because a pointer does not fit in an `size_t' (the `unordered_set' type)");
                assert_Ns( searchNodes.push( Ns_SearchNode( 0, searchNodes.gbegin() ) ),
                           "NsProblemManager::NsProblemManager: First push should succeed");
        }

        ~NsProblemManager (void);

        ///  Adds a goal to be executed/satisfied.

        void
        addGoal (NsGoal *goal)
        {
                if (goal != 0)
                        searchNodes.top().stackAND.push( goal );
        }

        void  add (const Ns_ExprConstr& expr);

        void  add (const Ns_ExprConstr& expr, const NsInt weight);

        ///  Returns the AC algorithm queue.
        Ns_Queue_t&
        getQueue (void)
        {
                return  Q;
        }

        ///  Informs NsProblemManager than an inconsistency has been found.
        void
        foundAnInconsistency (void)
        {
                foundInconsistency = true;
        }

        bool  nextSolution (void);

        void  restart (void);

        ///  Writes to a file a view of the search tree in a Graphviz supported format.
        void
        searchToGraphFile (const char *fileName)
        {
                searchNodes.searchToGraphFile(fileName);
        }

        ///  @{
        ///  \name  Representation of the constraint network as a graph

    private:

        ///  File to store the constraint network graph.
        std::ofstream  fileConstraintsGraph;

    public:

        ///  Writes to a file a view of the constraint network in a Graphviz supported format.
        void
        constraintsToGraphFile (const char *fileName);

        ///  @}

        ///  @{
        ///  \name  Optimization members

    private:

        ///  Our objective is to minimize this constrained variable.
        NsIntVar  *vMinObj;

        ///  The minimum upper limit of `vMinObj' found so far (initially +oo).
        NsInt      bestMinObjValue;

    public:

        void
        minimize (NsIntVar& VarObj)
        {
                vMinObj          =  &VarObj;
                bestMinObjValue  =   NsPLUS_INF;
        }

        void
        minimize (const Ns_Expression& expr)
        {
                minimize( expr.post() );
        }

        void
        objectiveUpperLimit (NsInt max)
        {
                assert_Ns( vMinObj != 0 ,  "NsProblemManager::objectiveUpperLimit: No cost variable exists" );
                if ( bestMinObjValue  >  max + 1 ) {
                        bestMinObjValue = max + 1;
                        vMinObj->remove(bestMinObjValue, NsPLUS_INF/*, 0*/);
                }
        }

        ///  @}

        ///  @{
        ///  \name  Time management members

    private:

        ///  True if the available time is real (i.e.\ not system time).
        bool     isRealTime;

        ///  The first (system) time that nextSolution() has been called.
        clock_t  startTime;

        ///  The first time that nextSolution() has been called.
        time_t   startRealTime;

        ///  Available time (for running the search process) in seconds.
        unsigned long  timeLim;

        ///  True if nextSolution() has not been yet called.
        bool  firstNextSolution;

        ///  True if a limit to the available time (for search process) has been set.
        bool  calledTimeLimit;

        ///  The first (system) time that nextSolution() has been called for the current search space split.
        clock_t  startSplitTime;

        ///  Available time ticks for running the search process for one search space split.
        clock_t  timeSplitLim;

        ///  The previous recorded node by the splitting process.
        NsUInt  startNodeId;

    public:

        ///  Sets the time ticks limit for each search space split.
        void
        splitTimeLimit (const clock_t ticks,
                        const double simulationRatioMin=0.05,
                        const double simulationRatioStep=0.01)
        {
                timeSplitLim = ticks;
                searchNodes.simulationRatioMin  = simulationRatioMin;
                searchNodes.simulationRatioStep = simulationRatioStep;
                startNodeId = getCurrentNodeNum();
                startSplitTime = clock();
                assert_Ns( startSplitTime != -1 ,
                           "Could not find time for `splitTimeLimit'");
                searchNodes.currentNodeId();
        }

        ///  Explore specific search tree splits described in standard input.
        bool
        readSplit (void)
        {
                bool  foo;
                return  searchNodes.readSplit(foo);
        }

        ///  Sets the time limit.  After this limit is exceeded, nextSolution() returns \c false.
        void
        realTimeLimit (const unsigned long secs)
        {
                calledTimeLimit = true;
                isRealTime = true;
                timeLim = secs;
        }

        ///  Sets the CPU time limit.  After this limit is exceeded, nextSolution() returns \c false.
        void
        timeLimit (const unsigned long secs)
        {
                calledTimeLimit = true;
                isRealTime = false;
                timeLim = secs;
        }

        ///  Sets the limit to the backtracks that search process does.  After this limit is exceeded, nextSolution() returns \c false.
        void
        backtrackLimit (const unsigned long lim)
        {
                backtrackLim = nBacktracks + lim;
        }

        ///  @}

        ///  @{
        ///  \name  Statistic members

    private:

        NsDeque<const NsIntVar *>  vars;
        NsUInt  domainsSizeMax;

        unsigned long  nFailures, nBacktracks, nGoals, nConstraintChecks,
                       backtrackLim;

    public:

        unsigned long
        numFailures (void)  const
        {
                return  nFailures;
        }

        unsigned long
        numGoals (void)  const
        {
                return  nGoals;
        }

        unsigned long
        numBacktracks (void)  const
        {
                return  nBacktracks;
        }

        NsStack<const NsIntVar *>::size_type
        numVars (void)  const
        {
                return  vars.size();
        }

        Ns_constraints_array_t::size_type
        numConstraints (void)  const
        {
                return  constraints.size();
        }

        unsigned long
        numConstraintChecks (void)  const
        {
                return  nConstraintChecks;
        }

        unsigned long
        numSearchTreeNodes (void)  const
        {
                return  searchNodes.numSearchTreeNodes();
        }

        ///  Collects the constrained variables of the problem.
        void
        addVar (const NsIntVar *Var)
        {
                vars.push_back( Var );
                if ( Var->size() > domainsSizeMax )
                        domainsSizeMax = Var->size();
        }

        ///  Removes the last variable pointer recorded, to correct a problem created by Ns_Expression::post(Var).
        void
        removeLastVar (void)
        {
                vars.resize( vars.size() - 1 );
        }

        ///  Prints statistics about the CSP parameters.
        void
        printCspParameters (void)  const
        {
                std::cout << numVars() << "\t"
                          << domainsSizeMax << "\t"
                          << numConstraints() << "\n";
        }

        ///  @}

        ///  @{
        ///  \name  Arrays of variables and constraints for memory management reasons

    private:

        NsDeque<NsIntVar *>  intermediateVars;

        Ns_constraints_array_t  constraints;

    public:

        void
        recordIntermediateVar (NsIntVar *Var)
        {
                intermediateVars.push_back( Var );
        }

        void
        recordConstraint (Ns_Constraint  *newConstr)
        {
                constraints.push_back( newConstr );
        }

        ///  @}

        ///  @{
        ///  \name  Provision of history ids to the search nodes

        unsigned long
        getCurrentNodeNum (void)  const
        {
                return  searchNodes.getCurrentNodeNum();
        }

        Ns_HistoryId_t
        getCurrentHistoryId (void)  const
        {
                return  searchNodes.getCurrentHistoryId();
        }

        bool
        isCurrentHistoryId (const Ns_HistoryId_t di)  const
        {
                return  searchNodes.isCurrentHistoryId(di);
        }

        bool
        isCurrentHistoryIdNextTo (const Ns_HistoryId_t di)  const
        {
                return  searchNodes.isCurrentHistoryIdNextTo(di);
        }

        bool
        isValidHistoryId (const Ns_HistoryId_t di)  const
        {
                return  searchNodes.isValidHistoryId(di);
        }

        ///  @}

#ifdef  Ns_LOCAL_SEARCH

        ///  @{
        ///  \name  Local Search

    public:

        ///  A list containing the variables that are included in violated constraints.
        NsList<NsIntVar *>  lsConflictingVariables;

        ///  An accessor to lsConflictingVariables.
        const NsList<NsIntVar *>&
        lsConflictingVars (void)  const
        {
                return  lsConflictingVariables;
        }

        ///  A list containing the violated constraints (a violated constraint is practically the list of its variables).
        NsList< NsList<NsIntVar *> >  lsViolatedConstrs;

        ///  Auxiliary list for lsViolatedConstraints: contains pointers to other entities that are involved in the corresponding violated constraints.
        NsList<NsIntVar::LsPointersToTuples_t>  lsPointersToTuples;

        ///  An accessor to lsViolatedConstrs.
        const NsList< NsList<NsIntVar *> >&
        lsViolatedConstraints (void)  const
        {
                return  lsViolatedConstrs;
        }

        ///  @}

#endif  // Ns_LOCAL_SEARCH

        ///  Saves the bitsetDomain--before being modified--for future backtracking purposes.
        void
        saveBitsetDomain (Ns_BitSet& bitsetDomain)
        {
                searchNodes.top().bitsetsStore.push(
                        Ns_SearchNode::BitsetCopy(bitsetDomain) );
                bitsetDomain.lastSaveHistoryId()  =
                        getCurrentHistoryId();
                //bitsetDomain.queueItem  =  0;
        }
};

}  // end namespace
#endif  // Ns_NAXOS_H

///  \example  nqueens.orig.cpp
///
///  An example of how to use the <span style="font-variant: small-caps;">
///   Naxos Solver </span> library to solve the <em>N</em>-queens problem.
///
///  The <em>N</em>-queens puzzle is the problem of putting eight chess
///   queens on an <em>N</em> x <em>N</em> chessboard such that none of
///   them is able to capture any other using the standard chess queen's
///   moves.  The queens must be placed in such a way that no two queens
///   would be able to attack each other. Thus, a solution requires that no
///   two queens share the same row, column, or diagonal.
///
///  Description source: Wikipedia.

///  \example  send_more_money.cpp
///
///  An example of how to use the <span style="font-variant: small-caps;">
///   Naxos Solver </span> library to solve the
///   <em>SEND</em>+<em>MORE</em>=<em>MONEY</em> problem.
///
///  Verbal arithmetic, also known as alphametics, cryptarithmetic,
///   crypt-arithmetic, or cryptarithm, is a type of mathematical game
///   consisting of a mathematical equation among unknown numbers, whose
///   digits are represented by letters. The goal is to identify the value
///   of each letter. The name can be extended to puzzles that use
///   non-alphabetic symbols instead of letters. This is often a technique
///   used by teachers to get students excited about math subjects such as
///   algebra 1 honors. Supposedly, students will think it is cool to add,
///   subtract, multiply, and divide their favorite words and letters.
///
///  The equation is typically a basic operation of arithmetic, such as
///   addition, multiplication, or division. The classic example, published
///   in the July 1924 issue of Strand Magazine by Henry Dudeney, is:
///
///\code
///    S E N D
///+   M O R E
///= M O N E Y
///\endcode
///
///  The solution to this puzzle is <em>O</em> = 0, <em>M</em> = 1,
///   <em>Y</em> = 2, <em>E</em> = 5, <em>N</em> = 6, <em>D</em> = 7,
///   <em>R</em> = 8, and <em>S</em> = 9.
///
///  Traditionally, each letter should represent a different digit, and (as
///   in ordinary arithmetic notation) the leading digit of a multi-digit
///   number must not be zero. A good puzzle should have a unique solution,
///   and the letters should make up a cute phrase (as in the example
///   above).
///
///  Description source: Wikipedia.
