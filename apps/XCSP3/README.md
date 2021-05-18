# XCSP3 integration to Naxos

Here we link Naxos to the XCSP3 parser.

## Compilation

Use the following commands to compile, if G++ version 4.9
or later is installed.

```sh
git clone https://github.com/pothitos/naxos.git
cd naxos/apps/XCSP3/
cmake .
make naxos-xcsp3
```

:bulb: In some cases you may have to
`sudo apt-get install libxml2-dev`

## Execution

To solve a problem execute
```sh
./naxos-xcsp3 BENCHNAME
```
where `BENCHNAME` can be `tsp-25-843.xml` for example.


# XCSP3 mini-solver competition

We will also try to cover the following constraints for the
[XCSP3 mini-solver competition
tracks](http://xcsp.org/competition).

## Intensional constraints

We should cover the following statements, where

> _x_, _y_ and _z_ denote integer variables, _k_ denotes an
> integer value, ⊙ denotes a relational operator in {`<`,
> `<=`, `>=`, `>`, `=`, `!=`} and ⊕ denotes a binary
> arithmetic operator in {`+`, `-`, `*`, `/`, `%`, `||`},
> with `||` being the distance.

 + Variable and Constant
    - _x_ ⊙ _k_
    - _k_ ⊙ _x_
 + Variable and Variable
    - _x_ ⊙ _y_
 + Variable and Expression between Variable and Constant
    - (_x_ ⊕ _k_) ⊙ _y_
    - (_k_ ⊕ _x_) ⊙ _y_
    - _x_ ⊙ (_y_ ⊕ _k_)
    - _x_ ⊙ (_k_ ⊕ _y_)
 + Variable and Expression between Variable and Variable
    - (_x_ ⊕ _y_) ⊙ _z_
    - _x_ ⊙ (_y_ ⊕ _z_)

## Extensional constraints

These constraints are also called _table constraints_. Two
examples follow.

```xml
<extension id="c1">
  <list> x1 x2 x3 </list>
  <supports> (0, 1, 0) (1, 0, 0) (1, 1, 0) (1, 1, 1) </supports>
</extension>
```

```xml
<extension id="c2">
  <list> y1 y2 y3 y4 </list>
  <conflicts> (1, 2, 3, 4) (3, 1, 3, 4) </conflicts>
</extension>
```

The above means that the allowed combinations of values for
the variables `x1`, `x2`, and `x3` are included inside the
`supports` tag. On the other hand, the dissallowed
combinations of values for the variables `y1`, `y2`, `y3`,
and `y4` are included in the `conflicts` tag.

Special symbols such as `*` or `{1, 2}` are not accepted.

## AllDifferent

The classic AllDifferent constraint for arrays. The tag
`except` is not accepted.

## Sum

This constraint implies that the (weighted) sum of a set of
variables is limited by another variable.

```xml
<sum id="c1">
  <list> x1 x2 x3 </list>
  <coeffs> 1 2 3 </coeffs>
  <condition> (gt,y) </condition>
</sum>
```

The above example means that the weighted sum `1 * x1 + 2 *
x2 + 3 * x3` is limited by (`gt` greater than) `y`. The
symbol `y` could be also a constant.

Please note that in place of the constants `1`, `2`, and
`3`, there could be constrained variables, e.g. `z1`, `z2`,
and `z3`. Also, in place of `gt`, there could be the
operators `lt`, `le`, `ge`, `eq`, or `ne`. All of these
correspond to the ⊙ relational operator of the above
intensional constraints.

## Element

A simplified element constraint (without `startIndex` and `rank` attributes) is supported.

```xml
<element id="c1">
  <list> x1 x2 x3 x4 </list>
  <index> i </index>
  <value> v </value>
</element>
```

The above describes the relation `x[i] = v`.

## Objective in Constrained Optimization

> The type of the objective cannot be `product` or `lex`,
> and when it is `expression`, it can only be a variable
> (identifier).
