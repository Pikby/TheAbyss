

#include <list>
#include <memory>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>

#include "../Objects/include/items.h"
#include "include/bsp.h"



struct VERTEX
{
glm::dvec3 pos;
glm::vec3 norm;
uint8_t id;

bool operator==(VERTEX p)
{
  return (this->pos == p.pos) && (this->id == p.id);
}
};


typedef struct
{
VERTEX p[3];
} TRIANGLE;



typedef struct
{
double val[8];
uint8_t id[8];
glm::vec3 norm[8];
} GRIDCELL;

/*
Code taken from http://paulbourke.net/geometry/polygonise/ adapted to use glm vectors


*/



/*
Given a grid cell and an isolevel, calculate the triangular
facets required to represent the isosurface through the cell.
Return the number of triangular facets, the array "triangles"
will be loaded up with the vertices at most 5 triangular facets.
0 will be returned if the grid cell is either totally above
of totally below the isolevel.
*/



static const int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
static const int triTable[256][16] =
{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};


int Polygonise(const GRIDCELL& grid, const glm::dvec3* p,const double isolevel,TRIANGLE *triangles)
{
  /*
  Determine the index into the edge table which
  tells us which vertices are inside of the surface
  */
  int cubeindex = 0;
  if (grid.val[0] < isolevel) cubeindex |= 1;
  if (grid.val[1] < isolevel) cubeindex |= 2;
  if (grid.val[2] < isolevel) cubeindex |= 4;
  if (grid.val[3] < isolevel) cubeindex |= 8;
  if (grid.val[4] < isolevel) cubeindex |= 16;
  if (grid.val[5] < isolevel) cubeindex |= 32;
  if (grid.val[6] < isolevel) cubeindex |= 64;
  if (grid.val[7] < isolevel) cubeindex |= 128;

  /* Cube is entirely in/out of the surface */
  if (edgeTable[cubeindex] == 0)
  return(0);

  /*
  Linearly interpolate the position where an isosurface cuts
  an edge between two vertices, each with their own scalar value
  */

  auto VertexInterp = [&](double isolevel,int id1,int id2)
  {

    glm::dvec3 p1 = p[id1];
    glm::dvec3 p2 = p[id2];



    double valp1 = grid.val[id1];
    double valp2 = grid.val[id2];

    VERTEX ret;

    if (abs(isolevel-valp1) < 0.00001)
    {
      ret.pos = p1;
      ret.id = grid.id[id1];
      ret.norm = grid.norm[id1];
      return ret;
    }
    if (abs(isolevel-valp2) < 0.00001)
    {
      ret.pos = p2;
      ret.id = grid.id[id2];
      ret.norm = grid.norm[id2];
      return ret;
    }
    if (abs(valp1-valp2) < 0.00001)
    {
      ret.pos = p1;
      ret.id = grid.id[id1];
      ret.norm = grid.norm[id1];
      return ret;
    }


    double mu = (isolevel - valp1) / (valp2 - valp1);
    ret.pos = p1 + mu*(p2-p1);
    ret.norm = glm::normalize(grid.norm[id1] + float(mu)*(grid.norm[id2]-grid.norm[id1]));
    ret.id = valp1 > valp2 ? grid.id[id1] : grid.id[id2];
    return(ret);
  };




  VERTEX vertlist[12];
  /* Find the vertices where the surface intersects the cube */
  if (edgeTable[cubeindex] & 1)
  vertlist[0] =
  VertexInterp(isolevel,0,1);
  if (edgeTable[cubeindex] & 2)
  vertlist[1] =
  VertexInterp(isolevel,1,2);
  if (edgeTable[cubeindex] & 4)
  vertlist[2] =
  VertexInterp(isolevel,2,3);
  if (edgeTable[cubeindex] & 8)
  vertlist[3] =
  VertexInterp(isolevel,3,0);
  if (edgeTable[cubeindex] & 16)
  vertlist[4] =
  VertexInterp(isolevel,4,5);
  if (edgeTable[cubeindex] & 32)
  vertlist[5] =
  VertexInterp(isolevel,5,6);
  if (edgeTable[cubeindex] & 64)
  vertlist[6] =
  VertexInterp(isolevel,6,7);
  if (edgeTable[cubeindex] & 128)
  vertlist[7] =
  VertexInterp(isolevel,7,4);
  if (edgeTable[cubeindex] & 256)
  vertlist[8] =
  VertexInterp(isolevel,0,4);
  if (edgeTable[cubeindex] & 512)
  vertlist[9] =
  VertexInterp(isolevel,1,5);
  if (edgeTable[cubeindex] & 1024)
  vertlist[10] =
  VertexInterp(isolevel,2,6);
  if (edgeTable[cubeindex] & 2048)
  vertlist[11] =
  VertexInterp(isolevel,3,7);

  /* Create the triangle */
  int ntriang = 0;
  for (int i=0;triTable[cubeindex][i]!=-1;i+=3) {
    triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
    triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
    triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
    ntriang++;
  }

  return(ntriang);
}





