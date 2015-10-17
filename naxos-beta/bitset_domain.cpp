///  \file
///  \brief  Constraint finite domain integer variables implementation,
///           based on bit-set.
///
///  \internal
///  This file is part of
///   Naxos Solver: A Constraint Programming Library
///   Copyright © 2007-2015  Nikolaos Pothitos
///
///  See ../license/LICENSE for the license of the library.

#include "naxos.h"

using namespace naxos;

namespace {

///  Returns the position in the bit-set of the corresponding bit to the value \a val.

///  This method performs a simple subtraction, to find out
///   the corresponding bit number for the value \a val.
///   However, it takes care about overflow issues, because
///   the limit for a bit number is \a NsUPLUS_INF and not
///   \a NsPLUS_INF!

inline NsUInt
correspondingBit (const NsInt val, const NsInt minDom)
{
        if ( minDom < 0  &&  val > 0 ) {
                // to avoid an overflow
                return  ( val + static_cast<NsUInt>( -minDom ) );
        }
        return  ( val - minDom );
}

///  Checks if the domain is continuous.

inline bool
isContinuous (const NsInt minVal, const NsInt maxVal,
              const NsUInt setCount)
{
        NsUInt  diff;
        // to avoid an overflow
        if ( minVal < 0  &&  maxVal > 0 )
                diff  =  maxVal + static_cast<NsUInt>( -minVal );
        else
                diff  =  maxVal - minVal;
        return  ( diff + 1 == setCount );
}

}                                                                // namespace

/////  Constructs a frame of the DomainStore_t.
//
//Ns_BitSet::DomainStore_t::DomainFrame_t::DomainFrame_t (
//              const NsProblemManager& pm/*,
//              const NsInt minVal_init,
//              const NsInt maxVal_init,
//              const NsUInt setCount_init*/)
//      : historyId(pm.getCurrentHistoryId())//,
//       //minVal(minVal_init), maxVal(maxVal_init), setCount(setCount_init)
//{     }
//
//
//
//
//
/////  Refreshes the top of the stack of domains, so that it contains a valid gap.
//
//      void
//Ns_BitSet::DomainStore_t::refresh (void)
//{
//      while ( ! pm.isValidHistoryId( domainStack.top().historyId ) )
//      {
//              domainStack.pop();
//      }
//}
//
//
//
//
/////  Returns the current valid DomainFrame_t.
//
//      Ns_BitSet::DomainStore_t::DomainFrame_t&
//Ns_BitSet::DomainStore_t::currentDomain (void)
//{
//      refresh();
//
//      return  domainStack.top();
//}
//
//
//
//
/////  Returns a DomainFrame_t for the current search level.  It is possible to modify it.
//
//      Ns_BitSet::DomainStore_t::DomainFrame_t&
//Ns_BitSet::DomainStore_t::modifyDomain (void)
//{
//      if ( domainStack.empty() )    {
//
//              domainStack.push( DomainFrame_t(pm) );
//
//      }  else if ( ! pm.isCurrentHistoryId(domainStack.top().historyId) )
//      {
//              DomainFrame_t&  dOld = currentDomain();
//
//              if ( isContinuous(dOld.minVal,dOld.maxVal,dOld.setCount) )  {
//                      dOld.minDom  =  dOld.minVal;
//
//                      dOld.nBits   =
//                          correspondingBit(dOld.maxVal,dOld.minDom) + 1;
//
//                      dOld.machw.clear();
//
//              }  else  {
//                      //  Trimming the empty words of the bit-set data
//                      //   structure.
//
//                      NsUInt  firstNonEmptyBit =
//                              correspondingBit(dOld.minVal,dOld.minDom);
//                      NsUInt  firstNonEmptyWord =
//                              firstNonEmptyBit / MW_BITS;
//
//
//                      dOld.minDom  +=  firstNonEmptyWord * MW_BITS;
//
//                      dOld.nBits   =
//                          correspondingBit(dOld.maxVal,dOld.minDom) + 1;
//
//
//                      NsUInt   newMachwSize = (dOld.nBits-1)/MW_BITS + 1;
//
//                      for (NsUInt i=0;   i < newMachwSize;   ++i)   {
//
//                              dOld.machw[i]  =
//                                      dOld.machw[firstNonEmptyWord + i];
//                      }
//
//                      dOld.machw.resize(newMachwSize);
//              }
//
//              domainStack.push( DomainFrame_t(pm) );
//
//              DomainFrame_t&  dNew = modifyDomain();
//
//              dNew.minDom    =  dOld.minDom;
//              dNew.minVal    =  dOld.minVal;
//              dNew.maxVal    =  dOld.maxVal;
//              dNew.nBits     =  dOld.nBits;
//              dNew.setCount  =  dOld.setCount;
//              dNew.machw     =  dOld.machw;
//      }
//
//      return  domainStack.top();
//}

