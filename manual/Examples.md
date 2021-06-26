 - [Previous section](Expressions.md)
 - [Contents](README.md)

---

# Examples


## _N_ Queens Problem

A real problem will be declared as an example.


### Definition

In the _N_ queens problem we should place _N_ queens on an
_N_ x _N_ chessboard, so that no queen is attacked. In other
words we should place _N_ items on an _N_ x _N_ grid, in a
way that no two items share the same line, column or
diagonal. The following table displays an example for _N_ =
8, i.e. 8 queens that are not attacked.

|       | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|-------|---|---|---|---|---|---|---|---|
| __1__ |   |   |   |   |   | ♕ |   |   |
| __2__ |   |   | ♕ |   |   |   |   |   |
| __3__ |   |   |   |   | ♕ |   |   |   |
| __4__ |   |   |   |   |   |   | ♕ |   |
| __5__ | ♕ |   |   |   |   |   |   |   |
| __6__ |   |   |   | ♕ |   |   |   |   |
| __7__ |   | ♕ |   |   |   |   |   |   |
| __8__ |   |   |   |   |   |   |   | ♕ |

Thus, in each column 0, 1, …, _N_-1 we will have a queen. It
remains to find out the _line_ where each queen will be
placed. Therefore, we ask to assign values to the variables
_X<sub>i</sub>_ with 0 ≤ _X<sub>i</sub>_ ≤ _N_-1, where
_X<sub>i</sub>_ is the line on which the queen of column _i_
is placed.

Regarding the constraints, first of all, no two queens
should share the same line, i.e.

_X<sub>i</sub>_ ≠ _X<sub>j</sub>_, ∀ _i_ ≠ _j_. (1)

They should not also share the same diagonal, consequently

_X<sub>i</sub>_ + i ≠ _X<sub>j</sub>_ + _j_ and
_X<sub>i</sub>_ - _i_ ≠ _X<sub>j</sub>_ - _j_, ∀ _i_ ≠ _j_.
(2)

_X<sub>i</sub>_ - _i_ corresponds to the primary diagonal
and _X<sub>i</sub>_ + _i_ to the secondary diagonal for the
queen of column _i_.


### Code

In the solver code, the variables _X<sub>i</sub>_ are
represented by the array `Var`, that according to (1) should
have different elements. Concerning (2), we create two other
arrays, `VarPlus` and `VarMinus`, with the elements
_X<sub>i</sub>_ + _i_ and _X<sub>i</sub>_ - _i_
respectively. For these arrays we will also declare that
their elements shall differ.

```C++
int N = 8;
NsProblemManager pm;

NsIntVarArray Var, VarPlus, VarMinus;
for (int i = 0; i < N; ++i) {
    Var.push_back(NsIntVar(pm, 0, N-1));
    VarPlus.push_back(Var[i] + i);
    VarMinus.push_back(Var[i] - i);
}
pm.add(NsAllDiff(Var));
pm.add(NsAllDiff(VarPlus));
pm.add(NsAllDiff(VarMinus));

pm.addGoal(new NsgLabeling(Var));
while (pm.nextSolution() != false)
    cout << "Solution: " << Var << "\n";
```


## _SEND_ + _MORE_ = _MONEY_

Another example is a known _cryptarithm_ problem. In those
problems we have some arithmetic relations between words,
such as _SEND_ + _MORE_ = _MONEY_. Each letter of the words
represents a specific digit (from 0 to 9); thus, each word
represents a decimal number. Two different letters should
not represent the same digit. E.g. for the equation _SEND_ +
_MORE_ = _MONEY_, we will put the same digit in the
positions where `E` appears. The same applies for the rest
of the letters, that should however be assigned different
digits than the one for `E`. After all the assignments the
relation of the cryptarithm should be valid. This is the
problem declaration for the solver:

```C++
NsProblemManager pm;

NsIntVar S(pm,1,9), E(pm,0,9), N(pm,0,9), D(pm,0,9),
         M(pm,1,9), O(pm,0,9), R(pm,0,9), Y(pm,0,9);

NsIntVar send  =           1000*S + 100*E + 10*N + D;
NsIntVar more  =           1000*M + 100*O + 10*R + E;
NsIntVar money = 10000*M + 1000*O + 100*N + 10*E + Y;

pm.add(send + more == money);

NsIntVarArray letters;
letters.push_back(S);
letters.push_back(E);
letters.push_back(N);
letters.push_back(D);
letters.push_back(M);
letters.push_back(O);
letters.push_back(R);
letters.push_back(Y);
pm.add(NsAllDiff(letters));

pm.addGoal(new NsgLabeling(letters));
if (pm.nextSolution() != false) {
    cout << "    " << send.value() << "\n"
         << " +  " << more.value() << "\n"
         << " = " << money.value() << "\n";
}
```

