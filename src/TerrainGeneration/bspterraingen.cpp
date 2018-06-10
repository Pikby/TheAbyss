#include "../headers/bsp.h"
#include "../headers/FastNoise.h"

int seed;
//Perlin noise object
FastNoise perlin;
void BSP::generateTerrain()
{
  perlin.SetSeed(seed);
  perlin.SetFractalOctaves(8);
  perlin.SetFrequency(0.01);
  perlin.SetFractalLacunarity(8);
  perlin.SetFractalGain(5);
  double freq = 128;
  int oct = 8;
  for(int x=0;x<CHUNKSIZE;x++)
  {
    for(int z=0;z<CHUNKSIZE;z++)
    {
      //Generates a height map for the surface
      int height = CHUNKSIZE+200*perlin.GetPerlin(x+xCoord*CHUNKSIZE,z+zCoord*CHUNKSIZE);
      for(int y=0;y<CHUNKSIZE;y++)
      {
        int realx = xCoord*CHUNKSIZE+x;
        int realy = yCoord*CHUNKSIZE+y;
        int realz = zCoord*CHUNKSIZE+z;


        if(realy < height)
        {
          if(realy == height - 1) addBlock(x,y,z,2);
          else addBlock(x,y,z,1);
        }
        else if(realy<120)
        {
          //addBlock(x,y,z,3);
        }

      }
    }
  }
}
