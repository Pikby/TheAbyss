#pragma once
#define PORT 3030
#include "../../headers/threadSafeQueue.h"
#include "../../headers/3dmap.h"
typedef unsigned char uchar;

union IntOrFloat
{
  int i;
  float f;
};
struct Message
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  int length;
  Message(){};
  Message(uchar a, uchar b, uchar c, uchar d, int xpos, int ypos, int zpos,int len)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.i = xpos;
    y.i = ypos;
    z.i = zpos;
    length = len;
  }
  Message(uchar a, uchar b, uchar c, uchar d, float xpos, float ypos, float zpos,int len)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.f = xpos;
    y.f = ypos;
    z.f = zpos;
    length = len;
  }
};

class Messenger
{
private:
  int fd;
public:
  Map3D<bool> requestMap;
  TSafeQueue<Message> messageQueue;
  void setupSockets(std::string ipAddress,std::string port);
  Message receiveAndDecodeMessage();
  void requestChunk(int x, int y, int z);
  void createMoveRequest(float x, float y, float z);
  void createAddBlockRequest(int x, int y, int z, uchar id);
  void requestAddBlock(int x, int y, int z, uchar id);
  void createDelBlockRequest(int x, int y, int z);
  void requestDelBlock(int x, int y, int z);
  void requestMove(float x, float y, float z);
  void requestExit();
  void receiveMessage(void *buffer,int length);
  void sendMessage(const void* buffer,int length);
  void createChunkRequest(int x, int y, int z);
};