AmbientOcclusion BSP::getAO(const glm::ivec3 &pos, Faces face, TextureSides top, TextureSides right)
{
  using namespace glm;
  ivec3 side1, side2;
  if(top == TOPTS)
  {
    switch(face)
    {
      case(TOPF):    side1 = ivec3(0,1,1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,1); break;
      case(FRONTF):  side1 = ivec3(0,1,-1); break;
      case(BACKF):   side1 = ivec3(0,1,1); break;
      case(LEFTF):   side1 = ivec3(-1,1,0); break;
      case(RIGHTF):  side1 = ivec3(1,1,0); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF):    side1 = ivec3(0,1,-1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,-1); break;
      case(FRONTF):  side1 = ivec3(0,-1,-1); break;
      case(BACKF):   side1 = ivec3(0,-1,1); break;
      case(LEFTF):   side1 = ivec3(-1,-1,0); break;
      case(RIGHTF):  side1 = ivec3(1,-1,0); break;
    }
  }

  if(right == RIGHTTS)
  {
    switch(face)
    {
      case(TOPF):    side2 = ivec3(1,1,0); break;
      case(BOTTOMF): side2 = ivec3(1,-1,0); break;
      case(FRONTF):  side2 = ivec3(1,0,-1); break;
      case(BACKF):   side2 = ivec3(1,0,1); break;
      case(LEFTF):   side2 = ivec3(-1,0,1); break;
      case(RIGHTF):  side2 = ivec3(1,0,1); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF):    side2 = ivec3(-1,1,0); break;
      case(BOTTOMF): side2 = ivec3(-1,-1,0); break;
      case(FRONTF):  side2 = ivec3(-1,0,-1); break;
      case(BACKF):   side2 = ivec3(-1,0,1); break;
      case(LEFTF):   side2 = ivec3(-1,0,-1); break;
      case(RIGHTF):  side2 = ivec3(1,0,-1); break;
    }
  }

  glm::ivec3 norm;
  switch(face)
  {
    case (FRONTF):  norm = ivec3(0,0,-1); break;
    case (BACKF):   norm = ivec3(0,0,1); break;
    case (TOPF):    norm = ivec3(0,1,0); break;
    case (BOTTOMF): norm = ivec3(0,-1,0); break;
    case (RIGHTF):  norm = ivec3(1,0,0); break;
    case (LEFTF):   norm = ivec3(-1,0,0); break;
  }

  ivec3 side1Pos = pos + side1;
  ivec3 side2Pos = pos + side2;


  Block b1 = ItemDatabase::blockDictionary[parent->getBlockOOB(side1Pos)];
  Block b2 = ItemDatabase::blockDictionary[parent->getBlockOOB(side2Pos)];
  bool side1Opacity = (OPAQUE == b1.visibleType);
  bool side2Opacity = (OPAQUE == b2.visibleType);

  if(side1Opacity && side2Opacity)
  {
    return FULLCOVER;
  }
  ivec3 corner = side1+side2-norm;
  ivec3 cornerPos = pos + corner;

  Block cornerBlock = ItemDatabase::blockDictionary[parent->getBlockOOB(cornerPos)];
  bool cornerOpacity = (OPAQUE==cornerBlock.visibleType);
  return static_cast<AmbientOcclusion> (side1Opacity+side2Opacity+cornerOpacity);
}


class Vertex
{
public:

  bool exists = false;
  std::pair<uint8_t,int8_t> ids[8] = {std::pair<uint8_t,uint8_t>(-1,0)};
  int idCount = 0;
  std::vector<glm::vec3> norms;
  void addNorm(const glm::vec3 &norm)
  {
    norms.push_back(norm);
  }
  glm::vec3 getNorm()
  {
    if(norms.empty()) return glm::vec3(0);
    glm::vec3 totalNorm = glm::vec3(0);
    for(auto itr = norms.begin(); itr != norms.end();itr++)
    {
      totalNorm += *itr;
    }
    return glm::normalize(totalNorm);
  }

