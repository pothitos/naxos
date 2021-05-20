 - [Previous section](ProblemManager.md)
 - [Contents](README.md)

---

# Expressions

In order to connect the variables, we take advantage of the
overloaded operators, and we create expressions and
combinations of them. A simple expression can be even a
variable or an integer. An expression is designated
_Expression_.


## Expressions for constraints

Expressions for constraint are denoted _ExprConstr_, and
they are a subcategory of the general expression category
_Expression_. They are mainly used as
`NsProblemManager::add()` arguments and for the creation of
meta-constraints. The following are _ExprConstr_:

 * _Expression_<sub>1</sub> op _Expression_<sub>2</sub>,
   op ∈ {`<`, `<=`, `>`, `>=`, `==`, `!=`}

 * `!(`_ExprConstr_`)`

 * _ExprConstr_<sub>1</sub> op _ExprConstr_<sub>2</sub>,
   op ∈ {`&&`, `||`}

 * `NsIfThen(`_ExprConstr_<sub>1</sub>`,
             `_ExprConstr_<sub>2</sub>`)`

 * `NsEquiv(`_ExprConstr_<sub>1</sub>`,
            `_ExprConstr_<sub>2</sub>`)`

 * `NsCount(`_VarArr_`, `_IntArr_<sub>1</sub>`,
            `_IntArr_<sub>2</sub>`)`

 * `NsSupports(`_VarArr_`, `_IntArr_`)`

 * `NsConflicts(`_VarArr_`, `_IntArr_`)`

 * `NsAllDiff(`_VarArr_`)`

Therefore, the definition is recursive. The last expression
means that the constrained variables inside _VarArr_ (an
`NsIntVarArray`) are different between them. If we use the
expression `NsAllDiff(`_VarArr_`, `_Capacity_`)`, where
_Capacity_ is a positive integer, then there will be more
constraint propagation. E.g. if _VarArr_ = {`[1..2]`,
`[1..2]`, `[1..5]`}, then by declaring
`NsAllDiff(`_VarArr_`, 1)` it infers that _VarArr_ =
{`[1..2]`, `[1..2]`, `[3..5]`}, but using the simple
expression `NsAllDiff(`_VarArr_`)` we would have a value
removal only when a variable of _VarArr_ becomes assigned.
However, this more powerful consistency may “cost” in terms
of time. Finally, the integer _Capacity_ is the maximum
number of occurrences that a value can have inside the array
_VarArr_.

On the other hand, the array _VarArr_ as declared in the
expression `NsCount(`_VarArr_`, `_Values_`,
`_Occurrences_`)` consists of _constrained_ variables, but
the other two arrays, namely _Values_ and _Occurrences_,
contain integers, as their type is `NsDeque<NsInt>`. The
array _Values_ contains the _values_ to be distributed
inside _VarArr_, while the array _Occurrences_ contains how
many times each value (in _Values_) appears inside _VarArr_.
E.g. if _Values_`[i] = 34` and the corresponding
_Occurrences_`[i] = 2`, then the value `34` will be assigned
to exactly `2` constrained variables in _VarArr_.

The constraint `NsIfThen(`_p_`, `_q_`)` implies the logical
proposition _p_ ⇒ _q_, and the constraint `NsEquiv(`_p_`,
`_q_`)` means _p_ ⇔ _q_. The two constraints can also be
expressed with an equivalent way as `(!`_p_` || `_q_`)` and
`(`_p_` == `_q_`)` respectively.

Some examples of expressions for constraints follow:

```C++
VarX < VarY
!(X == Y || X + Y == -3)
(X == Y) != 1
```

Finally the constraint `NsSupports(`_VarArr_`, `_IntArr_`)`
implies that if the constrained variables of _VarArr_ are
instantiated, the tuple with their values must belong to
_IntArr_.  _IntArr_ is a two-dimensional array of integers;
its type is `NsDeque<NsDeque<NsInt>>`. In other words, there
should exist an _i_ with _VarArr_[0] = _IntArr_[_i_][0],
_VarArr_[1] = _IntArr_[_i_][1], …,  _VarArr_[_n_ - 1] =
_IntArr_[_i_][_n_ - 1], where _n_ is the size of the arrays.

On the other hand, `NsConflicts(`_VarArr_`, `_IntArr_`)`
means that if the constrained variables of _VarArr_ are
instantiated, the tuple with their values must __not__
belong to _IntArr_. In other words, there should __not__
exist an _i_ with _VarArr_[0] = _IntArr_[_i_][0],
_VarArr_[1] = _IntArr_[_i_][1], …,  _VarArr_[_n_ - 1] =
_IntArr_[_i_][_n_ - 1].


## General expressions

Apart from _ExprConstr_, the following also belong to the
category of general expressions _Expression_:

 * _Expression_<sub>1</sub> op _Expression_<sub>2</sub>,
   op ∈ {`+`, `-`, `*`, `/`, `%`}

 * `NsAbs(`_Expression_`)`

 * `NsMin(`_VarArr_`)`

 * `NsMax(`_VarArr_`)`

 * `NsSum(`_VarArr_`)`

 * `NsSum(`_VarArr_`, `_start_`, `_length_`)`

 * _IntArr_`[`_Expression_`]`

 * _VarArr_`[`_Expression_`]`

