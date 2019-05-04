#include "include/bsp.h"

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}



typedef struct
{
   glm::vec3 p[3];
} TRIANGLE;

typedef struct
{
  glm::vec3 p[8];
  double val[8];
} GRIDCELL;


/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
glm::vec3 VertexInterp(double isolevel,glm::vec3 p1,glm::vec3 p2, double valp1,double valp2)
{
   glm::vec3 p;

   if (abs(isolevel-valp1) < 0.00001)
      return(p1);
   if (abs(isolevel-valp2) < 0.00001)
      return(p2);
   if (abs(valp1-valp2) < 0.00001)
      return(p1);
   double mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/


int Polygonise(GRIDCELL grid,double isolevel,TRIANGLE *triangles)
{


   const int edgeTable[256]={
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
    const int triTable[256][16] =
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
      glm::vec3 vertlist[12];
   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

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

  bool side1Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side1Pos));
  bool side2Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side2Pos));

  if(side1Opacity && side2Opacity)
  {
    return FULLCOVER;
  }
  ivec3 corner = side1+side2-norm;
  ivec3 cornerPos = pos + corner;
  bool cornerOpacity = (OPAQUE==parent->blockVisibleTypeOOB(cornerPos));
  return static_cast<AmbientOcclusion> (side1Opacity+side2Opacity+cornerOpacity);
}

void BSP::addIndices(RenderType renderType,int index1, int index2, int index3, int index4)
{
  std::vector<uint>* curBuffer;
  if(renderType == OPAQUE)
  {
    curBuffer = &oIndices;
  }
  else if(renderType == TRANSLUCENT)
  {
    curBuffer = &tIndices;
  }
  else
  {
    std::cout << "Indices error\n";
    return;
  }
  //Add the First triangle of the square
  curBuffer->push_back(index1);
  curBuffer->push_back(index2);
  curBuffer->push_back(index3);

  //Add the second triangle of the square
  curBuffer->push_back(index4);
  curBuffer->push_back(index1);
  curBuffer->push_back(index3);
}


int BSP::addVertex(const VertexData &vertex)
{

  std::vector<float>* curBuffer;
  if(vertex.renderType == OPAQUE)
  {
    curBuffer = &oVertices;
  }
  else if(vertex.renderType == TRANSLUCENT)
  {
    curBuffer = &tVertices;
  }
  else
  {
    std::cout << "adding vertex of a transparentobject ???\n";
  }
  int numbVert = curBuffer->size()/2;
  //Adds position vector

  int fullPos = 0;
  fullPos = ((int)(vertex.pos.x*16) & 0x3FF);
  fullPos |= (((int)(vertex.pos.y*16) & 0x3FF) << 10);
  fullPos |= (((int)(vertex.pos.z*16) & 0x3FF) << 20);

  curBuffer->push_back(*(float*)&fullPos);
  //Add the normal and texture ids
  uint8_t compactFace;
  switch(vertex.face)
  {
    case (FRONTF):   compactFace = 0b1001; break;
    case (BACKF):    compactFace = 0b0001; break;
    case (TOPF):     compactFace = 0b0010; break;
    case (BOTTOMF):  compactFace = 0b1010; break;
    case (RIGHTF):   compactFace = 0b0100; break;
    case (LEFTF):    compactFace = 0b1100; break;
  }
  uint8_t normandtex = compactFace | (vertex.ao << 4)| vertex.tb | vertex.rl;
  uint8_t texId = vertex.blockId;


  uint32_t package = pack4chars(normandtex,texId,1,1);
  curBuffer->push_back(*(float*)&package);


  return numbVert;
}

