/// @file
/// Internal functions declarations
///
/// These functions are used in more than one file.
/// Part of https://github.com/pothitos/naxos

#ifndef Ns_INTERNAL_H
#define Ns_INTERNAL_H

#include "naxos-mini.h"

bool intersectionEmpty(const naxos::NsIntVar* VarY,
                       const naxos::NsIntVar* VarZ);

void YmodC_min_max(const naxos::NsIntVar* VarY, const naxos::NsInt C,
                   naxos::NsInt& min, naxos::NsInt& minmax);

void CmodY_min_max(const naxos::NsInt C, naxos::NsIntVar* VarY,
                   naxos::NsInt& min, naxos::NsInt& minmax);

void product_min_max(const naxos::NsIntVar* VarY, const naxos::NsIntVar* VarZ,
                     naxos::NsInt& min, naxos::NsInt& max);

void quotient_min_max(const naxos::NsIntVar* VarY, naxos::NsIntVar* VarZ,
                      naxos::NsInt& min, naxos::NsInt& max);

void remainder_min_max(const naxos::NsIntVar* VarY, naxos::NsIntVar* VarZ,
                       naxos::NsInt& min, naxos::NsInt& max);

void array_min_minmax(const naxos::NsIntVarArray* VarArr, naxos::NsInt& min,
                      naxos::NsInt& minmax);

void array_maxmin_max(const naxos::NsIntVarArray* VarArr, naxos::NsInt& maxmin,
                      naxos::NsInt& max);

void array_sum_min_max(const naxos::NsIntVarArray* VarArr,
                       const naxos::NsIndex start, const naxos::NsIndex length,
                       naxos::NsInt& summin, naxos::NsInt& summax);

void array_VarArr_elements_min_max(const naxos::NsIntVarArray& VarArr,
                                   naxos::NsIntVar& VarIndex, naxos::NsInt& min,
                                   naxos::NsInt& max);

void update_min_max(const naxos::NsInt candidate, naxos::NsInt& min,
                    naxos::NsInt& max);

#endif // Ns_INTERNAL_H
