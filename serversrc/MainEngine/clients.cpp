
#include "include/clients.h"
#include "include/world.h"
#include "include/bsp.h"
#include "include/server.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>



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
  char name[24];
  recvMessage(name,24);
  userName = std::string(name);
  sendMessage(&id,sizeof(id));
  std::cout << userName << " Connected\n";
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

std::shared_ptr<OutMessage> Client::getInfo()
{
  std::shared_ptr<OutMessage> tmp(new OutMessage(90,id,0,0,xpos,ypos,zpos,NULL));
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
    std::shared_ptr<OutMessage> m;
    if(msgQueue.empty())
    {
      if(chunkQueue.empty())
      {
        continue;
      }
      else
      {
        m = chunkQueue.front();
        chunkQueue.pop();
      }
    }
    else
    {
      m = msgQueue.front();
      msgQueue.pop();
    }
    if(!open) return;
    int arr[5];
    arr[0] = ((m->opcode << 24) | (m->ext1 << 16) | (m->ext2 << 8) | m->ext3);
    arr[1] = m->x.i;
    arr[2] = m->y.i;
    arr[3] = m->z.i;
    arr[4] = m->data == NULL ? 0 : m->data->length();


    //std::cout << "Sending Message: " << arr[0] << arr[1] << arr[2] << arr[3] <<arr[4] << "\n";
    sendMessage(arr,5*sizeof(int));

    if(m->data != NULL)
    {
      sendMessage(m->data->data(), arr[4]);
    }


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
    int buf[5];
    int sizeOfMessage = 5*sizeof(int);
    recvMessage(buf,sizeOfMessage);
    if(fatalError) break;
    uchar opcode = (buf[0] >> 24) & 0xFF;
    uchar ext1 = (buf[0] >> 16) & 0xFF;
    uchar ext2 = (buf[0] >> 8) & 0xFF;
    uchar ext3 = buf[0] & 0xFF;
    IntOrFloat x,y,z;
    x.i = buf[1];
    y.i = buf[2];
    z.i = buf[3];
    int length = buf[4];
    switch(opcode)
    {
      case (0):
      {
        /*
        std::thread chunkThread(&Client::sendChunk,this,x,y,z);
        chunkThread.detach();
        */
        sendChunk(x.i,y.i,z.i);
      }
        break;
      case (1):
        std::cout << "deleting block\n";
        curWorld->delBlock(x.i,y.i,z.i);
        sendDelBlockAll(x.i,y.i,z.i);
        break;
      case (2):
        curWorld->addBlock(x.i,y.i,z.i,ext1);
        sendAddBlockAll(x.i,y.i,z.i,ext1);
        break;
      case (91):
        sendPositionAll(x.f,y.f,z.f);
        break;
      case (100):
        std::cout << "received opcode 100\n";
        receiveChatMessage(length);
        break;
      case (0xFF):
        disconnect();
        break;
      default:
        std::cout << "Unknown opcode: " << (int)opcode << "\n";
    }
  }
}

void Client::receiveChatMessage(int length)
{
  char* msg = new char[length];
  recvMessage(msg,length);
  std::cout << userName + " said: " + msg + "\n";
  delete[] msg;
}

void Client::sendPositionAll(float x, float y,float z)
{
  setPos(glm::vec3(x,y,z));
  std::shared_ptr<OutMessage> tmp(new OutMessage(91,id,0,0,x,y,z,NULL));
  Server::messageAll(tmp);
}

void Client::sendAddBlockAll(int x, int y, int z, uchar id)
{
  std::shared_ptr<OutMessage> tmp(new OutMessage(2,id,0,0,x,y,z,NULL));
  Server::messageAll(tmp);
}

void Client::sendChunk(int x,int y,int z)
{
  curWorld->generateChunk(x,y,z);
  std::shared_ptr<std::string> msg = curWorld->getChunk(x,y,z)->getCompressedChunk();
  std::shared_ptr<OutMessage> tmp(new OutMessage(0,0,0,0,x,y,z,msg));
  msgQueue.push(tmp);
}
void Client::sendChunkAll(int x,int y,int z)
{
  curWorld->generateChunk(x,y,z);
  std::shared_ptr<std::string> msg = curWorld->getChunk(x,y,z)->getCompressedChunk();
  std::shared_ptr<OutMessage> tmp(new OutMessage(0,0,0,0,x,y,z,msg));
  Server::messageAll(tmp);
}
void Client::sendDelBlockAll(int x, int y, int z)
{
  std::shared_ptr<OutMessage> tmp(new OutMessage(1,0,0,0,x,y,z,NULL));
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
