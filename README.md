# UEPolyhedron
Polyhedron Procedural Meshes in Unreal Engine

Using this UE5 plug-in, you can quickly have procedurally-built Polyhedron meshes in your UE project.
The meshes include UV Mapping and Tangents for normal-maps.

### User Interface
The main user interface for the plug-in is the APolyhedronConway Actor.
It has three properties:
* ConwayPolyhedronNotation determines the shape of the polyhedron. See below for more details.
* Scale is used to rescale the polyhedron to a sphere with a radius = Scale.
* EnableCollision enables the collision and physics geometry on the primitive component.

### Conway Notation
In the PolyhedronConway actor, you will need to write a notation string that includes a starter polyhedron and a sequence of [Conway Polyhedron Notation](https://en.wikipedia.org/wiki/Conway_polyhedron_notation) operations.
This string is parsed from back to front.

The following starter polyhedra are supported:
* `I` [Icosahedron](https://en.wikipedia.org/wiki/Icosahedron)

The following operations are supported:
* `d` Dual
* `k` Kis or Triakis
* `t` Truncate

I can add more starter polyhedra and operations on request.

### Example Conway Notations
* `dI` makes a [Regular Dodecahedron](https://en.wikipedia.org/wiki/Dodecahedron)
* `tI` makes a Soccer ball / [Truncated Icosahedron](https://en.wikipedia.org/wiki/Truncated_icosahedron)
* `tktI` makes a Golf ball.

### Example Goldberg Polyhedron
For more details on Goldberg Polyhedron and its notation: https://en.wikipedia.org/wiki/Goldberg_polyhedron.
* `dI` is GP(1,0).
* `tI` is GP(1,1).
* `tktI` is GP(3,3).
* `tktktI` is GP(6,6).

### Inspiration
Based on the Polyhédronisme project.  Copyright 2019, Anselm Levskaya.
* https://levskaya.github.io/polyhedronisme/
* https://github.com/levskaya/polyhedronisme

Based on earlier work from George W. Hart.
* http://www.georgehart.com/
