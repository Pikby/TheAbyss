
#include "../headers/clients.h"
#include "../headers/world.h"
#include "../headers/bsp.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

std::thread Server::serverCommands;
std::mutex Server::clientMutex;
std::shared_ptr<Client> Server::clients[4];
World* Server::curWorld;

Client::Client(int Fd,uchar Id,World* world)
{
  curWorld = world;
  fd = Fd;
  id = Id;
  xpos = 10;
  ypos = 50;
  zpos = 10;
  open = true;
  fatalError = false;
  if (send(fd, &id, sizeof(id), 0) == -1)
  {
      std::cout << "ERROR: failed to send id message." << std::endl;
  }

  Server::sendInitAll(this);
  Server::retClients(this);
  sendThread = std::thread(&Client::sendMessages,this);
  recvThread = std::thread(&Client::recvMessages,this);
  sendThread.detach();
  recvThread.detach();
}

int Client::getFD()
{
  return fd;
}
void Client::setPos(glm::vec3 newPos)
{
  xpos = newPos.x;
  ypos = newPos.y;
  zpos = newPos.z;
}

std::shared_ptr<Message> Client::getInfo()
{
  std::shared_ptr<Message> tmp(new Message(90,id,0,0,*(int*)&xpos,*(int*)&ypos,*(int*)&zpos,NULL));
  return tmp;
}

inline void Client::sendMessage(const void *buffer,int length)
{
  const char* buf = (char*)buffer;
  int totalSent = 0;
  while(totalSent <length)
  {
    if(fatalError || !open) return;
    int curSent = send(fd,buf+totalSent,length-totalSent,0);
    if(curSent == -1)
    {
      std::cout << "ERROR: sending message to Client:" << id << "\n";
      errorDisconnect();
      return;
    }
    totalSent += curSent;

  }
}
inline void Client::recvMessage(void *buffer, int length)
{
  char* buf = (char*)buffer;
  int totalReceived = 0;
  while(totalReceived < length)
  {
    if(fatalError || !open) return;
    int curReceived = recv(fd,buf+totalReceived,length-totalReceived,0);
    if(curReceived == -1)
    {
      std::cout << "Error: receiving Message from client:" << id << "\n";
      errorDisconnect();
      return;
    }
    totalReceived += curReceived;
  }
}

void Client::sendMessages()
{
  while(open)
  {
    queueMutex.lock();
    if(msgQueue.empty())
    {
      queueMutex.unlock();
      continue;
    }
    if(!open) return;
    std::shared_ptr<Message> m = msgQueue.front();
    queueMutex.unlock();
    int arr[5];
    arr[0] = ((m->opcode << 24) | (m->ext1 << 16) | (m->ext2 << 8) | m->ext3);
    arr[1] = m->x;
    arr[2] = m->y;
    arr[3] = m->z;
    arr[4] = m->data == NULL ? 0 : m->data->length();


    //std::cout << "Sending Message: " << arr[0] << arr[1] << arr[2] << arr[3] <<arr[4] << "\n";
    sendMessage(arr,5*sizeof(int));

    if(m->data != NULL)
    {
      sendMessage(m->data->data(), arr[4]);
    }
    queueMutex.lock();
    msgQueue.front() = NULL;
    msgQueue.pop();
    queueMutex.unlock();

  }

  if(fatalError) return;

  int arr[5];
  arr[0] = 0xFFFFFFFF;
  std::cout << "Sending dc msg\n";
  sendMessage(arr,5*sizeof(int));
}

void Client::disconnect()
{
  open = false;
  Server::remove(id);
  std::cout << "Client disconnectting safely\n";
}

void Client::errorDisconnect()
{
  open = false;
  fatalError = true;
  Server::remove(id);
  std::cout << "Client disconnectting due to error\n";
}

void Client::recvMessages()
{
  while(open)
  {
    int buf[4];
    int sizeOfMessage = 4*sizeof(int);
    recvMessage(buf,sizeOfMessage);
    if(fatalError) break;
    uchar opcode = (buf[0] >> 24) & 0xFF;
    uchar ext1 = (buf[0] >> 16) & 0xFF;
    uchar ext2 = (buf[0] >> 8) & 0xFF;
    uchar ext3 = buf[0] & 0xFF;
    int x = buf[1];
    int y = buf[2];
    int z = buf[3];

    switch(opcode)
    {
      case (0):
        //curWorld->sendChunk(x,y,z,fd);
        sendChunk(x,y,z);
        break;
      case (1):
        curWorld->delBlock(x,y,z);
        sendDelBlockAll(x,y,z);
        break;
      case (2):
        curWorld->addBlock(x,y,z,ext1);
        sendAddBlockAll(x,y,z,ext1);
        break;
      case (91):
        sendPositionAll(*(float*)&x,*(float*)&y,*(float*)&z);
        break;
      case (0xFF):
        disconnect();
        break;
      default:
        std::cout << "Unknown opcode: " << (int)opcode << "\n";
    }
  }
}


