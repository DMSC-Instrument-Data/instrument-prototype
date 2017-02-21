##Prototype Summary##

This prototype attempts to do the following
* Eliminate the need for a `ParameterMap` for write operations to an instrument
* Provide read-operations that do not need to walk the `Component` hierachy
* Make it cheap and easy to copy what would traditionally be classed as an `Instrument`  
* Demonstrate complex/neutronic beam paths
* Demonstrate step scanning of detectors
* Demonstrate serialization and de-serialization

##Design##

The design is centered around a structure of arrays approach to improve cache locality. As far as possible information can be read directly without the need for lazy calculation. The design uses various "Info" layers which can be indexed and enumerated over quickly to provide different views on the beamline. For example a `DetectorInfo` provides a simple detector centric view of the beamline. The instrument tree is represented in a flat vector format, which eliminates the need for pointer dereferncing and complex/expensive runtime hierachies.

###Modifications###

Writes are more expensive since absolute values are taken for positions and rotations, and offsets need to be applied to sub-levels of the tree. Since writes are an infrequent operation, this is the correct comprimise to make. Vectors homed in copy on write pointers are used to give high degrees of sharing across copies.