If we execute the code we take the result

```
    9567
 +  1085
 = 10652
```


## How do we state and solve a problem?

In the previous sections we stated some problems-examples;
but what are the steps in order to state and solve another
problem?

In the [Error Handling](Errors.md) section, we saw the
source code basis to solve a problem. Under the comment
`CODE OF THE PROGRAM` we insert the main part of our code.

Our code is summarized into the following triptych.

 1. Constrained variables (`NsIntVar`) declaration, together
    with their domains.
 2. Constraints statement (`pm.add(·)`).
 3. Goals declaration (`pm.addGoal(new NsgLabeling(·))`) and
    search for solutions (`pm.nextSolution()`).

The first thing to do is to create a problem manager (`pm`),
to store the whole constraint network. The declaration is

```C++
NsProblemManager pm;
```

Next, we declare the constrained variables of the problem.
Remember that while a simple variable (e.g. `int x`) stores
only one value (e.g. `x = 5`), a _constrained_ variable
stores a _range_ or, better, a domain. E.g. with the
declaration `NsIntVar V(pm,0,5)`, the domain of `V` is the
integer values range `[0..5]`.

When there are many constrained variables, then we use
constrained variables arrays `NsIntVarArray`, as in the [_N_
Queens problem](#n-queens-problem) for example. E.g.

```C++
NsIntVarArray R;
```

The array `R` is initially empty. It is not possible to
define a priori neither the array size, nor the included
constrained variables domains. We can do this through an
iteration

```C++
for (i = 0; i < N; ++i)
    R.push_back(NsIntVar(pm,min,max));
```

…in the way we insert items into a list. In place of `min`
and `max` we put the minimum and maximum domain value,
respectively. Next, we declare the existing constraints
through `pm.add(·)` calls…

Before the end, if we solve an _optimization_ problem, it
remains to declare the parameter to optimize. When we find
out this parameter-variable, we will pass it as an argument
of `pm.minimize(·)`. This method is unnecessary when we seek
for _any_ solution of the problem.

We can now add a goal to be satisfied through the statement:

```C++
pm.addGoal(new NsgLabeling(R));
```

This goal instructs the solver to assign values to the
constrained variables of the array `R`. If we do not state
this goal, the solver will not instantiate the variables
`R[i]`, but it will only check the satisfaction of the
constraints between _ranges_, and the variables will not
become fixed.

Finally, we execute `pm.nextSolution()` to find a solution.
This function is called inside a loop; every time it returns
`true`, we have another unique problem solution.

**Note:** After the `pm.nextSolution()` call, we refer to a
constraint variable, e.g. `NsIntVar X`, by its const method
`NsIntVar::value()`. For example, it is wrong to write
`cout << X + 1;` the correct is `cout << X.value() + 1;`.

_If we have previously called `pm.minimize(·)`_, the solver
guarantees that each new solution will be better from the
previous one. In case `pm.nextSolution()` returns `false`,
then either the solution cost cannot be further improved, or
there is not any other solution. Thus we should have stored
somewhere the last solution (and perhaps its cost too), in
order to print it in the end, as in the following code for
example:

```C++
NsDeque<NsInt> bestR(N);

while (pm.nextSolution() != false) {
    // Record the (current) best solution.
    for (i = 0; i < N; ++i)
        bestR[i] = R[i].value();
}

// Print the best solution...
```

Note that when `nextSolution` seeks a solution, the
constrained variables should not be destructed. Hence, it
makes no sense to define a local constrained variable in a
function and call `nextSolution` in another function.
Finally, a constrained variable definition should be
straight; we cannot write something like the following,
because in every iteration, the variable `vSum` is actually
redefined:

```C++
NsIntVar vSum;
for (i = 0; i < N; ++i)
        vSum += R[i];  // WRONG!
```

The above can be simply stated as `NsIntVar vSum = NsSum(R);`

---

 - [Next section](Goals.md)
