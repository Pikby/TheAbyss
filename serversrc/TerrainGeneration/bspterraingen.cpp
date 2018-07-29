#include "../headers/bsp.h"
#include "FastNoiseSIMD.h"
#include "../headers/FastNoise.h"

//Perlin noise object
FastNoise BSP::noise2d;
FastNoiseSIMD* BSP::noise3d;

void BSP::initTerrainGenEngine()
{
  std::cout << "Initalizing terrain gen algorithm\n";
  int seed=10000;
  noise3d = FastNoiseSIMD::NewFastNoiseSIMD(seed);
  noise3d->SetNoiseType(FastNoiseSIMD::PerlinFractal);
  std::cout << "SIMD level detected as" << noise3d->GetSIMDLevel() << "\n";
  noise2d.SetSeed(seed);
  noise2d.SetFractalOctaves(6);
  noise2d.SetFrequency(0.01);
  noise2d.SetFractalLacunarity(6);
  noise2d.SetFractalGain(5);
}

void BSP::generateTerrain()
{
  float* caveSet = noise3d->GetPerlinSet(xCoord*CHUNKSIZE,zCoord*CHUNKSIZE,yCoord*CHUNKSIZE,CHUNKSIZE,CHUNKSIZE,CHUNKSIZE);
  float* heightMap = noise3d->GetPerlinSet(xCoord*CHUNKSIZE,zCoord*CHUNKSIZE,0,CHUNKSIZE,CHUNKSIZE,1);

  for(uint index = 0;index<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;index++)
  {
    int curHeight = index % 32;
    int height = CHUNKSIZE+200*heightMap[index/32];
    int realy = curHeight + yCoord*CHUNKSIZE;

    if(caveSet[index] < 0)
    {

    }
    else if(realy < height)
    {
      if(realy == height - 1) worldArray[index] = 2;
      else worldArray[index] = 1;
    }

  }
  FastNoiseSIMD::FreeNoiseSet(caveSet);
  FastNoiseSIMD::FreeNoiseSet(heightMap);
}