void BSP::build()
{
  //std::cout << "Starting build\n";
  //std::cout << glm::to_string(parent->chunkPos) << "\n";
  oVertices = std::vector<float>();
  oIndices = std::vector<uint>();
  tVertices = std::vector<float>();
  tIndices = std::vector<uint>();


  lightList.clear();
  oVertices.reserve(20000);
  oIndices.reserve(10000);
  Array3D<BlockFace,32> arrayFaces;


  Array3D<bool,33> arrayVertices;
  //Populate arrayFaces in order to determine all visible faces of the mesh

  for(int x = 0; x<CHUNKSIZE;x++)
  {
    for(int z = 0;z<CHUNKSIZE;z++)
    {
      for(int y = 0;y<CHUNKSIZE;y++)
      {
        glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         //if(!blockExists(chunkLocalPos)) continue;
         RenderType renderType = blockVisibleType(chunkLocalPos);

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         bool defaultNull = true;



         if(renderType == OPAQUE)
         {
          auto check = [&](Faces face,const glm::ivec3& pos,bool* flag)
          {
            auto neigh = parent->getNeighbour(face);
            if(neigh != NULL)
            {
              if(neigh->blockVisibleType(pos) == OPAQUE) *flag = true;
            }
            else if(defaultNull) *flag = true;
          };
          if(x+1 >= CHUNKSIZE)
          {
            check(RIGHTF,glm::ivec3(0,y,z),&rightNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x+1,y,z)) == OPAQUE) rightNeigh = true;

          if(x-1 < 0)
          {
            check(LEFTF,glm::ivec3(CHUNKSIZE-1,y,z),&leftNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x-1,y,z)) == OPAQUE) leftNeigh = true;

          if(y+1 >= CHUNKSIZE)
          {
            check(TOPF,glm::ivec3(x,0,z),&topNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x,y+1,z)) == OPAQUE) topNeigh = true;

          if(y-1 < 0)
          {
            check(BOTTOMF,glm::ivec3(x,CHUNKSIZE-1,z),&bottomNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x,y-1,z)) == OPAQUE) bottomNeigh = true;

          if(z+1 >= CHUNKSIZE)
          {
            check(BACKF,glm::ivec3(x,y,0),&backNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x,y,z+1)) == OPAQUE) backNeigh = true;

          if(z-1 < 0)
          {
            check(FRONTF,glm::ivec3(x,y,CHUNKSIZE-1),&frontNeigh);
          }
          else if(blockVisibleType(glm::ivec3(x,y,z-1))==OPAQUE) frontNeigh = true;
         }
         else if(renderType == TRANSLUCENT)
         {
           uint8_t id = getBlock(chunkLocalPos);
           auto check = [&](Faces face,const glm::ivec3& pos,bool* flag)
           {
             auto neigh = parent->getNeighbour(face);
             if(neigh != NULL)
             {
               if(neigh->getBlock(pos) == id) *flag = true;
             }
             else if(defaultNull) *flag = true;
           };
           if(x+1 >= CHUNKSIZE)
           {
             check(RIGHTF,glm::ivec3(0,y,z),&rightNeigh);
           }
           else if(getBlock(glm::ivec3(x+1,y,z)) == id) rightNeigh = true;

           if(x-1 < 0)
           {
             check(LEFTF,glm::ivec3(CHUNKSIZE-1,y,z),&leftNeigh);
           }
           else if(getBlock(glm::ivec3(x-1,y,z)) == id) leftNeigh = true;

           if(y+1 >= CHUNKSIZE)
           {
             check(TOPF,glm::ivec3(x,0,z),&topNeigh);
           }
           else if(getBlock(glm::ivec3(x,y+1,z)) == id) topNeigh = true;

           if(y-1 < 0)
           {
             check(BOTTOMF,glm::ivec3(x,CHUNKSIZE-1,z),&bottomNeigh);
           }
           else if(getBlock(glm::ivec3(x,y-1,z)) == id) bottomNeigh = true;

           if(z+1 >= CHUNKSIZE)
           {
             check(BACKF,glm::ivec3(x,y,0),&backNeigh);
           }
           else if(getBlock(glm::ivec3(x,y,z+1)) == id) backNeigh = true;

           if(z-1 < 0)
           {
             check(FRONTF,glm::ivec3(x,y,CHUNKSIZE-1),&frontNeigh);
           }
           else if(getBlock(glm::ivec3(x,y,z-1)) == id) frontNeigh = true;
         }
         BlockFace& curFace = arrayFaces.get(chunkLocalPos);
         if(!frontNeigh)  curFace.setFace(FRONTF);
         if(!backNeigh)   curFace.setFace(BACKF);
         if(!topNeigh)    curFace.setFace(TOPF);
         if(!bottomNeigh) curFace.setFace(BOTTOMF);
         if(!leftNeigh)   curFace.setFace(LEFTF);
         if(!rightNeigh)  curFace.setFace(RIGHTF);

         uint8_t blockId = getBlock(chunkLocalPos);
         Block curBlock = ItemDatabase::blockDictionary[blockId];


         if(curBlock.isLightSource)
         {
           if(!lightExists(chunkLocalPos))
           {
             std::cout << "Adding lightsource in chunk" << glm::to_string(parent->chunkPos) << "at" << glm::to_string(chunkLocalPos) << "\n";
             Light tempLight = {chunkLocalPos+(parent->chunkPos)*32,curBlock.lightColor,curBlock.lightRadius};
             addToLightList( chunkLocalPos,tempLight);
           }

         }
       }
     }
   }

