#include "../headers/server.h"
#include "../headers/bsp.h"
#include <time.h>
#include <chrono>
uint Server::ticksPerSecond;
std::atomic<bool> Server::serverLogicOn;
std::atomic<uint> Server::numberOfClients;
std::thread Server::serverCommands, Server::serverLogic;
std::mutex Server::clientMutex;
std::shared_ptr<Client> Server::clients[4];
World* Server::curWorld;

void Server::handleServerLogic()
{
  using namespace std::chrono;
  auto lastFrame = high_resolution_clock::now();
  auto currentFrame = high_resolution_clock::now();
  double tickRate = 1.0f/ticksPerSecond;
  while(serverLogicOn)
  {
    lastFrame = currentFrame;
    currentFrame = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(currentFrame - lastFrame);
    double deltaFrame = time_span.count();
    //std::cout << deltaFrame << "\n";
    if(deltaFrame > tickRate )
    {
      std::cout << "Server is running slowly, what did u do?\n";
    }
    else
    {
      //Stall the thread so it can catch up
      int waitTime = (tickRate-deltaFrame)*1000;
      std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    }
    currentFrame = high_resolution_clock::now();
    //Do all game logic here
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
  BSP::initTerrainGenEngine();
  serverLogicOn = true;
  ticksPerSecond = 2;
  numberOfClients = 0;
  curWorld = temp;
  initServerCommands();
  serverCommands = std::thread(Server::handleServerCommands);
  serverCommands.detach();
  serverLogic = std::thread(Server::handleServerLogic);
  serverLogic.detach();
  memset(clients,0,sizeof(clients));
}


void Server::add(int fd)
{
  clientMutex.lock();
  numberOfClients++;
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
  numberOfClients--;
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
      curClient->msgQueue.push(msg);
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
      target->msgQueue.push(curClient->getInfo());
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
      curClient->msgQueue.push(msg);
    }
  }
   clientMutex.unlock();
}
