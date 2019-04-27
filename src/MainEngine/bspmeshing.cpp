#include "include/bsp.h"

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
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
  int numbVert = curBuffer->size()/2;
  //Adds position vector

  int fullPos = 0;
  fullPos = vertex.pos.x;
  fullPos |= (vertex.pos.y << 6);
  fullPos |= (vertex.pos.z << 12);

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
  oVertices = std::vector<float>();
  oIndices = std::vector<uint>();
  tVertices = std::vector<float>();
  tIndices = std::vector<uint>();


  lightList.clear();
  oVertices.reserve(20000);
  oIndices.reserve(10000);
  Array3D<BlockFace,32> arrayFaces;
  //Populate arrayFaces in order to determine all visible faces of the mesh
  for(int x = 0; x<CHUNKSIZE;x++)
  {
    for(int z = 0;z<CHUNKSIZE;z++)
    {
      for(int y = 0;y<CHUNKSIZE;y++)
      {
        glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         if(!blockExists(chunkLocalPos)) continue;
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
           uchar id = getBlock(chunkLocalPos);
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

         uchar blockId = getBlock(chunkLocalPos);
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
     vertex.ao = NOAO;
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

  //Go through the array and test all faces and attempt to join them using greedy meshing
   for(int x = 0; x<CHUNKSIZE;x++)
   {
     for(int z = 0;z<CHUNKSIZE;z++)
     {
       for(int y = 0;y<CHUNKSIZE;y++)
       {
         glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         RenderType renderType = blockVisibleType(chunkLocalPos);
         BlockFace curFace  = arrayFaces.get(chunkLocalPos);
         //if(curFace.isEmpty()) continue;
         uint8_t blockId = getBlock(chunkLocalPos);
         Block tempBlock = ItemDatabase::blockDictionary[blockId];

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

       }
     }
   }

}
