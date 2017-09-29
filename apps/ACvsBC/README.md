# Arc Consistency vs. Bounds Consistency Sources

:warning: Here is the _source code_ that supports the
corresponding [article](https://github.com/pothitos/ACvsBC).

## Mission Statement

We are trying to track the cases when _bounds consistency_
(BC) is more efficient than _arc consistency_ (AC) in
Constraint Programming.

## Intuition

Let _n_ be the number of the constraint variables and _d_
the cardinality of their maximum domain, in a _constraint
satisfaction problem_ (CSP).There seems to be a threshold of
the ratio _d_/_n_: after this theshold, maintaining _bounds
consistency_ is more efficient than maintaining _arc
consistency_.

## Completed Tasks

Our above intuition seems true for a couple of CSPs we've
solved so far.

 - _N_ Queens
 - Magic Square
 - Crew Scheduling

## Current Task

 - Extend the above problems range.

## Future Task

 - Support the intuition through algebraic evidence.
