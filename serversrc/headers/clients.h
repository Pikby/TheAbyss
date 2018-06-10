#include <iostream>
#include <atomic>
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

class ClientList;
class Client : public WorldWrap
{
  private:
    int fd;
    uchar id;
    std::atomic_bool open;
    std::atomic<float> xpos;
    std::atomic<float> ypos;
    std::atomic<float> zpos;
    std::thread sendThread;
    std::thread recvThread;
    World* curWorld;
    ClientList* parent;
  public:
    std::mutex queueMutex;
    std::queue<std::shared_ptr<Message>> msgQueue;
    Client(int Fd,uchar Id,World* world,ClientList* par);
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

};

#define MAX_CLIENTS 4
class ClientList
{
  private:
    std::mutex clientMutex;
    std::shared_ptr<Client> clients[MAX_CLIENTS];
    World* curWorld;
  public:
    ClientList(World* temp);
    void add(int fd);
    void remove(int id);
    void messageAll(std::shared_ptr<Message> msg);
    void retClients(Client* target);
    void sendInitAll(Client* target);
};
