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

## `allDifferent`

## `sum`

## `element`
