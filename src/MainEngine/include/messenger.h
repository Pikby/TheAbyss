#include <chrono>
#include "../../headers/3dmap.h"
#include "../../headers/threadSafeQueue.h"
#include "messages.h"

class Messenger
{
private:
  int fd;
  std::chrono::system_clock::time_point lastPing;
  inline int pack4chars(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
  {
    return ((a << 24) | (b << 16) | (c << 8) | d);
  }
  int floatBitsToInt(float f)
  {
      return *(int*)&f;
  }
public:
  explicit Messenger(){};
   Messenger(const Messenger&) = delete;
   Messenger& operator=(const Messenger&) = delete;
   ~Messenger() = default;
  Map3D<bool> requestMap;
  TSafeQueue<OutMessage> messageQueue;
  void setupSockets(std::string ipAddress,std::string port);
  InMessage receiveAndDecodeMessage();
  void disconnect();
  void requestChunk(int x, int y, int z);
  void createMoveRequest(float x, float y, float z);
  void createAddBlockRequest(int x, int y, int z, uint8_t id);
  void createChatMessage(const std::string & msg);
  void createViewDirectionChangeRequest(float x, float y, float z);

  void requestAddBlock(int x, int y, int z, uint8_t id);
  void createDelBlockRequest(int x, int y, int z);
  void requestViewDirectionChange(float x, float y, float z);
  void requestDelBlock(int x, int y, int z);
  void requestMove(float x, float y, float z);
  void requestExit();
  void pingStart();
  double pingEnd();
  void createPingRequest();
  void sendChatMessage(std::shared_ptr<std::string> msg);
  void receiveMessage(void *buffer,int length);
  void sendMessage(const void* buffer,int length);
  void createChunkRequest(int x, int y, int z);
};
