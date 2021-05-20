 - [Previous section](Variables.md)
 - [Contents](README.md)

---

# Constrained Variable Arrays

The default (flexible) array class, that contains
constrained variables, is `NsIntVarArray`. The `i`-th
variable of the array `VarArr` is accessed as usually, via
`VarArr[i]`. The default data-type for `i` is `NsIndex`.

An `NsIntVarArray` is initially empty. We can insert a
constrained variable to it, either in front of it or at its
back, in the way that we insert elements into a linked list.
Its constructor function does not have any arguments. The
description for the rest of the class functions follow.


#### `NsIndex size()`

Returns the size of the array.


#### `bool empty()`

Returns `true` if the array is empty.


#### `NsIntVar& back()`

The last constrained variable of the array.


#### `NsIntVar& front()`

The first constrained variable of the array.


#### `void push_back(`_Expression_`)`

Inserts at the end of the array the variable that is
described by the _Expression_. In the following section it
is explained that an _Expression_ can be simply a
constrained variable or a combination of variables. E.g.

```C++
VarArr.push_back(NsIntVar(pm, 10, 30));
VarArr.push_back(3 * VarX + VarY);
VarArr.push_back(VarX > 11);
```

The first statement above inserts at the end of `VarArr` a
new constrained variable with domain `[10..30]`.

The last statement inserts a constrained variable at the end
of the array. This variable will equal `1` if it holds that
`VarX > 11`, otherwise it will equal `0`. (It is possible
that its domain will be `[0..1]`, if `VarX` contains some
values less than `11` and some other values greater than
`11`.) Therefore, we have a _meta-constraint_.


#### `void push_front(`_Expression_`)`

Like `push_back()` but the insertion takes place at the
beginning of the array.

Iterators for arrays follow. We can use them in order to
iterate easily through the variables of an array.


#### `NsIntVarArray::iterator`

With this iterator we can access all the variables of the
array in turn. E.g. the following code removes the value `2`
from every variable of `VarArr`.

```C++
for (NsIntVarArray::iterator V = VarArr.begin(); V != VarArr.end(); ++V)
    V->remove(2);
```


#### `NsIntVarArray::const_iterator`

This is an iterator like the above one, but it is useful
only when we do not modify the constrained variables. E.g.
we can use it in order to print the variables of an array.

```C++
for (NsIntVarArray::const_iterator V=VarArr.begin(); V != VarArr.end(); ++V)
    cout << *V << "\n";
```

Finally, the operator `<<` has been overloaded for the
arrays too. We can type an array by writing for example
`cout << VarArr;`.


## Generic arrays

In case we want to create an array with the philosophy and
methods of `NsIntVarArray`, we can use the template class
`NsDeque<`*data_type*`>`. E.g. with

```C++
NsDeque<int> arr;
```

we declare that `arr` is a flexible array of integers,
initially empty. With

```C++
NsDeque<int> arr(5);
```

we declare that it initially contains `5` elements. The
*data_type* does not have any sense to be `NsIntVar`,
because in this case we can use directly `NsIntVarArray`,
instead of `NsDeque`. `NsDeque` is actually an extension of
`std::deque`, which is included in C++ standard library.
The difference in `NsDeque` is that it always checks that we
are inside the array bounds; if we exceed them, the
corresponding exception is thrown.


## How to implement a grid of constrained variables?

In general, there's a flexibility using such kinds of data
structures. In any case, an easy implementation follows when
you know a priori how many `lines` the grid will contain.

```C++
NsDeque<NsIntVarArray> grid(lines);
```

You will be then able to add constrained variables to the
array of line `l` by using the `push_back` method. For
example, the following alternative cases are possible.

```C++
grid[l].push_back(NsIntVar(pm, 0, 1));
```

```C++
NsIntVar X(pm, -2, 10);
grid[l].push_back(X);
```

```C++
grid[l].push_back(another_grid[3][4]);
```

---

 - [Next section](ProblemManager.md)
