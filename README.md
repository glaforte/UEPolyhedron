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
* `C` [Cube](https://en.wikipedia.org/wiki/Cube)
* `D` [Dodecahedron](https://en.wikipedia.org/wiki/Dodecahedron)
* `I` [Icosahedron](https://en.wikipedia.org/wiki/Icosahedron)
* `O` [Octahedron](https://en.wikipedia.org/wiki/Octahedron)
* `P<n>` [Prism](https://en.wikipedia.org/wiki/Prism_(geometry)) where `<n>` is [3,9]. For example: `P3` makes a [Triangular Prism](https://en.wikipedia.org/wiki/Triangular_prism) and `P5` makes a [Pentagonal Prism](https://en.wikipedia.org/wiki/Pentagonal_prism).
* `T` [Tetrahedron](https://en.wikipedia.org/wiki/Tetrahedron)

The following operations are supported:
* `a` Ambo
* `b` Bevel
* `c` Chamfer
* `d` Dual
* `e` Expand
* `g` Gyro
* `j` Join
* `k` Kis or Triakis
* `m` Meta
* `o` Ortho
* `s` Snub
* `t` Truncate

I can add more starter polyhedra and operations on request.

#### Examples

 | **Notation** |  `D` or `dI` | `tI` | `tktI` | `eC` |
 |----|----|----|----|----|
 | | ![Regular Dodecahedron](./Resources/DualIcosahedron.png) | ![Truncated Icosahedron](./Resources/TruncatedIcosahedron.png) | ![TKT Icosahedron](./Resources/TKTIcosahedron.png) | ![Expanded Cube](./Resources/ExpandedCube.png) | 
 | | [Regular Dodecahedron](https://en.wikipedia.org/wiki/Dodecahedron) |  Soccer ball / [Truncated Icosahedron](https://en.wikipedia.org/wiki/Truncated_icosahedron) | Golf ball | Expanded Cube | 

### Goldberg Polyhedron
For more details on Goldberg Polyhedron and its notation: https://en.wikipedia.org/wiki/Goldberg_polyhedron.
* `dI` is GP(1,0).
* `tI` is GP(1,1).
* `tktI` is GP(3,3).
* `tktktI` is GP(6,6).

### Inspiration
Based on the [Polyhédronisme](https://levskaya.github.io/polyhedronisme/) project.
Copyright 2019, Anselm Levskaya.
The PolyhedronConway actor notation is intended to match Polyhédronisme's notation. See the [Polyhédronisme User Interface](https://github.com/levskaya/polyhedronisme) for more details.

Based on earlier work from [George W. Hart](http://www.georgehart.com/).