///  Constructs the domain [\a minDom_init .. \a maxDom_init].

Ns_BitSet::Ns_BitSet (NsProblemManager& pm_init,
                      const NsInt minDom_init, const NsInt maxDom_init)
        : pm(&pm_init),
          minDom(minDom_init),
          minVal(minDom_init),
          maxVal(maxDom_init),
          nBits(correspondingBit(maxVal,minDom) + 1),
          setCount(nBits)
//queueItem(0)
{
        assert_Ns( NsMINUS_INF < minDom_init
                   && minDom_init <= maxDom_init
                   && maxDom_init < NsPLUS_INF ,
                   "Ns_BitSet::Ns_BitSet: Domain out of range");
        //  Making `lastSaveId' dirty as the domain has not been saved.
        lastSaveId.id     =  NsUPLUS_INF;
        //  Initializing `lastSaveId.level' to prevent valgrind from
        //   reporting a warning.
        lastSaveId.level  =  0;
        //DomainStore_t::DomainFrame_t&  d = domainStore.modifyDomain();
}

///  Removes a range/interval of values.

bool
Ns_BitSet::removeRange (NsInt rangeMin, NsInt rangeMax)
{
        //DomainStore_t::DomainFrame_t&  d = domainStore.modifyDomain();
        //  Check whether the domain will be cleared.
        if ( rangeMin <= minVal  &&  maxVal <= rangeMax )
                return  false;
        //  Check whether the domain will be kept intact.
        if ( rangeMax < minVal  ||  maxVal < rangeMin )
                return  true;
        //  The domain is going to be changed.
        //  Saving the domain for future backtracking purposes.
        if ( ! pm->isCurrentHistoryId(lastSaveHistoryId()) )
                pm->saveBitsetDomain(*this);
        if ( rangeMin  <  minVal )
                rangeMin  =  minVal;
        if ( rangeMax  >  maxVal )
                rangeMax  =  maxVal;
        if ( machw.empty() ) {   // Bounds Consistency
                if ( rangeMin  <=  minVal ) {
                        setCount  -=  rangeMax + 1  -  minVal;
                        minVal     =  rangeMax + 1;
                        return  true;
                } else if ( maxVal <= rangeMax ) {
                        setCount  -=  maxVal -  (rangeMin - 1);
                        maxVal     =  rangeMin - 1;
                        return  true;
                } else {
                        //  Creating the array `machw'.
                        minDom  =  minVal;
                        nBits   =  correspondingBit(maxVal,minDom) + 1;
                        machw.resize((nBits-1)/MW_BITS + 1);
                        for (NsUInt i=0;   i < machw.size();   ++i)
                                machw[i]  =  ~static_cast<size_t>(0u);
                        //  Setting the bits of the last machine word.
                        if ( nBits % MW_BITS  !=  0 ) {
                                for (size_t bit=static_cast<size_t>(1)<<(nBits%MW_BITS);
                                     bit != 0;
                                     bit<<=1) {
                                        machw[machw.size()-1]  &=  ~bit;
                                }
                        }
                        //  Continuing on reseting the value...
                }
        }
        bool  changedMinVal = false,  changedMaxVal = false;
        for (NsInt val=rangeMin;   val <= rangeMax;  ++val) {
                //std::cout << "val = " << val << "\n";
                NsUInt  nbit  =  correspondingBit(val, minDom);
                NsUInt  mw    =  nbit / MW_BITS;
                assert_Ns( minVal <= val  &&  val <= maxVal
                           && nbit < nBits ,
                           "Ns_BitSet::removeRange: Machine word out of `*this' range");
                if ( machw[mw]  ==  0u ) {
                        // speedup by means of comparing the whole word
                        if ( MW_BITS - nbit % MW_BITS  >
                             static_cast<NsUInt>( rangeMax - val ) ) {
                                break;
                        }
                        val  +=  MW_BITS - nbit % MW_BITS - 1;
                        // `-1' above is used to compensate
                        //  the for's `++val' increment.
                }  else if ( machw[mw] == ~static_cast<size_t>(0u)
                             &&  nbit % MW_BITS == 0
                             &&  val + static_cast<NsInt>( MW_BITS ) - 1
                             <= rangeMax ) {
                        // speedup by means of comparing the whole word
                        machw[mw]  =  0u;
                        setCount  -=  MW_BITS;
                        if ( val  ==  minVal )
                                changedMinVal  =  true;
                        val  +=  MW_BITS - 1;
                        // `-1' above is used to compensate
                        //  the for's `++val' increment.
                        if ( val  ==  maxVal )
                                changedMaxVal  =  true;
                } else {
                        size_t  mwbit  =  static_cast<size_t>(1)<<(nbit%MW_BITS);
                        if ( machw[mw] & mwbit ) {
                                machw[mw]  &=  ~mwbit;
                                --setCount;
                                if ( val  ==  minVal )
                                        changedMinVal  =  true;
                                if ( val  ==  maxVal )
                                        changedMaxVal  =  true;
                        }
                }
        }
        if ( changedMinVal )
                minVal  =  next(minVal);
        if ( changedMaxVal )
                maxVal  =  previous(maxVal);
        return  true;
}

