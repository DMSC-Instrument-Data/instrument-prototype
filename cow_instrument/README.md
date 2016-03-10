##Prototype Summary##

This prototype attempts to do the following
* Eliminate the need for a `ParameterMap` for write operations to an instrument
* Provide read-operations that do not need to walk the `Component` hierachy
* Make it cheap and easy to copy an `Instrument` 

##Design##

This prototype introduces a `Node` that provides immutable access only to a `Component`. `Nodes` are linked together to form the tree that we think of as the `Instrument`.  `Nodes` are very small, and cheap to manufacture. Any modification to an `Instrument` results in a new `Instrument` (completely new set of `Nodes`), but crutially `Components`, which are held by nodes are copy-on-write. The end result is that new `Instruments` should be cheap to manufacture, because different `Nodes` will largely share the same acutal `Component` representations. 

###Modifications###

All `Components` are effectively immutable behind `Nodes`. `Nodes` take `Commands` to allow the underlying `Component` representations to be modified. When a modification is made, the entire node tree is always shallow copied (see definition below) first. The `Command` is then executed on the relevant `Component`s of the new copy. 

Unlike the v1 Mantid Instrument, we make reads cheap at the expense of the writes/modifications. We store absolute values at the `Component` level, and that means that any modifications need to cascade down the tree. For this reason all `Commands` are both executed on the target `Component` and all child `Node`s too.

After each modification a pointer to the root `Node` of the new tree is returned. The root `Node` is entirely equivalent to the `Instrument`

###Shallow Copy###

When `Nodes` are copied, the COW pointer to the `Component` is copied, but a new `Component` is not generated. Only when mutable access to a `Component` is made is the `Component` copied too. 
