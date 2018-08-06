#pragma once
#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <glm/glm.hpp>
#include "../../headers/threadSafeQueue.h"
typedef unsigned char uchar;
class World;

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

class Server;
class Client
{
  private:
    int fd;
    uchar id;
    std::string userName;
    bool fatalError;
    std::atomic_bool open;
    std::atomic<float> xpos;
    std::atomic<float> ypos;
    std::atomic<float> zpos;
    std::thread sendThread;
    std::thread recvThread;
    World* curWorld;
  public:
    TSafeQueue<std::shared_ptr<OutMessage>> msgQueue;
    TSafeQueue<std::shared_ptr<OutMessage>> chunkQueue;
    Client(int Fd,uchar Id,World* world);
    int getFD();
    void setPos(glm::vec3 newPos);
    std::shared_ptr<OutMessage> getInfo();
    void receiveChatMessage(int length);
    void sendMessages();
    void recvMessages();
    void sendChunk(int x,int y,int z);
    void sendChunkAll(int x, int y, int z);
    void sendAddBlockAll(int x, int y, int z, uchar id);
    void sendDelBlockAll(int x, int y, int z);
    void sendInitAll();
    void sendPositionAll(float x, float y, float z);
    void sendExit();
    void disconnect();
    void errorDisconnect();
    inline void sendMessage(const void *buffer,int length);
    inline void recvMessage(void*buffer,int length);
};
