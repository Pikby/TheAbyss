#version 430 core

layout(points) in;
layout(points) out;

in ivec3[] blockPos;
out ivec3 outValue;


uniform sampler3D blockArr;
#define CHUNKSIZE 32;

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

struct GridCell
{
  double val[8];
  uint8_t id[8];
  glm::vec3 norm[8];
}


int getBlockTextureIdFromPos(ivec3 pos)
{
  return int(texelFetch(blockArr,pos,0).r*255);

}



int getVertexValue(ivec3 pos)
{
  int id = getBlockTextureIdFromPos(floor(pos/2));

  int weight;
  weight = (id!= 0) ? 100 : 0;
  return weight;
}


void main()
{
  GRIDCELL gridArray[27];
  ivec3 pos = blockPos[0];
  for(int x = 0;x<3;x++) for(int y =0;y<3;y++) for(int z=0;z<3;z++)
  {
    vec3 offset = vec3(x,y,z);
    int weight = getVertexValue(pos+offset);
    GRIDCELL &cell = GridArray[x+y*3+z*3*3];
    cell.set()

  }
}