/*
   auto createVertices = [&](FaceData faceData,bool front)
   {
     using namespace glm;
     ivec3 depthOffset,rightVector,topVector;

     switch(faceData.face)
     {
       case(TOPF):
         rightVector = vec3(1,0,0);
         topVector = vec3(0,0,1);
         depthOffset = vec3(0,1,0);
         break;
      case(BOTTOMF):
        rightVector = vec3(1,0,0);
        topVector = vec3(0,0,1);
        depthOffset = vec3(0,0,0);
        break;
      case(RIGHTF):
        rightVector = vec3(0,0,1);
        topVector = vec3(0,1,0);
        depthOffset = vec3(1,0,0);
        break;
      case(LEFTF):
        rightVector = vec3(0,0,1);
        topVector = vec3(0,1,0);
        depthOffset = vec3(0,0,0);
        break;
      case(BACKF):
        rightVector = vec3(1,0,0);
        topVector = vec3(0,1,0);
        depthOffset = vec3(0,0,1);
        break;
      case(FRONTF):
        rightVector = vec3(1,0,0);
        topVector = vec3(0,1,0);
        depthOffset = vec3(0,0,0);
        break;
     }
     ivec3 bottomright = faceData.pos + depthOffset + rightVector;
     ivec3 topright = faceData.pos + depthOffset + topVector + rightVector;
     ivec3 bottomleft = faceData.pos + depthOffset;
     ivec3 topleft = faceData.pos + depthOffset + topVector;

     AmbientOcclusion ao00,ao01,ao11,ao10;
     ao00 = getAO(faceData.pos,faceData.face,BOTTOMTS,LEFTTS);
     ao01 = getAO(faceData.pos,faceData.face,TOPTS,LEFTTS);
     ao11 = getAO(faceData.pos,faceData.face,TOPTS,RIGHTTS);
     ao10 = getAO(faceData.pos,faceData.face,BOTTOMTS,RIGHTTS);

     VertexData vertex;
     vertex.face = faceData.face;
     vertex.renderType = faceData.renderType;
     vertex.blockId = faceData.blockId;






     vertex.pos = bottomleft;
     vertex.tb = BOTTOMTS;
     vertex.rl = LEFTTS;
     vertex.ao = ao00;
     int index1 = addVertex(vertex);

     vertex.pos = topleft;
     vertex.tb = TOPTS;
     vertex.rl = LEFTTS;
     vertex.ao = ao01;
     int index2 = addVertex(vertex);

     vertex.pos = topright;
     vertex.tb = TOPTS;
     vertex.rl = RIGHTTS;
     vertex.ao = ao11;
     int index3 = addVertex(vertex);

     vertex.pos = bottomright;
     vertex.tb = BOTTOMTS;
     vertex.rl = RIGHTTS;
     vertex.ao = ao10;
     int index4 = addVertex(vertex);
     if(front)
     {
       if(ao00 + ao11 > ao01 + ao10)
       {
         addIndices(faceData.renderType,index2,index3,index4,index1);
       }
       else addIndices(faceData.renderType,index1,index2,index3,index4);
     }
     else
     {
       if(ao00 + ao11 > ao01 + ao10)
       {
         addIndices(faceData.renderType,index4,index3,index2,index1);
       }
       else addIndices(faceData.renderType,index3,index2,index1,index4);
     }

   };
   */
  //Go through the array and test all faces and attempt to join them using greedy meshing
   for(int x = 0;x<CHUNKSIZE;x++)
   {
     for(int z = 0;z<CHUNKSIZE;z++)
     {
       for(int y = 0;y<CHUNKSIZE;y++)
       {
         glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         RenderType renderType = blockVisibleType(chunkLocalPos);
         BlockFace curFace  = arrayFaces.get(chunkLocalPos);
         uint8_t blockId = getBlock(chunkLocalPos);
         Block tempBlock = ItemDatabase::blockDictionary[blockId];

         if(renderType == OPAQUE) continue;
         if(curFace.isEmpty()) continue;

         GRIDCELL cell;
         memset(&cell,0,sizeof(GRIDCELL));
         cell.p[0] = glm::vec3(0,0,1);
         cell.p[1] = glm::vec3(1,0,1);
         cell.p[2] = glm::vec3(1,0,0);
         cell.p[3] = glm::vec3(0,0,0);
         cell.p[4] = glm::vec3(0,1,1);
         cell.p[5] = glm::vec3(1,1,1);
         cell.p[6] = glm::vec3(1,1,0);
         cell.p[7] = glm::vec3(0,1,0);

         for(int i=0;i<8;i++)
         {
           //cell.p[i] -= glm::vec3(-0.5);
         }

         //std::cout << "Origin: " << glm::to_string(chunkLocalPos) << "\n";
         bool neighbours[27];
         memset(neighbours,0,27*sizeof(bool));


         for(int yn = 0;yn<3;yn++)
         {
           for(int zn = 0;zn<3;zn++)
           {
               for(int xn = 0;xn<3;xn++)
               {
               //std::cout << "i:" << xn+zn*3+yn*3*3 << ":" << glm::to_string(glm::vec3(chunkLocalPos) -glm::vec3(1,1,1)+glm::vec3(xn,yn,zn)) << "\n";
               if(parent->blockVisibleTypeOOB(glm::vec3(chunkLocalPos) -glm::vec3(1,1,1)+glm::vec3(xn,yn,zn)) == OPAQUE)
               {
                 neighbours[xn+zn*3+yn*3*3] = true;
               }
               else neighbours[xn+zn*3+yn*3*3] = false;
             }
           }
         }




         const int lookupBottom[4][8] =
         {
           {3,4,6,7,12,13,15,16},
           {4,5,7,8,13,14,16,17},
           {1,2,4,5,10,11,13,14},
           {0,1,3,4,9,10,12,13},
         };

         for(int i=0;i<8;i++)
         {
           int modVal = i < 4 ? 0 : 9;
           const int* vals = lookupBottom[i % 4];

           bool exists = false;
           for(int j=0;j<8;j++)
           {
             if(neighbours[vals[j] + modVal])
             {
               exists = true;
               break;
             }
           }
           if(exists) cell.val[i] = 10;
         }

         TRIANGLE tris[6];
         int ntris = Polygonise(cell,6,tris);
         //std::cout << ntris << "\n";
         for(int i=0;i<ntris;i++)
         {
           VertexData vertex;
           vertex.blockId = blockId;
           vertex.renderType = OPAQUE;
           vertex.ao = NOAO;
           vertex.tb = BOTTOMTS;
           vertex.rl = LEFTTS;
           vertex.face = FRONTF;



           for(int j=0;j<3;j++)
           {
             vertex.pos = tris[i].p[j] + glm::vec3(chunkLocalPos) + glm::vec3(0.5);
             //std::cout <<  glm::to_string(vertex.pos) << "\n";
             int id = addVertex(vertex);
             oIndices.push_back(id);
           }
         }


         /*
         FaceData face;
         face.renderType = renderType;
         face.pos = chunkLocalPos;
         face.textCount = glm::vec2(1,1);
         if(curFace.getFace(TOPF))
         {
           face.blockId = tempBlock.getFace(TOPF);
           face.face = TOPF;
           createVertices(face,true);
         }
         if(curFace.getFace(BOTTOMF))
         {
           face.blockId = tempBlock.getFace(BOTTOMF);
           face.face = BOTTOMF;
           createVertices(face,false);
         }

         if(curFace.getFace(RIGHTF))
         {
           face.blockId = tempBlock.getFace(RIGHTF);
           face.face = RIGHTF;
           createVertices(face,true);
         }

         if(curFace.getFace(LEFTF))
         {
           face.blockId = tempBlock.getFace(LEFTF);
           face.face = LEFTF;
           createVertices(face,false);
         }

         if(curFace.getFace(BACKF))
         {
           face.blockId = tempBlock.getFace(BACKF);
           face.face = BACKF;
           createVertices(face,false);
         }

         if(curFace.getFace(FRONTF))
         {
           face.blockId = tempBlock.getFace(FRONTF);
           face.face = FRONTF;
           createVertices(face,true);
         }
         */

       }
     }
   }
}
