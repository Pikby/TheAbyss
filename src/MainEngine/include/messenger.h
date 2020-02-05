#include <chrono>
#include <memory>
#include "messages.h"
#include "../../headers/3dmap.h"
#include "../../headers/threadSafeQueue.h"
#include "../../headers/util.h"




class Messenger
{
private:
  int fd;
  std::chrono::system_clock::time_point lastPing;
public:
  Map3D<bool> requestMap;
  TSafeQueue<OutMessage> messageQueue;
  explicit Messenger(){};
   Messenger(const Messenger&) = delete;
   Messenger& operator=(const Messenger&) = delete;
   ~Messenger() = default;
  void setupSockets(std::string ipAddress,std::string port);
  InMessage receiveAndDecodeMessage();
  void disconnect();

  void createMoveRequest(float x, float y, float z);
  void createAddBlockRequest(int x, int y, int z, uint8_t id);
  void createChatMessage(const std::string & msg);
  void createViewDirectionChangeRequest(float x, float y, float z);
  void createDelBlockRequest(int x, int y, int z);

  void requestChunk(int x, int y, int z);
  void requestAddBlock(int x, int y, int z, uint8_t id);
  void requestViewDirectionChange(float x, float y, float z);
  void requestDelBlock(int x, int y, int z);
  void requestMove(float x, float y, float z);
  void requestExit();

  void createPingRequest();
  void pingStart();
  double pingEnd();

  void sendChatMessage(std::shared_ptr<std::string> msg);
  void receiveMessage(void *buffer,int length);
  void sendMessage(const void* buffer,int length);

  void createChunkRequest(int x, int y, int z);
};
