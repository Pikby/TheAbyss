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
struct InMessage
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  int length;
  InMessage(){};
  InMessage(uchar a, uchar b, uchar c, uchar d, int xpos, int ypos, int zpos,int len)
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
  InMessage(uchar a, uchar b, uchar c, uchar d, float xpos, float ypos, float zpos,int len)
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
struct OutMessage
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  std::shared_ptr<std::string> data;
  OutMessage();
  OutMessage(uchar a, uchar b, uchar c, uchar d, int xpos, int ypos, int zpos,std::shared_ptr<std::string> newData)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.i = xpos;
    y.i = ypos;
    z.i = zpos;
    data = newData;
  }
  OutMessage(uchar a, uchar b, uchar c, uchar d, float xpos,float ypos, float zpos,std::shared_ptr<std::string> newData)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.f = xpos;
    y.f = ypos;
    z.f = zpos;
    data = newData;
  }
};


class Messenger
{
private:
  int fd;
public:
  Map3D<bool> requestMap;
  TSafeQueue<OutMessage> messageQueue;
  void setupSockets(std::string ipAddress,std::string port);
  InMessage receiveAndDecodeMessage();
  void requestChunk(int x, int y, int z);
  void createMoveRequest(float x, float y, float z);
  void createAddBlockRequest(int x, int y, int z, uchar id);
  void createChatMessage(const std::string & msg);
  void requestAddBlock(int x, int y, int z, uchar id);
  void createDelBlockRequest(int x, int y, int z);
  void requestDelBlock(int x, int y, int z);
  void requestMove(float x, float y, float z);
  void requestExit();
  void sendChatMessage(std::shared_ptr<std::string> msg);
  void receiveMessage(void *buffer,int length);
  void sendMessage(const void* buffer,int length);
  void createChunkRequest(int x, int y, int z);
};