NsInt
Ns_BitSet::previous (const NsInt val)  const
{
        //const DomainStore_t::DomainFrame_t&  d = domainStore.currentDomain();
        if ( val  <=  minVal )
                return  NsMINUS_INF;
        if ( val  >   maxVal )
                return  maxVal;
        if ( isContinuous(minVal,maxVal,setCount) )
                return  (val - 1);               // Bounds Consistency
        NsUInt     nbit  =  correspondingBit(val,minDom) - 1;
        NsInt        mw  =  nbit / MW_BITS;
        size_t    mwbit  =  static_cast<size_t>(1)<<(nbit%MW_BITS);
        assert_Ns( nbit < nBits ,
                   "Ns_BitSet::previous: Machine word out of `*this' range");
        if ( machw[mw]  ==  0u ) {
                // speedup by means of comparing the whole word
                nbit  =  mw * MW_BITS -1;
        } else {
                for ( ;   mwbit != 0;   mwbit>>=1) {
                        if ( machw[mw] & mwbit )
                                return  (nbit + minDom);
                        --nbit;
                }
        }
        --mw;
        for ( ;   mw >= 0;   --mw) {
                if ( machw[mw]  ==  0u ) {
                        // speedup by means of comparing the whole word
                        nbit  -=  MW_BITS;
                } else {
                        for (mwbit=static_cast<size_t>(1)<<(MW_BITS-1);  mwbit!=0;  mwbit>>=1) {
                                if ( machw[mw] & mwbit )
                                        return  (nbit + minDom);
                                --nbit;
                        }
                }
        }
        throw  NsException("Ns_BitSet::previous: Should not reach here");
}

NsInt
Ns_BitSet::next (const NsInt val)  const
{
        //const DomainStore_t::DomainFrame_t&  d = domainStore.currentDomain();
        if ( val  >=  maxVal )
                return  NsPLUS_INF;
        if ( val  <  minVal )
                return  minVal;
        if ( isContinuous(minVal,maxVal,setCount) )
                return  (val + 1);               // Bounds Consistency
        NsUInt     nbit  =  correspondingBit(val,minDom) + 1;
        NsUInt       mw  =  nbit / MW_BITS;
        size_t    mwbit  =  static_cast<size_t>(1)<<(nbit%MW_BITS);
        assert_Ns(nbit < nBits ,
                  "Ns_BitSet::next: Machine word out of `*this' range");
        if ( machw[mw]  ==  0u ) {
                // speedup by means of comparing the whole word
                nbit  =  (mw + 1) * MW_BITS;
        } else {
                for ( ;   mwbit != 0;   mwbit<<=1) {
                        if ( machw[mw] & mwbit )
                                return  (nbit + minDom);
                        ++nbit;
                }
        }
        ++mw;
        for ( ;   mw < machw.size();   ++mw) {
                if ( machw[mw]  ==  0u ) {
                        // speedup by means of comparing the whole word
                        nbit  +=  MW_BITS;
                } else {
                        for (mwbit=1;   mwbit != 0;   mwbit<<=1) {
                                if ( machw[mw] & mwbit )
                                        return  (nbit + minDom);
                                ++nbit;
                        }
                }
        }
        throw  NsException("Ns_BitSet::next: Should not reach here");
}

