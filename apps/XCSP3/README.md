# XCSP3 Integration to Naxos

Here we link Naxos to the XCSP3 parser.


# XCSP3 Mini-Solver Competition

We will also try to cover the following constraints for the
[XCSP3 mini-solver competition
tracks](http://xcsp.org/competition).

## Intensional Constraints

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
    - (_x_ ⊕ _y_) ⊙ _y_
    - _x_ ⊙ (_y_ ⊕ _z_)

## Extensional Constraints

These constraints are also called _table constraints_. An
example follows.

```xml
<extension id="c1">
  <list>x1 x2 x3</list>
  <supports>(0, 1, 0) (1, 0, 0) (1, 1, 0) (1, 1, 1)</supports>
</extension>
<extension id="c2">
  <list>y1 y2 y3 y4</list>
  <conflicts>(1, 2, 3, 4) (3, 1, 3, 4)</conflicts>
</extension>
```

The above means that the allowed combinations of values for
the variables `x1`, `x2`, and `x3` is included inside the
`supports` tag. On the other hand, the dissallowed
combinations of values for the variables `y1`, `y2`, `y3`,
and `y4` is included in the `conflicts` tag.

## AllDifferent

The classic AllDifferent constraint for arrays.

## `sum`

## `element`
