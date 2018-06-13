#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
#endif
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "../headers/world.h"
#include "../headers/bsp.h"
#include "../headers/SOIL.h"
#include "../headers/entities.h"


#define PORT 3030
#define ADDRESS "127.0.0.1"
//#define ADDRESS "154.5.254.242"

World::World(int numbBuildThreads,int width,int height)
{
  typedef std::queue<std::shared_ptr<BSPNode>> buildType;
  buildQueue = new buildType[numbBuildThreads];
  numbOfThreads = numbBuildThreads;

  screenWidth = width;
  screenHeight = height;


  worldName = "default";

  const char* texture = "../assets/textures/atlas.png";
  ItemDatabase::loadBlockDictionary("../assets/blockDictionary.dat");


  boost::filesystem::create_directory("saves");
  boost::filesystem::create_directory("saves/"+worldName);
  boost::filesystem::create_directory("saves/"+worldName+"/chunks");

  drawnChunks = 0;
  frontNode = NULL;
  horzRenderDistance = 4;
  vertRenderDistance = 4;
  renderBuffer = 1;
  totalChunks = 0;

  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  //Load and bind the texture from the class
  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGBA);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);


  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
      std::cout << "ERROR: failed to create socket." << std::endl;
      return;
  }
  std::cout << "Successfully created socket." << std::endl;

  sockaddr_in serveraddress;
  serveraddress.sin_family = AF_INET;
  serveraddress.sin_port = htons(PORT);

  #ifdef _WIN32
  WSADATA wsa_data;
  WSAStartup(MAKEWORD(1,1), &wsa_data);

  std::cout << "Doing windows shit\n";
  // addrinfo contains a sockaddr struct
  struct addrinfo *result = NULL, *ptr = NULL, hints;
  // initialize the addrInfo structs
  ZeroMemory(&hints, sizeof(hints));  // fills a block of memory with 0s
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(ADDRESS,"3030", &hints, &result) != 0)
  {
    std::cout << "getaddrinfo failed" << std::endl;
    WSACleanup();
    return;
  }

  // attempt to connect to the first address returned by the call to getaddrinfo
  ptr = result;
  // create a SOCKET for the connecting server
  fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
  // error check to make sure it is a valid socket
  if (fd == INVALID_SOCKET)
  {
      std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
      freeaddrinfo(result);
      WSACleanup();
      return;
  }
  if(connect(fd, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
  {
    closesocket(fd);
    fd = INVALID_SOCKET;
  }


  #else
  std::cout << "Doing linux shit\n";
  inet_pton(AF_INET, ADDRESS, &(serveraddress.sin_addr));


  // connect to the server
  if (connect(fd, (sockaddr*) &serveraddress, sizeof(serveraddress)) < 0)
  {
      std::cout << "ERROR: failed to connect to server." << std::endl;
      return;
  }
  #endif
  std::cout << "Successfully connected to server" << std::endl;

  if(recv(fd,&mainId,sizeof(mainId),0)<0)
  {
    std::cout << "Failed to retrieve id from server\n";
  }

}

Message World::receiveAndDecodeMessage()
{
  int buf[5];



  receiveMessage(buf,sizeof(int)*5);
  uchar opcode = (buf[0] >> 24) & 0xFF;
  uchar ext1 = (buf[0] >> 16) & 0xFF;
  uchar ext2 = (buf[0] >> 8) & 0xFF;
  uchar ext3 = buf[0] & 0xFF;
  Message msg = {opcode,ext1,ext2,ext3,buf[1],buf[2],buf[3],buf[4]};
  return msg;

}

void World::receiveChunk(int x, int y, int z, int length)
{
  //std::cout << "Receiving chunk" << x << ":" << y << ":" << z << "\n";
  char* buffer = new char[length];

  receiveMessage(buffer,length);
  generateChunkFromString(x,y,z,std::string(buffer,length));
  delete[] buffer;
}


void World::requestChunk(int x, int y, int z)
{
  int request[4];
  request[0] = 0;
  request[1] = x;
  request[2] = y;
  request[3] = z;


  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
  //std::cout << "Requesting chunk" << x << ":" << y << ":" << z << "\n";
}


void World::createMoveRequest(float x, float y, float z)
{
  Message tmp = {91,0,0,0,*(int*)&x,*(int*)&y,*(int*)&z,0};
  msgQueueMutex.lock();
  messageQueue.push(tmp);
  msgQueueMutex.unlock();
}

void World::createAddBlockRequest(int x, int y, int z, uchar id)
{
  Message tmp = {2,id,0,0,x,y,z,0};
  msgQueueMutex.lock();
  messageQueue.push(tmp);
  msgQueueMutex.unlock();
}

void World::requestAddBlock(int x, int y, int z, uchar id)
{
  int request[4];
  request[0] = ((2 << 24) | (id << 16) | (0 << 8) | 0);
  request[1] = x;
  request[2] = y;
  request[3] = z;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}

void World::createDelBlockRequest(int x, int y, int z)
{
  Message tmp = {1,0,0,0,x,y,z,0};
  msgQueueMutex.lock();
  messageQueue.push(tmp);
  msgQueueMutex.unlock();

}
void World::requestDelBlock(int x, int y, int z)
{
  int request[4];
  request[0] = ((1 << 24) | (0 << 16) | (0 << 8) | 0);
  request[1] = x;
  request[2] = y;
  request[3] = z;

  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}

void World::requestMove(float x, float y, float z)
{

  int request[4];
  request[0] = ((91 << 24) | (0 << 16) | (0 << 8) | 0);
  request[1] = *(int*)&x;
  request[2] = *(int*)&y;
  request[3] = *(int*)&z;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send message to server\n";
    return;
  }
}

