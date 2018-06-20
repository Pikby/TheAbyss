#pragma once
#define PORT 3030
#include "../headers/threadSafeQueue.h"
#include "../headers/3dmap.h"
typedef unsigned char uchar;
struct Message
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  int x;
  int y;
  int z;
  int length;
};

class Messenger
{
private:
  int fd;
public:
  Map3D<bool> requestMap;
  TSafeQueue<Message> messageQueue;
  void setupSockets(std::string ipAddress);
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
  void createChunkRequest(int x, int y, int z);
};
