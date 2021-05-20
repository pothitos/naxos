 - [Contents](README.md)

---

# Introduction

A _constraint satisfaction problem_ (CSP) contains a set of
_constrained variables_, that can be simply called
_variables_; each variable corresponds to a _domain_.
Constrained variables are connected to each other via a set
of _constraints_. Generally speaking, a constraint that
involves specific constrained variables is a set with all
valid combinations of values that can be assigned. For
example, if we take the variables _x_ and _y_ with domains
{0,1,2,3}, the equality constraint can be declared as
_C_({_x_,_y_}, {(0,0),(1,1),(2,2),(3,3)}). Although this
notation for the constraint is as generic as possible, in
practice (i.e. in Constraint Programming) we use simple
relations to describe the constraint networks. In the above
example the constraint can be simply written as _x_ = _y_. A
_solution_ to a constraint satisfaction problem is a valid
assignment of a value to every constraint variable, that
satisfies all the constraints. Finally, it should be noted
that the advantage of Constraint Programming is that it
allows the separation of the problem declaration process and
the solution generation mechanism.

_Naxos Solver_ is a library that solves constraint
satisfaction problems, that was designed for the C++
object-oriented programming environment. The solver is
threadsafe, i.e. safe to use in a multithreaded environment.
“Internal” classes, methods, functions, etc. that are not
stated in this manual, should not be used by the application
developer, as they may change in future. Still, to avoid
misunderstandings, we should not name our own variables,
classes, etc. with names that begin with `Ns` as this is the
prefix for the solver built-in classes and constants.
Finally, note that the solver does not check for any
possible overflows (e.g. during the addition of two big
integers) for performance reasons.

Part of the solver design and its naming conventions are
influenced by the Standard Template Library (STL) modelling
for C++. E.g. several iterators are used and implemented.

There is _no_ distinction between handle-classes and
implementation-classes, as in _Ilog Solver_. This
distinction exists in _Ilog Solver_, because it attempts to
automatically manage memory resources à la Java. In every
handle-class there exists a reference to an
implementation-class instance. It is possible that many
handle-class instances point to the same
implementation-class instance. The implementation-class
instance is destructed only when all the handle-class
instances that point to it are destructed; something similar
happens with the references in Java. Thus, in a function in
_Ilog Solver_ it is possible to construct automatic
variables-instances in order to describe a constraint; the
constraint and the variables involved will continue to exist
after the function returns. In the same circumstance in
_Naxos Solver_ we would have a segmentation fault.

---

 - [Next section](Errors.md)