An _Expression_—except from describing a constraint—can be
assigned to a variable. E.g. we can write

```C++
NsIntVar X = Y + 3 / Z;
NsIntVar W = NsSum(VarArrA) - (NsMin(VarArrB) == 10);
```

Note that instead of writing `VarArr[0] + VarArr[1] +
VarArr[2]`, it is more efficient to use the equivalent
expression `NsSum(VarArr, 0, 3)`. The second and third
argument of `NsSum` are respectively the position of the
first element of `VarArr` that will be included in the sum
and the number of the next elements that (together with the
first element) will be included in the sum. If neither of
these two arguments exist, then all the array is included in
the sum.

`NsSum` is the more efficient expression, because for the
other expression, _an intermediate variable will be created_
that will be equal to `VarArr[0] + VarArr[1]`; the variable
`VarArr[2]` will be afterwards added to the intermediate
variable. This intermediate variable will not be created if
we use `NsSum`.

`NsAbs` gives the absolute value. `NsMin` and `NsMax` give
respectively the minimum and the maximum of the array that
they accept as an argument.


### The Element constraint

A separate paragraph for the last two expressions
_IntArr_`[`_Expression_`]` and _VarArr_`[`_Expression_`]` is
dedicated, because they have to do with the special
_element_ constraint. The name “element” comes from logic
programming. For simplicity reasons, we suppose that the
_Expression_ is simply the constrained variable _VarIndex_,
that is used as an “index” in the array _IntArr_ or
_VarArr_. Note that _IntArr_ is an array containing
_integer_ values, because it is an `NsDeque<NsInt>`
instance. On the other hand the array _VarArr_ does contain
constrained variables, as it is an `NsIntVarArray` instance.

In order to understand the constraint usability, we will see
an example. Let the array `NsDeque<NsInt> grades` contains
eight students' grades. Also, let the domain of _VarIndex_
contain all the array positions, that is `[0..7]`. If we
want the domain of the constrained variable _VarValue_ to
contain every grade, we declare the constraint _VarValue_`
== grades[`_VarIndex_`]`.

In case we declare another constraint, e.g. _VarValue_` >=
9`, the domain of _VarIndex_ will be limited in order to
contain only the students' numbers whose grades are `9` or
`10`.

We saw that the expression _VarValue_` ==
`_IntArr_`[`_VarIndex_`]` declares an element constraint,
but the same constraint can also be declared with a logic
programming style as `NsElement(`_VarIndex_`, `_IntArr_`,
`_VarValue_`)`.


## Expressions for arrays

Finally there is a special independent expression category,
that can be assigned to arrays of constrained variables
(`NsIntVarArray`). It contains the following expressions for
the Inverse constraint (see [The Inverse
Constraint](#the-inverse-constraint) section).

 * `NsInverse(`_VarArr_`)`

 * `NsInverse(`_VarArr_`, `_maxdom_`)`

_maxdom_ is the size of the inverse array that will be
created. If this argument does not exist, it is taken that
_maxdom_ = max{_V_.max | _V_ ∈ _VarArr_}. In any case,
_maxdom_ should be greater or equal than this value. E.g.

```C++
NsIntVarArray VarArrB = NsInverse(VarArrA);
NsIntVarArray VarArrC;
VarArrC = NsInverse(VarArrA, 100);
```


### The Inverse constraint

The _Inverse_ constraint is applied between two arrays of
constrained variables. Let _Arr_ be an array that contains
variables with positive values in their domains. We want
_ArrInv_ to be the “inverse” array of _Arr_. Still, let
_D<sub>x</sub>_ be the domain of the constrained variable
_x_. Then it holds that:

∀ v ∈ _D_<sub>_ArrInv_[_i_]</sub>, _D_<sub>_Arr_[_v_]</sub>
∋ _i_.

If there is no _v_ such that _i_ ∈ _D_<sub>_Arr_[_v_]</sub>,
then the domain of _ArrInv_[_i_] will _only_ contain the
special value -1.

In a simpler notation, we can write that it holds:

_Arr_[_ArrInv_[_i_]] = _i_ and _ArrInv_[_Arr_[_i_]] = _i_.

That is why the constraint is called “Inverse.” Of course,
the above relations would have sense if the variables of the
two arrays were bound and if the unique value that each
variable had was designated with the variable name itself.
It should also apply that ∀ _i_, _ArrInv_[_i_] ≠ -1.


#### Usefulness of the constraint

This constraint can be used in dual modelings of a problem.
E.g. suppose that we have a number of tasks to assign to
some workers. One modelling could be to have a variable for
each task with the set of workers as domain. Another
modelling is to have a variable for each worker with the set
of tasks as domain. Obviously, there exist some constraints
in the problem. Some constraints may be declared more easily
using the first modelling, but there may be other
constraints that would be declared more easily and naturally
using the second modelling.

In this case, the solver gives the possibility to use both
modellings. However, the variables of the two modellings are
not irrelevant. We should declare something like _X_[_i_] =
_j_ ⇔ _Y_[_j_] = _i_. This is done using an Inverse
constraint.

---

 - [Next section](Examples.md)