  void addId(uint8_t id)
  {
    for(int i=0;i<idCount;i++)
    {
      if(ids[i].first == id)
      {
        ids[i].second++;
        return;
      }
    }
    ids[idCount].first = id;
    ids[idCount].second = 1;
    idCount++;
  }

  uint8_t getId()
  {
    int maxCount = -1;
    int maxId = 3;
    for(int i=0;i<idCount;i++)
    {
      if(maxCount < ids[i].second)
      {
        maxCount = ids[i].second;
        maxId = ids[i].first;
      }
      else if(maxCount == ids[i].second)
      {
        if(ids[i].first > maxId)
        {
          maxId = ids[i].first;
        }
      }
    }
    return maxId;
  }
};

template<int T>
class VertexArray
{
private:
  Array3D<Vertex,T> array;
  Array3D<int,CHUNKSIZE> renderArr;
  void setRender(const glm::vec3& rPos)
  {
    if(!(rPos.x < 0 || rPos.y < 0 || rPos.z < 0 || rPos.x > CHUNKSIZE -1 || rPos.y > CHUNKSIZE -1 || rPos.z > CHUNKSIZE -1 ))
    {
      int& val = renderArr.get(rPos);
      if(val == -1) return;
      val = 1;
    }
  }
  void setNeverRender(const glm::vec3& rPos)
  {
    if(!(rPos.x < 0 || rPos.y < 0 || rPos.z < 0 || rPos.x > CHUNKSIZE -1 || rPos.y > CHUNKSIZE -1 || rPos.z > CHUNKSIZE -1 ))
    {
      renderArr.get(rPos) = -1;
    }
  }

public:
  void setVertexValue(uint8_t blockId,const glm::ivec3& pos, const glm::vec3& norm)
  {
    if(pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x > T-1 || pos.y > T-1|| pos.z > T-1) return;

    Vertex& v = array.get(pos);
    v.exists = true;
    v.addId(blockId);
    v.addNorm(norm);

  }
  void setRenderFlags(const glm::ivec3& pos)
  {
    setNeverRender(pos);

    setRender(pos+glm::ivec3(-1,-1,-1));
    setRender(pos+glm::ivec3(0,-1,-1));
    setRender(pos+glm::ivec3(1,-1,-1));
    setRender(pos+glm::ivec3(-1,0,-1));
    setRender(pos+glm::ivec3(0,0,-1));
    setRender(pos+glm::ivec3(1,0,-1));
    setRender(pos+glm::ivec3(-1,1,-1));
    setRender(pos+glm::ivec3(0,1,-1));
    setRender(pos+glm::ivec3(1,1,-1));

    setRender(pos+glm::ivec3(-1,-1,0));
    setRender(pos+glm::ivec3(0,-1,0));
    setRender(pos+glm::ivec3(1,-1,0));
    setRender(pos+glm::ivec3(-1,0,0));
    //setRender(pos+glm::ivec3(0,0,0));
    setRender(pos+glm::ivec3(1,0,0));
    setRender(pos+glm::ivec3(-1,1,0));
    setRender(pos+glm::ivec3(0,1,0));
    setRender(pos+glm::ivec3(1,1,0));

    setRender(pos+glm::ivec3(-1,-1,1));
    setRender(pos+glm::ivec3(0,-1,1));
    setRender(pos+glm::ivec3(1,-1,1));
    setRender(pos+glm::ivec3(-1,0,1));
    setRender(pos+glm::ivec3(0,0,1));
    setRender(pos+glm::ivec3(1,0,1));
    setRender(pos+glm::ivec3(-1,1,1));
    setRender(pos+glm::ivec3(0,1,1));
    setRender(pos+glm::ivec3(1,1,1));
  }


  bool shouldRender(const glm::ivec3& pos)
  {
    return renderArr.get(pos) == 1;
  }

  void setCell(GRIDCELL* cell,int index,int x,int y, int z)
  {
    Vertex& v = array.get(x,y,z);
    cell->id[index] = v.getId();
    cell->norm[index] = v.getNorm();

    cell->val[index] = v.exists * 100;
    /*
    if(v.exists)
    {
      if(cell->id[index] == 0)
      {
        cell->val[index] = 100;

      }
      else if(cell->id[index] == 1)
      {
        cell->val[index] = 101;
      }
      else cell->val[index] = 101;

    }else cell->val[index] = 0;
  }
  */
  }
};


