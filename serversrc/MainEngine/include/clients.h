#pragma once
typedef unsigned char uchar;
#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <glm/glm.hpp>
#include "../../headers/threadSafeQueue.h"
#include "messages.h"
class World;
class Server;
class Client
{
  private:
    int fd;
    uchar id;
    std::string userName;
    std::atomic_bool open,fatalError;
    std::atomic<float> xpos,ypos,zpos;
    std::thread sendThread;
    std::thread recvThread;
    std::thread chunkThread;
    World* curWorld;
  public:
    TSafeQueue<std::shared_ptr<OutMessage>> msgQueue;
    TSafeQueue<glm::ivec3> chunkQueue;
    ~Client();
    Client(int Fd,uchar Id,World* world);
    int getFD();
    void setPos(glm::vec3 newPos);
    std::shared_ptr<OutMessage> getInfo();
    void generateAndSendChunks();
    void addChunkToQueue(int x, int y, int z);
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
