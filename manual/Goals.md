 - [Previous section](Examples.md)
 - [Contents](README.md)

---

# Search via Goals

## Introduction

A pair of variables (_x_,_x'_) is _consistent_, if for each
value _v_ in the domain of _x_, there is a value _v'_ in the
domain of _x'_ such that every constraint that connects the
two variables is satisfied. When every pair of variables is
consistent, then we say that the constraint network is
_arc-consistent_. Arc-consistency does not necessarily mean
that we have a solution—but if the constraint network is not
arc-consistent, we are sure that there is no solution.
Therefore, we have to combine arc-consistency with a search
method. Besides, arc-consistency reduces the search space
that a search method—such as depth first search (DFS) or
limited discrepancy search (LDS) etc.—has to explore.

It is known that in most problems arc-consistency does not
suffice to find a solution (see also the following figure).
After a specific point, we should begin searching, by
repeating the assignment of values to variables and by
checking every time—e.g. after every assignment—if the
constraint network is arc-consistent, according to the
_maintaining arc-consistency_ (MAC) methodology. If an
assignment causes an inconsistency, then it should be
canceled, and another value should be chosen.

![Three arc-consistent constraint
networks](https://rawgit.com/pothitos/naxos/master/manual/figures/AC.svg)

In order to facilitate or, better, to guide search, a _goals
mechanism_ has been implemented in the solver. The
application developer that uses the solver can declare their
own goals, or they can use the built-in ones. A goal often
makes an assignment to a constrained variable, or it removes
a value from the domain. If search reaches a dead-end, the
solver automatically cancels the goals that guided to it,
and the constraint network with its variables is restored
back to the state before those goals were executed.

Generally speaking, a goal can assign or remove values to
one or more variables, or it can be used to choose a
variable in order to be successively assigned a value. In
this way it defines the search method. While a goal
terminates, it can optionally generate another goal; this
possibility provides recursion characteristics to the goals
mechanism. Last but not least, there are also the AND and OR
_meta-goals_. They are called “meta-goals” because each of
them is used to manipulate two _other_ goals, namely
_subgoals_. An AND-goal succeeds if its two subgoals succeed
both, while an OR-goal succeeds if one or more of its
subgoals succeed.

It is worth to mention that the OR-goals are also known as
_choice points_. Indeed, they are points where we have two
alternatives, that is points where the search tree branches
off. During the execution of an OR-goal, its first subgoal
is chosen, and if it finally fails, the solver cancels all
the chain modifications that were made on the domains of the
variables (after the first subgoal execution); the second
subgoal is then tried. If the second subgoal also fails,
then the whole OR-goal fails.


## Object-oriented modelling

The declaration for the basic goal class in _Naxos Solver_
follows.

```C++
class NsGoal {
    public:
        virtual bool isGoalAND(void) const;
        virtual bool isGoalOR(void) const;
        virtual NsGoal* getFirstSubGoal(void) const;
        virtual NsGoal* getSecondSubGoal(void) const;

        virtual NsGoal* GOAL(void) = 0;
};
```

The `NsgAND` and `NsgOR` meta-goal classes derive from the
above `NsGoal` class. `NsgAND` and `NsgOR` constructor
functions take two arguments (of `NsGoal*` type), that
represent their two subgoals. Every `NsGoal`
member-function—apart from `GOAL()`—has to do with
meta-goals. The application developer that wants to define
their own goals, has to take only care of `GOAL()`.

Every custom goal defined by the application developer
should be a class that (directly or indirectly) extends
`NsGoal`. Subsequently, function `GOAL()` should be defined
in every goal class. Evidently, the goal class may also
contain other member-functions—except from the ones also
contained in the basic class, to avoid misunderstandings.

`GOAL()` is a critical method, as the solver executes it
every time it tries to satisfy a goal. This method returns a
pointer to another `NsGoal` instance, i.e. it returns the
next goal to be satisfied. If the pointer equals to `0`,
this means that the current goal succeeded (was satisfied),
and thus no other goal has to be created.

Therefore, an example follows, illustrating goals already
built in the solver, as they are widely used. These goals
describe the search method _depth-first-search_ (DFS).

```C++
class NsgInDomain : public NsGoal {

    private:
        NsIntVar& Var;

    public:
        NsgInDomain(NsIntVar& Var_init)
          : Var(Var_init)
        {
        }

        NsGoal* GOAL(void)
        {
            if (Var.isBound())
                return 0;
            NsInt value = Var.min();
            return (new NsgOR(new NsgSetValue(Var,value),
                              new NsgAND(new NsgRemoveValue(Var,value),
                                         new NsgInDomain(*this))));
        }
};

class NsgLabeling : public NsGoal {

    private:
        NsIntVarArray& VarArr;

    public:
        NsgLabeling (NsIntVarArray& VarArr_init)
          : VarArr(VarArr_init) { }

        NsGoal* GOAL(void)
        {
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
```

We observe the operator `new` in the return value of
`GOAL()` (when it is not `0`) and in the meta-goals
(`NsgAND` and `NsgOR`) constructor functions. `new` is
necessary when constructing a pointer to a goal. The solver
is responsible to destruct the goal when it becomes useless,
using the `delete` operator. That is why _all the goals that
we create must be constructed with the `new` operator, and
we must **not** `delete` them by ourselves_.

Regarding the practical meaning of the example, when we ask
the solver to satisfy the goal `NsgLabeling(VarArr)`, we
except that all the variables of `VarArr` will be assigned
values. Thus, the function `GOAL()` of `NsgLabeling` chooses
a variable (specifically, the one with the smallest domain
size according to the first-fail heuristic). Then it asks
(via the goal `NsgInDomain` that assigns to a variable, its
domain minimum value) to instantiate the variable _and_ to
satisfy the goal `this`. This goal—that refers to a kind of
“recursion”—constructs another `NsgLabeling` instance, that
is identical to the current one. In fact, `this` tells the
solver to assign values to the rest of `VarArr` variables.
When `GOAL()` returns `0`, we have finished.

![The combination of the goals that compose
NsgLabeling](https://rawgit.com/pothitos/naxos/master/manual/figures/NsgLabeling.svg)

While `NsgLabeling` chooses a variable to be instantiated,
`NsgInDomain` chooses the value to assign to the variable.
More specifically, it always chooses the minimum value of
the domain of the variable. Then it calls the built-in goal
`NsgSetValue` that simply assigns the value to the variable.
If it is proved afterwards that this value does not guide to
a solution, it is removed from the domain by the goal
`NsgRemoveValue`, and another value will be assigned (by
`NsgInDomain(*this)`).

Usually, when we face difficult and big problems, we should
define our own goals, like `NsgLabeling` and `NsgInDomain`.
The aim is to make search more efficient by using heuristic
functions to take better decisions/choices.

---

 - [Contents](README.md)