void BSP::build()
{





  //Delete and reserve space for the vectors;
  oVertices.clear();
  oIndices.clear();
  tVertices.clear();
  tIndices.clear();

  lightList.clear();
  //oVertices.reserve(200000);
//  oIndices.reserve(100000);


  const int lod = 2;
  const int arrSize = CHUNKSIZE*lod+2;

  VertexArray<arrSize> vArray;

  using namespace std::chrono;
  auto t1 = high_resolution_clock::now();
  for(int x = -1; x<CHUNKSIZE+1;x++)
  {
    for(int z = -1; z<CHUNKSIZE+1;z++)
    {
      for(int y = -1; y<CHUNKSIZE+1;y++)
      {
        int rx = x*lod;
        int ry = y*lod;
        int rz = z*lod;

        Block curBlock = ItemDatabase::blockDictionary[parent->getBlockOOB(glm::ivec3(x,y,z))];
        if(curBlock.visibleType == OPAQUE)
        {
          glm::ivec3 pos = glm::ivec3(x,y,z);
          if(parent->getBlockOOB(pos + glm::ivec3(0,1,0)) > 0 && parent->getBlockOOB(pos + glm::ivec3(0,-1,0)) > 0 &&
             parent->getBlockOOB(pos + glm::ivec3(1,0,0)) > 0 && parent->getBlockOOB(pos + glm::ivec3(-1,0,0)) > 0 &&
             parent->getBlockOOB(pos + glm::ivec3(0,0,1)) > 0 && parent->getBlockOOB(pos + glm::ivec3(0,0,-1)) > 0)
          {
            continue;
          }

          uint8_t blockId = curBlock.getTop();
          glm::vec3 norm = glm::vec3(0,1,0);
          vArray.setRenderFlags(glm::vec3(x,y,z));

          {
            const int zn = 0;

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+0,rz+zn),glm::normalize(glm::vec3(-1,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+0,rz+zn),glm::normalize(glm::vec3(0,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+0,rz+zn),glm::normalize(glm::vec3(1,-1,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+1,rz+zn),glm::normalize(glm::vec3(-1,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+1,rz+zn),glm::normalize(glm::vec3(0,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+1,rz+zn),glm::normalize(glm::vec3(1,0,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+2,rz+zn),glm::normalize(glm::vec3(-1,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+2,rz+zn),glm::normalize(glm::vec3(0,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+2,rz+zn),glm::normalize(glm::vec3(1,1,zn-1)));
          }
          {
            const int zn = 1;

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+0,rz+zn),glm::normalize(glm::vec3(-1,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+0,rz+zn),glm::normalize(glm::vec3(0,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+0,rz+zn),glm::normalize(glm::vec3(1,-1,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+1,rz+zn),glm::normalize(glm::vec3(-1,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+1,rz+zn),glm::normalize(glm::vec3(0,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+1,rz+zn),glm::normalize(glm::vec3(1,0,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+2,rz+zn),glm::normalize(glm::vec3(-1,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+2,rz+zn),glm::normalize(glm::vec3(0,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+2,rz+zn),glm::normalize(glm::vec3(1,1,zn-1)));
          }
          {
            const int zn = 2;

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+0,rz+zn),glm::normalize(glm::vec3(-1,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+0,rz+zn),glm::normalize(glm::vec3(0,-1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+0,rz+zn),glm::normalize(glm::vec3(1,-1,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+1,rz+zn),glm::normalize(glm::vec3(-1,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+1,rz+zn),glm::normalize(glm::vec3(0,0,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+1,rz+zn),glm::normalize(glm::vec3(1,0,zn-1)));

            vArray.setVertexValue(blockId,glm::vec3(rx+0,ry+2,rz+zn),glm::normalize(glm::vec3(-1,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+1,ry+2,rz+zn),glm::normalize(glm::vec3(0,1,zn-1)));
            vArray.setVertexValue(blockId,glm::vec3(rx+2,ry+2,rz+zn),glm::normalize(glm::vec3(1,1,zn-1)));
          }
        }

          /*
          for(int xn=0;xn<3;xn++) for(int yn=0;yn<3;yn++) for(int zn=0;zn<3;zn++)
          {
            /*
            static const glm::vec3 FRONT = glm::vec3(0,0,-1), BACK = glm::vec3(0,0,1);
            static const glm::vec3 LEFT = glm::vec3(-1,0,0), RIGHT = glm::vec3(1,0,0);
            static const glm::vec3 TOP = glm::vec3(0,1,0), BOTTOM = glm::vec3(0,-1,0);
            static const glm::vec3 NONE = glm::vec3(0);

            glm::vec3 vx,vy,vz;

            static const glm::vec3 xnarr[3] =
            {
              LEFT,
              NONE,
              RIGHT,
            };

            static const glm::vec3 ynarr[3] =
            {
              BOTTOM,
              NONE,
              TOP,
            };

            static const glm::vec3  znarr[3] =
            {
              FRONT,
              NONE,
              BACK,
            };

            vx = xnarr[xn];
            vy = ynarr[yn];
            vz = znarr[zn];

            glm::vec3 norm;
            if(xn == 1 && yn == 1 && zn == 1)
            {
              norm = glm::vec3(0);
            }
            else norm = glm::normalize(vx+vy+vz);
            */




      }
    }
  }
  auto t2 = high_resolution_clock::now();

  auto t2t1 = duration_cast<milliseconds> (t2-t1);
  //std::cout << "Step1 takes:" << t2t1.count() << "ms\n";

  double punit = 1.0/lod;
  glm::dvec3 p[8] =
  {
    glm::dvec3(0,0,punit),
    glm::dvec3(punit,0,punit),
    glm::dvec3(punit,0,0),
    glm::dvec3(0,0,0),
    glm::dvec3(0,punit,punit),
    glm::dvec3(punit,punit,punit),
    glm::dvec3(punit,punit,0),
    glm::dvec3(0,punit,0),
  };

  for(int x = 0;x<CHUNKSIZE;x++)
  {
    for(int z = 0;z<CHUNKSIZE;z++)
    {
      for(int y = 0;y<CHUNKSIZE;y++)
      {
        glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
        if(!vArray.shouldRender(chunkLocalPos)) continue;
        RenderType renderType = blockVisibleType(chunkLocalPos);
        if(renderType == OPAQUE) continue;

        VertexData vertex;
        vertex.renderType = OPAQUE;
        vertex.ao = NOAO;
        vertex.tb = BOTTOMTS;
        vertex.rl = LEFTTS;
        vertex.face = FRONTF;



        GRIDCELL cell;


        int rx = x*lod;
        int ry = y*lod;
        int rz = z*lod;


        static const glm::dvec3 subCubeLookup[8]=
        {
          glm::dvec3(0,0,1), glm::dvec3(1,0,1),glm::dvec3(1,0,0),glm::dvec3(0,0,0),
          glm::dvec3(0,1,1), glm::dvec3(1,1,1),glm::dvec3(1,1,0),glm::dvec3(0,1,0),
        };



        for(int subCubes=0;subCubes<8;subCubes++)
        {
          int tx = rx + subCubeLookup[subCubes].x;
          int ty = ry + subCubeLookup[subCubes].y;
          int tz = rz + subCubeLookup[subCubes].z;



          vArray.setCell(&cell,0,tx,ty,tz+1);
          vArray.setCell(&cell,1,tx+1,ty,tz+1);
          vArray.setCell(&cell,2,tx+1,ty,tz);
          vArray.setCell(&cell,3,tx,ty,tz);

          vArray.setCell(&cell,4,tx,ty+1,tz+1);
          vArray.setCell(&cell,5,tx+1,ty+1,tz+1);
          vArray.setCell(&cell,6,tx+1,ty+1,tz);
          vArray.setCell(&cell,7,tx,ty+1,tz);

          TRIANGLE tris[6];
          int ntris = Polygonise(cell,p,50,tris);


          /* If ntris is 2 intialize the quad checking algorithm in order to test
          if the given quad should be flipped in order to deal with problems of anistropy.
          Otherwise just render normally.

          */
          if(ntris == 2)
          {

            std::vector<VERTEX> singleList;
            std::vector<VERTEX> duplicateList;
            std::vector<VERTEX> fullList;
            for(int i=0;i<3;i++)
            {

              auto itr = std::find(singleList.begin(),singleList.end(),tris[0].p[i]);
              if(itr == singleList.end())
              {
                fullList.push_back(tris[0].p[i]);
                singleList.push_back(tris[0].p[i]);
              }
              else
              {

                duplicateList.push_back(tris[0].p[i]);
                singleList.erase(itr);
              }
            }
            for(int i=0;i<3;i++)
            {
              auto itr = std::find(singleList.begin(),singleList.end(),tris[1].p[i]);
              if(itr == singleList.end())
              {
                fullList.push_back(tris[1].p[i]);
                singleList.push_back(tris[1].p[i]);
              }
              else
              {

                duplicateList.push_back(tris[1].p[i]);
                singleList.erase(itr);
              }
            }
            int startId;

            glm::dvec3 flip,shift,norm;
            glm::dmat4 rot;
            bool shouldFlip = false;
            if( (duplicateList[0].id != duplicateList[1].id) && (singleList[0].id !=  duplicateList[0].id || singleList[0].id !=  duplicateList[1].id))
            {
              shouldFlip = true;
            }
            if(fullList.size() == 4 && duplicateList.size() == 2)
            {
              shift = (singleList[0].pos+ singleList[1].pos + duplicateList[0].pos + duplicateList[1].pos)/4.0;
              norm = glm::normalize(glm::cross(tris[0].p[0].pos -tris[0].p[1].pos,tris[0].p[0].pos -tris[0].p[2].pos));

              vertex.flatNorm = norm;
              flip.x = abs(norm.z) > 0.9 ? -1.0 : 1.0;
              flip.y = abs(norm.x) > 0.9 ? -1.0 : 1.0;
              flip.z = abs(norm.y) > 0.9 ? -1.0 : 1.0;

              if(!(flip.x != 1.0f || flip.y != 1.0f || flip.z != 1.0f))
              {
                //If the quad is not axis alligned dont try to flip it
                shouldFlip = false;
              }
              rot = glm::scale(flip);
            }
            if(shouldFlip)
            {
              for(int tri =0;tri<2;tri++)
              {
                //Change ordering since the quad is going to be flipped
                glm::dvec3 points[3];
                glm::vec3 norms[3];
                points[1] = glm::dvec3(rot*glm::dvec4(glm::dvec3(tris[tri].p[0].pos) -shift,1)) +shift;
                points[0] = glm::dvec3(rot*glm::dvec4(glm::dvec3(tris[tri].p[1].pos) -shift,1)) +shift;
                points[2] = glm::dvec3(rot*glm::dvec4(glm::dvec3(tris[tri].p[2].pos) -shift,1)) +shift;


                for(int i=0;i<3;i++)
                {
                  for(int j=0;j<4;j++)
                  {
                    //The new positions arent exactly the same as the old positions, so find nearest
                    if(abs(glm::length(glm::dvec3(fullList[j].pos)- points[i])) < 0.01)
                    {
                      norms[i] = fullList[j].norm;
                      points[i] = fullList[j].pos;
                      vertex.texIds[i] = fullList[j].id;
                    }
                  }
                }

                for(int j=0;j<3;j++)
                {
                  vertex.pos = points[j] + glm::dvec3(chunkLocalPos) + subCubeLookup[subCubes]/(double)lod;
                  vertex.norm = norms[j];
                  int id = addVertex(vertex);
                  oIndices.push_back(id);
                }
              }
              //Skip normal rendering
              continue;
            }
          }

          //Normal rendering
          for(int i=0;i<ntris;i++)
          {
            glm::dvec3 normal = glm::cross(tris[i].p[0].pos- tris[i].p[1].pos,tris[i].p[0].pos - tris[i].p[2].pos);
            vertex.texIds[0] = tris[i].p[0].id;
            vertex.texIds[1] = tris[i].p[1].id;
            vertex.texIds[2] = tris[i].p[2].id;
            vertex.flatNorm = glm::normalize(normal);
            for(int j=0;j<3;j++)
            {
              vertex.norm = glm::normalize(tris[i].p[j].norm);
              vertex.pos = glm::dvec3(tris[i].p[j].pos) + glm::dvec3(chunkLocalPos) + subCubeLookup[subCubes]/(double)lod;
              vertex.vId = j;
              int id = addVertex(vertex);
              oIndices.push_back(id);
            }
          }
        }
      }
    }
  }

  auto t3 = high_resolution_clock::now();
  auto t3t2 = duration_cast<milliseconds> (t3-t2);
  //std::cout << "Step2 takes:" << t3t2.count() << "ms\n";

  oVertices.shrink_to_fit();
  oIndices.shrink_to_fit();

}
