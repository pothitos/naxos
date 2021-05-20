 - [Previous section](Errors.md)
 - [Contents](README.md)

---

# Constrained Variables

The solver supports _finite domain integer constrained
variables_. The class that implements them is called
`NsIntVar` and contains the following methods.


#### `NsIntVar(NsProblemManager& pm, NsInt min, NsInt max)`

A constructor function for a constraint variable. Argument
`pm` is the problem manager that the variable belongs to;
see the [Problem Manager](ProblemManager.md) section for
more. `min` and `max` are the bounds of its domain, that is
also designated `[min..max]`.

Data-type `NsInt` can at least represent the integers that
can be represented by data-type `long`. The minimum value
that an `NsInt` can hold, equals to the constant
`NsMINUS_INF`, and the maximum equals to `NsPLUS_INF`. (The
maximum value of the unsigned data-type `NsUInt` is
`NsUPLUS_INF`.)

The minimum of a domain must be strictly greater than
`NsMINUS_INF`, and the maximum value must be strictly less
than `NsPLUS_INF`, as those constants represent infinity, as
we will see below.


#### `NsIntVar()`

This constructor creates a variable that can be initialized
afterwords by assigning an expression to it using the
overloaded operator `=`, as on the third line of the
following example.

```C++
NsIntVar X(pm, 0, 3), Y(pm, -1, 15), Z;
NsIntVar W = X + 3 * Y;
Z = W * W;
```

On the second line of the example, we used another
constructor function, that takes an _Expression_ as
argument; here the expression is `X + 3 * Y`.


#### `void remove(NsInt val)`

Removes the value `val` from the domain of the variable.


#### `void remove(NsInt min, NsInt max)`

Removes every value of the domain that is in the range
[`min`,`max`]. Instead of removing those values one by one
using `remove(val)`, it is more efficient to call this
method.


#### `void removeAll()`

“Empties” the domain of the variable. Practically, the
solver only informs the problem manager that an
inconsistency occurred, due to an empty domain. This method
is useful when we want to make search fail. E.g. when we
want a goal to fail during its execution, we call this
method for any variable. In order to show that a goal
succeeds, we make `GOAL()` return `0`; but in order to show
that a goal failed, this is a—less elegant—way to do it. For
more information about the goals mechanism see the [Search
via Goals](Goals.md) section.


#### `void set(NsInt val)`

Assigns the value `val` to the variable; thus, the variable
becomes _bound_ (instantiated).

The following methods do not alter the variable for which
they are called.


#### `bool contains(NsInt val)`

Returns `true` if the domain of the variable contains the
value `val`.


#### `NsInt min()`

The minimum value of the domain.


#### `NsInt max()`

The maximum value of the domain.


#### `NsUInt size()`

The number of the values in the domain.


#### `bool isBound()`

Returns `true` if the variable is bound. “Bound” is synonym
to “instantiated,” “singleton,” and “fixed” and means that a
value has been assigned to the variable or, in other words,
that the domain contains only one value.


#### `NsInt value()`

It is used only when the variable is bound and returns its
(unique) value. If it is called for an unbound variable, an
exception is thrown.

If the variable is bound, this method is equivalent to
`min()` and `max()`, but we use this method for better code
readability.


#### `NsInt next(NsInt val)`

Returns the smallest value in the domain, that is strictly
greater than `val`. If such a value does not exist, the
function returns `NsPLUS_INF`. The argument `val` may be
equal to the special values `NsMINUS_INF` and `NsPLUS_INF`
too.


#### `NsInt previous(NsInt val)`

Returns the greatest value in the domain, that is strictly
less than `val`. If such a value does not exist, the
function returns `NsMINUS_INF`. The argument `val` may be
equal to the special values `NsMINUS_INF` and `NsPLUS_INF`
too.

Two iterators for the class accompanied by examples follow.


#### `NsIntVar::const_iterator`

Iterates through all the values of the domain of the
variable. E.g. the following code prints the values of the
variable `Var`, in ascending order.

```C++
for (NsIntVar::const_iterator v = Var.begin(); v != Var.end(); ++v)
    cout << *v << "\n";
```


#### `NsIntVar::const_reverse_iterator`

Iterates through the values of the domain of the variable in
reverse order. E.g. the following code prints the values of
the variable in descending order.

```C++
for (NsIntVar::const_reverse_iterator v=Var.rbegin(); v != Var.rend(); ++v)
    cout << *v << "\n";
```


#### `NsIntVar::const_gap_iterator`

Iterates through the gaps that exist inside the domain of
the variable. Mathematically speaking, it gives all the
values (inside [`min`,`max`]) of the complement of the
domain (where `min` is the minimum, and `max` is the maximum
of the domain). E.g.

```C++
for (NsIntVar::const_gap_iterator g = Var.gap_begin(); g != Var.gap_end(); ++g)
    cout << *g << "\n";
```

Finally, the operator `<<` has been overloaded in order to
print the variable to an output stream, by writing e.g.
`cout << Var;`.

---

 - [Next section](Arrays.md)