void World::movePlayer(float x, float y, float z, uchar id)
{
  //std::cout << "Moving player" << x << y << z << '\n';
  std::shared_ptr<Entity> tmp = playerList[id];
  if(tmp != NULL)
  {
    //std::cout << "Moving player" << x << y << z << '\n';
    playerList[id]->setPostion(x,y,z);
  }
}

void World::requestExit()
{
  int request[4];
  request[0] = 0xFFFFFFFF;
  if(send(fd,(char*)request,4*sizeof(int),0)<0)
  {
    std::cout << "Failed to send exit message to server\n";
    return;
  }
  std::cout << "exit message Sent\n";
}

void World::addPlayer(float x, float y, float z, uchar id)
{
  std::cout << "Adding player at" << x << ":" << y << ":" << z << ":"<< (int) id << "\n";
  std::shared_ptr<Player> temp(new Player(glm::vec3(x,y,z)));
  playerList[id] = temp;
}


void World::receiveMessage(void *buf,int length)
{
  int totalReceived = 0;
  while(totalReceived<length)
  {
    int curReceived = recv(fd,buf+totalReceived,length-totalReceived,0);
    if(curReceived < 0)
    {
      std::cout << "ERROR: receive chunk." << std::endl;
    }
    totalReceived += curReceived;

  }
}

void World::removePlayer(uchar id)
{
  std::cout << "Removing PLayer\n";
  playerList.erase(id);
}

void World::createChunkRequest(int x, int y, int z)
{
  if(!requestMap.exists(x,y,z))
  {
    Message tmp = {0,0,0,0,x,y,z,0};
    msgQueueMutex.lock();
    messageQueue.push(tmp);
    msgQueueMutex.unlock();
    requestMap.add(x,y,z,true);
  }
}


void World::drawPlayers(glm::mat4* view)
{
  //std::cout << "Map has: " << playerList.size() << "\n";
  for(auto it = playerList.begin(); it != playerList.end();it++)
  {
    it->second->draw(view);
  }
}

void World::drawWorld(glm::mat4 viewMat, glm::mat4 projMat, bool useHSR)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all  resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */



  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  std::shared_ptr<BSPNode>  curNode = frontNode;
  std::shared_ptr<BSPNode>  nextNode;

  glm::mat4 mat = projMat*viewMat;
  while(curNode != NULL)
  {
    nextNode = curNode->nextNode;
    if(curNode->toDelete == true)
    {
      if(curNode->prevNode == NULL) frontNode = curNode->nextNode;
      curNode->del();
    }
    else
    {
      if(useHSR)
      {
        int x = curNode->curBSP.xCoord*CHUNKSIZE+CHUNKSIZE/2;
        int y = curNode->curBSP.yCoord*CHUNKSIZE+CHUNKSIZE/2;
        int z = curNode->curBSP.zCoord*CHUNKSIZE+CHUNKSIZE/2;


        glm::vec4 p1 = glm::vec4(x,y,z,1);

        p1 = mat*p1;
        double w = p1.w;


        if(abs(p1.x) < w && abs(p1.y) < w && 0 < p1.z && p1.z < w)
          curNode->drawOpaque();
      }
      else curNode->drawOpaque();
    }
    curNode = nextNode;
  }
}

