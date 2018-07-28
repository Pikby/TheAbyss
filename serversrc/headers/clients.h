#pragma once
#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <glm/glm.hpp>
#include "threadSafeQueue.h"
typedef unsigned char uchar;
class World;
struct Message
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  int x;
  int y;
  int z;
  std::shared_ptr<std::string> data;
  Message(uchar a,uchar b, uchar c, uchar d, int e, int f, int g,std::shared_ptr<std::string> stuff)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x = e;
    y = f;
    z = g;
    data = stuff;
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
    TSafeQueue<std::shared_ptr<Message>> msgQueue;
    Client(int Fd,uchar Id,World* world);
    int getFD();
    void setPos(glm::vec3 newPos);
    std::shared_ptr<Message> getInfo();
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
