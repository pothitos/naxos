///  \file
///  \brief  Internal functions declarations.  (Those functions are used in more than one file.)
///
///  \internal
///  This file is part of
///   <span style="font-variant: small-caps;"> Naxos Solver: </span>
///   A Constraint Programming Library. \n
///   Copyright © 2007-2015  Nikolaos Pothitos.
///
///  See ../license/LICENSE for the license of the library.

#ifndef Ns_INTERNAL_H
#define Ns_INTERNAL_H

#include "naxos.h"

bool
intersectionEmpty (const naxos::NsIntVar *VarY, const naxos::NsIntVar *VarZ);

void
YmodC_min_max (const naxos::NsIntVar *VarY, const naxos::NsInt C, naxos::NsInt& min, naxos::NsInt& minmax);

void
product_min_max (const naxos::NsIntVar *VarY, const naxos::NsIntVar *VarZ, naxos::NsInt& min, naxos::NsInt& max);

void
quotient_min_max (const naxos::NsIntVar *VarY, naxos::NsIntVar *VarZ, naxos::NsInt& min, naxos::NsInt& max);

void
array_min_minmax (const naxos::NsIntVarArray *VarArr, naxos::NsInt& min, naxos::NsInt& minmax);

void
array_maxmin_max (const naxos::NsIntVarArray *VarArr, naxos::NsInt& maxmin, naxos::NsInt& max);

void
array_sum_min_max (const naxos::NsIntVarArray *VarArr, const naxos::NsIndex start, const naxos::NsIndex length, naxos::NsInt& summin, naxos::NsInt& summax);

///  Returns floor( x / y ).  (Overrides C problems when an operand is negative.)

inline naxos::NsInt
xDIVy (const naxos::NsInt x, const naxos::NsInt y)
{
	if ( x < 0  &&  y < 0 )
		return  (-x / -y);
	else if ( x  <  0 )
		return  -((-x+y-1) / y);
	else if ( y  <  0 )
		return  -((x-y-1) / -y);
	else
		return  (x / y);
}
#endif							 // Ns_INTERNAL_H
