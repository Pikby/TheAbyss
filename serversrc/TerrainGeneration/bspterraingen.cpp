#include "../MainEngine/include/bsp.h"
#include "FastNoiseSIMD/FastNoiseSIMD.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
//Perlin noise object
FastNoiseSIMD* BSP::noise3d;



#define NOISERES 7
struct PerlinSet
{
  static FastNoiseSIMD* heightGen;
  static FastNoiseSIMD* caveGen;
  float* caveSet;
  float* heightSet;
  glm::ivec3 pos;
  float ratio;
  static void initSet(int seed)
  {
    heightGen = FastNoiseSIMD::NewFastNoiseSIMD(seed);
    heightGen->SetFrequency(0.01);
    heightGen->SetFractalOctaves(4);
    caveGen = FastNoiseSIMD::NewFastNoiseSIMD(seed);
    caveGen->SetFrequency(0.02);
    caveGen->SetFractalOctaves(5);
    std::cout << "SIMD level detected as" << heightGen->GetSIMDLevel() << "\n";
  }
  PerlinSet(const int x, const int y, const int z)
  {
    ratio = (float)CHUNKSIZE/(float)(NOISERES);
    pos = glm::ivec3(x/ratio,y/ratio,z/ratio);

    caveSet = caveGen->GetPerlinFractalSet(pos.x,pos.y,pos.z,NOISERES+1,NOISERES+1,NOISERES+1);
    heightSet = heightGen->GetPerlinFractalSet(x,z,0,CHUNKSIZE,CHUNKSIZE,1);
  }
  ~PerlinSet()
  {
    FastNoiseSIMD::FreeNoiseSet(caveSet);
    FastNoiseSIMD::FreeNoiseSet(heightSet);
  }
  float getHeight(const int x, const int z)
  {
    return heightSet[x*CHUNKSIZE+z];
  }
  float lerpValue(const int x, const int y, const int z)
  {
    //std::cout << "Input is" << x << ":" << y << ":" << z << "\n";
    //std::cout << "Ratio :" << ratio << "\n";
    glm::ivec3 pos = glm::ivec3(floor(x/ratio),floor(y/ratio),floor(z/ratio));
    glm::vec3 off = glm::vec3(x/ratio,y/ratio,z/ratio);
    off = glm::vec3(off.x-pos.x,off.y-pos.y,off.z-pos.z);
    return calcLerpValues(pos,off);
  }
  float calcLerpValues(glm::ivec3 pos, glm::vec3 offset)
  {
    //std::cout << glm::to_string(pos) << ":" << glm::to_string(offset) << "\n";
    float initVal = getValue(pos.x,pos.y,pos.z);
    float curTotal = 0;
    float upperXVal = getValue(pos.x+1,pos.y,pos.z);
    curTotal += lerp(initVal,upperXVal,offset.x);

    float upperYVal = getValue(pos.x,pos.y+1,pos.z);
    curTotal += lerp(initVal,upperYVal,offset.y);

    float upperZVal = getValue(pos.x,pos.y,pos.z+1);
    curTotal += lerp(initVal,upperZVal,offset.z);

    return curTotal/3.0f;

  }
  float lerp(const float a,const float b,const float distance)
  {
    return a + distance*(b-a);
  }
  float getValue(const int x,const int y, const int z)
  {
    if(x >= NOISERES+1 || y >= NOISERES+1 || z >= NOISERES+1)
    {
      std::cout << x << ":" << y << ":" << z <<"Its fucking broken\n";
    }
    return caveSet[(NOISERES+1)*(NOISERES+1)*x+(NOISERES+1)*y+z];
  }
};
FastNoiseSIMD* PerlinSet::heightGen;
FastNoiseSIMD* PerlinSet::caveGen;
void BSP::initTerrainGenEngine()
{
  std::cout << "Initalizing terrain gen algorithm\n";
  int seed=10000;
  PerlinSet::initSet(seed);
}
void BSP::generateTerrain()
{
  PerlinSet noise(xCoord*CHUNKSIZE,yCoord*CHUNKSIZE,zCoord*CHUNKSIZE);
  for(int x=0;x<CHUNKSIZE;x++)
  {
    for(int y=0;y<CHUNKSIZE;y++)
    {
      for(int z=0;z<CHUNKSIZE;z++)
      {
        int height = CHUNKSIZE+200*noise.getHeight(x,z);
        int realy = y + yCoord*CHUNKSIZE;

        float caveValue = noise.lerpValue(x,y,z);
        // /std::cout << caveValue << "\n";
        if(caveValue < 0)
        {

        }
        else if(realy < height)
        {
          if(realy == height - 1) worldArray.set(x,y,z,2);
          else worldArray.set(x,y,z,1);
        }

      }

    }
  }
}
