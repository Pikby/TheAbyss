#pragma once
#define PORT 3030
typedef unsigned char uchar;
#include "../../headers/threadSafeQueue.h"
#include "../../headers/3dmap.h"
#include "messages.h"

class Messenger
{
private:
  int fd;
  inline int pack4chars(char a, char b, char c, char d)
  {
    return ((a << 24) | (b << 16) | (c << 8) | d);
  }
  int floatBitsToInt(float f)
  {
      return *(int*)&f;
  }
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
