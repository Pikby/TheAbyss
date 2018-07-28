#include "../headers/clients.h"
#include "../headers/server.h"
#include <sstream>
#include <map>
#include <functional>
using namespace std;
typedef const std::string & Args;
void Server::quit(Args arguments)
{
  std::shared_ptr<Message> tmp(new Message(255,0,0,0,0,0,0,NULL));
  messageAll(tmp);
  std::cout << "Waiting for clients to disconnect\n";
  while(numberOfClients > 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  std::cout << "All clients disconnected, shutting down\n";
  exit(0);
}

vector<string> getArguments(Args arguments)
{
  stringstream ss;
  ss << arguments;
  vector<string> argList;
  while(!ss.eof())
  {
    string arg;
    ss >> arg;
    argList.push_back(arg);
  }
  return argList;
}

void test(Args arguments)
{
  auto stuff = getArguments(arguments);
  for(auto itr = stuff.begin();itr != stuff.end();++itr)
  {
    std::cout << (*itr) << "\n";
  }
}
std::map<std::string,std::function<void(Args)>> dictionary;


void Server::say(Args arguments)
{
  std::shared_ptr<Message> tmp(new Message(100,0,0,0,0,0,0,std::make_shared<std::string>(arguments)));
  messageAll(tmp);
}

void Server::teleport(Args arguments)
{
  auto argList = getArguments(arguments);
  if(argList.size()<4)
  {
    throw "Error: Not enough arguments";
    return;
  }
  char id;
  float xpos,ypos,zpos;
  try
  {
    id = std::stoi(argList[0]);
    xpos = std::stof(argList[1]);
    ypos = std::stof(argList[2]);
    zpos = std::stof(argList[3]);

  }
  catch(...)
  {
    throw "Error: Invalid arguments";
    return;
  }
  auto curClient = clients[id];
  if(curClient != NULL)
  {
    std::shared_ptr<Message> msg(new Message(91,id,0,0,*(int*)&xpos,*(int*)&ypos,*(int*)&zpos,NULL));
    curClient->setPos(glm::vec3(xpos,ypos,zpos));
    curClient->msgQueue.push(msg);
  }
  else throw "Error: Id does not exist";

}

void Server::initServerCommands()
{
  dictionary["quit"] = Server::quit;
  dictionary["test"] = test;
  dictionary["say"] = Server::say;
  dictionary["teleport"] = Server::teleport;
  dictionary["tp"] = Server::teleport;
}

void Server::parseCommand(std::string command)
{

  stringstream ss;
  ss << command;
  string operation;
  ss >> operation;

  if(dictionary.count(operation) == 1)
  {
    try
    {
      dictionary[operation](command.substr(operation.size(),command.size()));
    }
    catch(const char* msg)
    {
      std::cout << msg <<"\n";
    }
  }
  else
  {
    std::cout << "Sorry, command: " << operation << " does not exist\n";
  }
}