void World::generateChunkFromString(int chunkx, int chunky, int chunkz,std::string value)
{
  //std::cout << "Generating chunk" << chunkx << ":" << chunky << ":" << chunkz << "\n";
  if(chunkExists(chunkx,chunky,chunkz))
  {
    std::cout << "Replacing CHunk \n";
    delChunk(chunkx,chunky,chunkz);
  }
  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz,worldName,value));
  BSPmap.add(chunkx,chunky,chunkz,tempChunk);

  if(frontNode == NULL)
  {
    frontNode = tempChunk;
  }
  else
  {
    tempChunk->nextNode = frontNode;
    frontNode->prevNode = tempChunk;
    frontNode = tempChunk;
  }


  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */
  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }
  //Adds the chunk to the current mainBuildQueue if it is not already;
  if(!tempChunk->toBuild)
  {
      addToBuildQueue(tempChunk);
  }
}


void World::addToBuildQueue(std::shared_ptr<BSPNode> curNode)
{
  static int currentThread = 0;
  buildQueue[currentThread].push(curNode);
  curNode->toBuild = true;

  currentThread++;
  if(currentThread >= numbOfThreads) currentThread = 0;
}


void World::generateChunk(int chunkx, int chunky, int chunkz)
{
  if(chunkExists(chunkx,chunky,chunkz)) return;
  requestChunk(chunkx,chunky,chunkz);
  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz,worldName));
  BSPmap.add(chunkx,chunky,chunkz,tempChunk);
  if(frontNode == NULL)
  {
    frontNode = tempChunk;
  }
  else
  {
    tempChunk->nextNode = frontNode;
    frontNode->prevNode = tempChunk;
    frontNode = tempChunk;
  }


  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */
  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }


  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);;
    }
  }
  //Adds the chunk to the current mainBuildQueue if it is not already;
  if(!tempChunk->toBuild)
  {
      addToBuildQueue(tempChunk);
  }
}

void World::renderWorld(float* mainx, float* mainy, float* mainz)
{
  for(int curDis = 0; curDis < horzRenderDistance; curDis++)
   {
     for(int height = 0; height < vertRenderDistance; height++)
     {

       int x = round(*mainx);
       int y = round(*mainy);
       int z = round(*mainz);
       x/= CHUNKSIZE;
       y/= CHUNKSIZE;
       z/= CHUNKSIZE;
       y--;
       int ychunk;
       if(height % 2 == 1) ychunk = y -(height/2);
       else ychunk = y + (height/2);

       int xchunk = x;
       int zchunk = z;


       zchunk = z+curDis;
       xchunk = x;
       for(int i = -curDis+1;i<curDis;i++)
       {
         createChunkRequest(xchunk+i,ychunk,zchunk);
       }

       zchunk = z-curDis;
       xchunk = x;
       for(int i = -curDis+1;i<curDis;i++)
       {
         createChunkRequest(xchunk+i,ychunk,zchunk);
       }
       zchunk = z;
       xchunk = x+curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         createChunkRequest(xchunk,ychunk,zchunk+i);
       }
       zchunk = z;
       xchunk = x-curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         createChunkRequest(xchunk,ychunk,zchunk+i);
       }
     }
   }
}

void World::buildWorld(int threadNumb)
{
  //Empties the build queue
  //TODO Break up queue into smaller pieces and use seperate threads to build them
  while(!buildQueue[threadNumb].empty())
  {
    std::shared_ptr<BSPNode> chunk = buildQueue[threadNumb].front();
    chunk->build();
    buildQueue[threadNumb].pop();
  }
}

std::shared_ptr<BSPNode>  World::getChunk(int x, int y, int z)
{
  return BSPmap.get(x,y,z);
}



void World::delChunk(int x, int y, int z)
{
  std::shared_ptr<BSPNode>  tempChunk = getChunk(x,y,z);
  if(tempChunk != NULL)
  {
    BSPmap.del(x,y,z);
    requestMap.del(x,y,z);
    tempChunk->disconnect();
    /*
    Essentially marks the chunk for death
    Removes all connected pointers besides the linked list
    The Node however must be destroyed in the draw string
    since that is the sole string using opengl functions;
    othereise the vao wont get freed causing a memory leak
    */

  }
}


void World::delScan(float* mainx, float* mainy, float* mainz)
{
  /*
    Scans through the same list as the draw function
    however this time referencing the current position of the player
    and raises the destroy flag if its out of a certain range
  */
  std::shared_ptr<BSPNode>  curNode = frontNode;
  while(curNode != NULL)
  {
    int x = round(*mainx);
    int y = round(*mainy);
    int z = round(*mainz);

    x/= CHUNKSIZE;
    y/= CHUNKSIZE;
    z/= CHUNKSIZE;

    int chunkx = curNode->curBSP.xCoord;
    int chunky = curNode->curBSP.yCoord;
    int chunkz = curNode->curBSP.zCoord;

    curNode = curNode->nextNode;
    if(abs(chunkx-x) > horzRenderDistance + renderBuffer
    || abs(chunky-y) > horzRenderDistance + renderBuffer
    || abs(chunkz-z) > horzRenderDistance + renderBuffer)
    {
      delChunk(chunkx,chunky,chunkz);
    }
  }
}

