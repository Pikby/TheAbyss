Current chunk meshing can be found [here](../src/MainEngine/bspmeshing.cpp)

The Meshing algorithm uses a [Cube Marching Algorithm](http://paulbourke.net/geometry/polygonise/) to mesh the chunks. Every cube is divided into a set of 3x3x3 (27) vertices which then are fed through the cube marching algorithm to allow for slants, and curves between blocks

Each block has an 8 bit data describing which of the 8 corners of the cube are exist, and the other 21 vertices are determined by testing for a straight line between the vertex and its neighbors.

The corners of the cube are encoded such as the first bit is the corner at (0,0,0),second at:(0,1,0), and so on

The overall scheme is (0,0,0),(0,1,0),(1,0,0),(1,1,0),(0,0,1),(0,1,1),(1,0,1),(1,1,1)
