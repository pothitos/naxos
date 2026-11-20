 - [Previous section](Arrays.md)
 - [Contents](README.md)

---

# Problem Manager

Before the problem statement, we should declare a _problem
manager_ (class `NsProblemManager`). This manager holds all
the information needed about the variables, the constraint
network being built, and the goals that are going to be
executed. The constructor function does not have any
argument. The other functions follow.


#### `NsIndex add(`_ExprConstr_`)`

Adds the constraint described by the constraint expression
_ExprConstr_ (see the [Expressions for
Constraints](Expressions.md) section). In a constraint
expression we can use condition operators (`<`, `==`, `!=`,
etc.) or built-in expressions like `NsAllDiff()` that states
that all the variables of an array must have different
values. E.g.

```C++
pm.add(3 * VarX != VarY / 2);
pm.add(VarW == -2 || VarW >= 5);
pm.add(NsAllDiff(VarArr));
```

It returns the index of the constraint, which is the number
of the previous `add()` calls, i.e. 0 for the first
constraint, 1 for the second, etc. The index can be passed
to `numFailures()` and `numPrunings()`.


#### `void addGoal(NsGoal* goal)`

Adds `goal` into the list with the goals that have to be
executed/satisfied (see the [Search via
Goals](Goals.md) section).


#### `bool nextSolution()`

Finds the next solution. The goals that have been added are
going to be satisfied. If there is no solution, `false` is
returned.


#### `void minimize(`_Expression_`)`

It gives solver the instruction to minimize the value of
_Expression_. Every time that the solver finds a solution,
the _Expression_ will be less than the one of the previous
solution (branch-and-bound algorithm). In fact, if
`nextSolution()` gives a solution and the _Expression_
maximum value is _a_, the constraint _Expression_ < _a_ is
imposed, for the next time `nextSolution()` is called.
Therefore, after each solution, the _Expression_ gets
reduced. If it cannot be further reduced, `nextSolution()`
returns `false`, and we should have stored somewhere the
last (best) solution. E.g.

```C++
pm.minimize(VarX + VarY);
while (pm.nextSolution() != false)
    { /* STORE SOLUTION */ }
```

If we wish to maximize an _Expression_, we can simply put a
`-` in front of it and call `minimize(`-_Expression_`)`.


#### `void objectiveUpperLimit(NsInt max)`

During search, this method defines an upper bound for the
solution cost equal to `max`; the solution cost is expressed
by the `NsProblemManager::minimize()` argument. In other
words, the constraint *cost_variable* <= `max` is imposed.

Besides, after the beginning of search—when `nextSolution()`
is called for the first time—we cannot add more constraints
by calling `NsProblemManager::add`, e.g. via `pm.add(X <=
5)`. Only this function (`objectiveUpperLimit`) can impose
such a constraint, but only onto the cost variable.


#### `void timeLimit(unsigned long secs)`

Search will last at most `secs` seconds. After this
deadline, `nextSolution()` will return `false`.


#### `void realTimeLimit(unsigned long secs)`

It works like the previous function, but the `secs` seconds
here are real time; in the previous function it was the time
that CPU has spent for the solver _exclusively_.


#### `void backtrackLimit(unsigned long x)`

If `x` is greater than zero, it makes `nextSolution()`
return `false` after search has backtracked `x` times, from
the moment that this function was called.


#### `unsigned long numFailures()`

Returns the number of failures during search.


#### `unsigned long numFailures(NsIndex constraint)`

Returns how many times the constraint with the index
`constraint`, which `add()` returned, emptied the domain of a
variable. The failures of a problem without solutions point
to the constraints that are more likely to conflict.


#### `unsigned long numPrunings(NsIndex constraint)`

Returns how many times the constraint with the index
`constraint` removed values from the domain of a variable,
without emptying it. A constraint with zero prunings and
zero failures did not remove any value during the search so
far. In this sense, it was not used to find the solutions,
although another order of the `add()` calls could make it
useful. For example, after `pm.add(X < Y)`, the constraint
`pm.add(X <= Y)` never removes a value.

