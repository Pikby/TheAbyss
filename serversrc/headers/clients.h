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
    int id;
    std::atomic_bool open;
    glm::vec3 pos;
    std::thread sendThread;
    std::thread recvThread;
    World* curWorld;
    ClientList* parent;
  public:
    std::mutex queueMutex;
    std::queue<std::shared_ptr<Message>> msgQueue;
  Client(int Fd,int Id,World* world,ClientList* par);
  int getFD();
  void changePos(glm::vec3 newPos);
  void sendMessages();
  void recvMessages();
  void sendChunk(int x,int y,int z);
  void sendChunkAll(int x, int y, int z);
  void sendExit();

};

#define MAX_CLIENTS 32
class ClientList
{
  private:
    std::shared_ptr<Client> clients[MAX_CLIENTS];
    World* curWorld;
  public:
    ClientList(World* temp);
    void add(int fd);
    void remove(int id);
    void messageAll(std::shared_ptr<Message> msg);

};