NsInt
Ns_BitSet::nextGap (const NsInt val)  const
{
        //const DomainStore_t::DomainFrame_t&  d = domainStore.currentDomain();
        if ( val >= maxVal-1
             ||  isContinuous(minVal,maxVal,setCount) ) {
                // || Bounds Consistency
                return  NsPLUS_INF;
        }
        NsUInt  nbit;
        if ( val  <  minVal )
                nbit  =  correspondingBit(minVal,minDom) + 1;
        else
                nbit  =  correspondingBit(val,minDom) + 1;
        NsUInt       mw  =  nbit / MW_BITS;
        size_t    mwbit  =  static_cast<size_t>(1)<<(nbit%MW_BITS);
        NsUInt   maxbit  =  correspondingBit(maxVal,minDom);
        assert_Ns( maxbit < nBits  &&  nbit < nBits ,
                   "Ns_BitSet::nextGap: Machine word out of `*this' range");
        if ( machw[mw]  ==  ~static_cast<size_t>(0u) ) {
                // speedup by means of comparing the whole word
                nbit  =  (mw + 1) * MW_BITS;
        } else {
                for ( ;   mwbit!=0  &&  nbit <= maxbit;   mwbit<<=1) {
                        if ( ! ( machw[mw] & mwbit ) )
                                return  (nbit + minDom);
                        ++nbit;
                }
        }
        ++mw;
        for ( ;   mw < machw.size();   ++mw) {
                if ( machw[mw]  ==  ~static_cast<size_t>(0u) ) {
                        // speedup by means of comparing the whole word
                        nbit  +=  MW_BITS;
                } else {
                        for (mwbit=1;  mwbit!=0 && nbit<=maxbit;  mwbit<<=1) {
                                if ( ! ( machw[mw] & mwbit ) )
                                        return  (nbit + minDom);
                                ++nbit;
                        }
                }
        }
        return  NsPLUS_INF;
}

///  Returns \c true if the domain contains the range [\a rangeMin..\a rangeMax].

bool
Ns_BitSet::containsRange (const NsInt rangeMin, const NsInt rangeMax)  const
{
        //const DomainStore_t::DomainFrame_t&  d = domainStore.currentDomain();
        assert_Ns( rangeMin <= rangeMax ,
                   "Ns_BitSet::containsRange: Invalid range");
        if ( rangeMin < minVal  ||  rangeMax > maxVal )
                return  false;
        if ( isContinuous(minVal,maxVal,setCount) )
                return  true;                    // Bounds Consistency
        for (NsInt val=rangeMin;   val <= rangeMax;  ++val) {
                //std::cout << "val = " << val << "\n";
                NsUInt  nbit  =  correspondingBit(val,minDom);
                NsUInt  mw    =  nbit / MW_BITS;
                assert_Ns( nbit < nBits ,
                           "Ns_BitSet::containsRange: Machine word out of `*this' range");
                if ( machw[mw]  ==  ~static_cast<size_t>(0u) ) {
                        // speedup by means of comparing the whole word
                        if ( MW_BITS - nbit % MW_BITS  >
                             static_cast<NsUInt>( rangeMax - val ) ) {
                                break;
                        }
                        val  +=  MW_BITS - nbit % MW_BITS - 1;
                        // `-1' above is used to compensate
                        //  the for's `++val' increment.
                } else {
                        size_t  mwbit  =  static_cast<size_t>(1)<<(nbit%MW_BITS);
                        if ( ! ( machw[mw] & mwbit ) )
                                return  false;
                }
        }
        return  true;
}