void Client::sendPositionAll(float x, float y,float z)
{
  setPos(glm::vec3(x,y,z));
  std::shared_ptr<Message> tmp(new Message(91,id,0,0,*(int*)&x,*(int*)&y,*(int*)&z,NULL));
  Server::messageAll(tmp);
}

void Client::sendAddBlockAll(int x, int y, int z, uchar id)
{
  std::shared_ptr<Message> tmp(new Message(2,id,0,0,x,y,z,NULL));
  Server::messageAll(tmp);
}

void Client::sendChunk(int x,int y,int z)
{
  curWorld->generateChunk(x,y,z);
  std::shared_ptr<std::string> msg = curWorld->getChunk(x,y,z)->getCompressedChunk();
  std::shared_ptr<Message> tmp(new Message(0,0,0,0,x,y,z,msg));
  queueMutex.lock();
  msgQueue.push(tmp);
  queueMutex.unlock();
}
void Client::sendChunkAll(int x,int y,int z)
{
  curWorld->generateChunk(x,y,z);
  std::shared_ptr<std::string> msg = curWorld->getChunk(x,y,z)->getCompressedChunk();
  std::shared_ptr<Message> tmp(new Message(0,0,0,0,x,y,z,msg));
  Server::messageAll(tmp);
}
void Client::sendDelBlockAll(int x, int y, int z)
{
  std::shared_ptr<Message> tmp(new Message(1,0,0,0,x,y,z,NULL));
  Server::messageAll(tmp);
}

void Client::sendExit()
{
  int arr[5];
  arr[0] = 0xFFFFFFFF;
  if (send(fd, arr, 5*sizeof(int), 0) == -1)
  {
      std::cout << "ERROR: failed to send exit message." << std::endl;
  }
}

void Server::handleServerCommands()
{
    std::string line;
    while(std::getline(std::cin,line))
    {
      parseCommand(line);
    }
}

void Server::initServer(World* temp)
{
  curWorld = temp;
  serverCommands = std::thread(Server::handleServerCommands);
  serverCommands.detach();
}


void Server::add(int fd)
{
  clientMutex.lock();
  for(int i = 0;i<MAX_CLIENTS;i++)
  {
    if(clients[i] == NULL)
    {
      std::cout << "Adding client " << i << '\n';
      clientMutex.unlock();
      std::shared_ptr<Client> tmp(new Client(fd,i,curWorld));
      clientMutex.lock();
      clients[i] = tmp;
      clientMutex.unlock();
      break;
    }
  }
}
void Server::remove(int id)
{
  std::cout << clients[id].use_count() << "\n";
  clientMutex.lock();
  clients[id] = NULL;
  clientMutex.unlock();
  std::shared_ptr<Message> tmp(new Message(99,id,0,0,0,0,0,NULL));
  messageAll(tmp);
}

void Server::sendInitAll(Client* target)
{
  clientMutex.lock();
  std::shared_ptr<Message> msg = target->getInfo();
  for(int i =0;i<MAX_CLIENTS;i++)
  {
    std::shared_ptr<Client> curClient = clients[i];
    if(curClient != NULL && curClient.get() != target)
    {
      curClient->queueMutex.lock();
      curClient->msgQueue.push(msg);
      curClient->queueMutex.unlock();
    }
  }
  clientMutex.unlock();
}

void Server::retClients(Client* target)
{
  clientMutex.lock();
  for(int i =0;i<MAX_CLIENTS;i++)
  {
    std::shared_ptr<Client> curClient = clients[i];
    if(curClient != NULL && curClient.get() != target)
    {
      target->queueMutex.lock();
      target->msgQueue.push(curClient->getInfo());
      target->queueMutex.unlock();
    }
  }
  clientMutex.unlock();
}

void Server::messageAll(std::shared_ptr<Message> msg)
{
  clientMutex.lock();
  for(int i=0;i<MAX_CLIENTS;i++)
  {

    std::shared_ptr<Client> curClient = clients[i];
    if(curClient != NULL)
    {
      curClient->queueMutex.lock();
      curClient->msgQueue.push(msg);
      curClient->queueMutex.unlock();
    }
  }
   clientMutex.unlock();
}