The statistics count only the constraints that `add()`
imposed. They do not count the constraints that come from an
expression assigned to a variable, e.g. `NsIntVar Z = X + Y`.


#### `unsigned long numBacktracks()`

Returns how many times the solver has backtracked during
search.


#### `unsigned long numGoals()`

Returns how many goals have been executed.


#### `unsigned long numVars()`

Returns the number of the constrained variables that have
been created. Note that the number includes
intermediate/auxiliary variables—if any—that the solver has
automatically constructed.


#### `unsigned long numConstraints()`

Returns the number of the problem constraints. Note that the
number includes intermediate/auxiliary constraints—if
any—that the solver has automatically created.


#### `unsigned long numSearchTreeNodes()`

Returns the number of nodes of the search tree that the
solver has already visited.


#### `void searchToGraphFile(char *fileName)`

Stores into a file named `fileName` a representation of the
search tree. The file format is called `dot`, and the
application Graphviz can graphically display it.


#### `void restart()`

Restores the constrained variables (i.e. their domains) to
the state they were initially, a little bit after the
_first_ `nextSolution()` was called. More specifically, it
restores the state that existed immediately before the first
`nextSolution()` call, but keeps the changes that were made
in order to achieve the first arc-consistency of the
constraint network (see the [Search via Goals](Goals.md)
section for the arc-consistency definition). In other words,
this function restores the constraint/variable network to
the first arc-consistency state that took ever place (before
the execution of any goal).

This method also cancels all the goals that were about to be
executed. That is, if we wish to begin search (with a
`nextSolution()` call) after a `restart()`, we have to
declare a goal to be executed (using `addGoal()`), otherwise
there is no goal!

`restart()` does _not_ affect the variable that was the
argument of `minimize()`—also known as _objective_ or _cost
variable_. That is, it does _not_ restore this variable to
its initial state. For example, if the objective variable
had initially the domain `[0..100]` and before `restart()`
had the domain `[0..10]`, then after `restart()` is called,
the domain will be kept `[0..10]`.

We cannot call this function inside goals, but outside them.
E.g. we can call it at the code “level” we call
`nextSolution()`.


## Explaining a problem without solutions

#### `NsDeque<NsIndex> NsMinimalConflict(NsIndex n, isSatisfiable)`

When a problem has no solution, this function finds a
minimal set of its constraints that cannot be satisfied
together. _Minimal_ means that if we remove any constraint
of the set, the rest can be satisfied. The constraints are
numbered from 0 to `n` - 1. The function `isSatisfiable`
takes a sorted `NsDeque<NsIndex>` with some of these
numbers, builds a _new_ problem manager with only these
constraints, and returns `true` if the problem has a
solution. `NsMinimalConflict` returns the sorted numbers of
the conflicting constraints, or an empty `NsDeque` if the
whole problem has a solution. E.g.

```C++
bool isSatisfiable(const NsDeque<NsIndex>& subset)
{
    NsProblemManager pm;
    NsIntVar X(pm, 0, 9), Y(pm, 0, 9), Z(pm, 0, 9);
    for (NsIndex i = 0; i < subset.size(); ++i) {
        switch (subset[i]) {
        case 0: pm.add(X < Y); break;
        case 1: pm.add(Y < Z); break;
        case 2: pm.add(X != 4); break;
        case 3: pm.add(Z < X); break;
        }
    }
    NsIntVarArray vars;
    vars.push_back(X);
    vars.push_back(Y);
    vars.push_back(Z);
    pm.addGoal(new NsgLabeling(vars));
    return pm.nextSolution();
}

NsDeque<NsIndex> conflict = NsMinimalConflict(4, isSatisfiable);
// conflict contains 0, 1, and 3
```

The function implements the QuickXplain algorithm by Ulrich
Junker. It solves a few subproblems, so it may take time for
hard problems. `isSatisfiable` should search the whole
subproblem, without a time limit, otherwise the result may
be wrong. A complete example is
[`apps/conflict.cpp`](../apps/conflict.cpp).

---

 - [Next section](Expressions.md)