void World::saveWorld()
{
    std::shared_ptr<BSPNode>  curNode = frontNode;
    while(curNode != NULL)
    {
      curNode->saveChunk();
      curNode = curNode->nextNode;
    }
}

bool World::chunkExists(int x ,int y, int z)
{
  return BSPmap.exists(x,y,z);
}

//If there is an entity, returns it id and position,
//If there is a block, return its position and an id of 0
//If there is nothing return a 0 vector  with -1 id
glm::vec4 World::rayCast(glm::vec3 pos, glm::vec3 front, int max)
{
  float parts = 10;
  for(float i = 0; i<max;i += 1/parts)
  {
    glm::vec3 curPos = pos+i*front;
    //std::cout << curPos.x << ":" << curPos.y << ":" << curPos.z << "\n";
/*
    int id = entityExists(curPos);
    if(id != 0)
    {
      return glm::vec4(curPos,BLOCK);
    }
    */
    if(blockExists(curPos))
    {
        return glm::vec4(curPos,0);
    }
  }
  return glm::vec4(0,0,0,-1);
}

bool World::blockExists(int x, int y, int z)
{
  /*
  Finds which ever chunk holds the block and then calls the blockExists
  function on that chunk with the localized coordinates
  */


  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE)xlocal = 0;
  if(ylocal == CHUNKSIZE)ylocal = 0;
  if(zlocal == CHUNKSIZE)zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {

    if(tempChunk->blockExists(xlocal,ylocal, zlocal))
    {
      return true;
    }
  }
  return false;
}
void World::addBlock(int x, int y, int z, int id)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode> tempChunk = getChunk(xchunk,ychunk,zchunk);
  if(tempChunk != NULL)
  {
    tempChunk->addBlock(xlocal,ylocal,zlocal,id);
    tempChunk->build();



    if(xlocal+1>=CHUNKSIZE)
    {
      if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->build();
    }
    else if(xlocal-1 < 0)
    {
      if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->build();
    }

    if(ylocal+1>=CHUNKSIZE)
    {
      if(tempChunk->topChunk != NULL) tempChunk->topChunk->build();
    }
    else if(ylocal-1 < 0)
    {
      if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->build();
    }

    if(zlocal+1>=CHUNKSIZE)
    {
      if(tempChunk->backChunk != NULL) tempChunk->backChunk->build();
    }
    else if(zlocal-1 < 0)
    {
      if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->build();
    }
  }

}


void World::delBlock(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk = getChunk(xchunk,ychunk,zchunk);
  if(tempChunk != NULL)
  {
    tempChunk->delBlock(xlocal,ylocal,zlocal);
    tempChunk->build();



    if(xlocal+1>=CHUNKSIZE)
    {
      if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->build();
    }
    else if(xlocal-1 < 0)
    {
      if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->build();
    }

    if(ylocal+1>=CHUNKSIZE)
    {
      if(tempChunk->topChunk != NULL) tempChunk->topChunk->build();
    }
    else if(ylocal-1 < 0)
    {
      if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->build();
    }

    if(zlocal+1>=CHUNKSIZE)
    {
      if(tempChunk->backChunk != NULL) tempChunk->backChunk->build();
    }
    else if(zlocal-1 < 0)
    {
      if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->build();
    }
  }
}

/*
void World::requestBlockPlace(int x, int y, int z)
{
  /*
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);



  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {
    tempChunk->delBlock(xlocal,ylocal,zlocal);
  }
  tempChunk->build();

  if(xlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk+1,ychunk,zchunk)->build();
  }
  else if(xlocal-1 < 0)
  {
    getChunk(xchunk-1,ychunk,zchunk)->build();
  }

  if(ylocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk+1,zchunk)->build();
  }
  else if(ylocal-1 < 0)
  {
    getChunk(xchunk,ychunk-1,zchunk)->build();
  }

  if(zlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk,zchunk+1)->build();
  }
  else if(zlocal-1 < 0)
  {
    getChunk(xchunk,ychunk,zchunk-1)->build();
  }

}
*/

void World::updateBlock(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(xchunk,ychunk,zchunk);
  if(!tempChunk->toBuild)
  {
    addToBuildQueue(tempChunk);
  }

}


int World::anyExists(glm::vec3 pos)
{
  if(entityExists(pos.x,pos.y,pos.z))
    return 2;
  else if( blockExists(pos.x,pos.y,pos.z))
    return 1;
  else return 0;

}

bool World::entityExists(glm::vec3 pos)
{
  //TODO
  return false;
}
bool World::entityExists(float x, float y, float z)
{
  //TODO
  return false;
}
