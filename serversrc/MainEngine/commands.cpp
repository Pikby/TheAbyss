#include "../headers/clients.h"
#include <sstream>
#include <map>
#include <functional>
using namespace std;
typedef const std::string & Args;
void quit(Args arguments)
{
  std::cout << "Exiting the application..." << std::endl;
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


void Server::parseCommand(std::string command)
{
  dictionary["quit"] = quit;
  dictionary["test"] = test;
  dictionary["say"] = Server::say;

  stringstream ss;
  ss << command;
  string operation;
  ss >> operation;
  /*
  vector<string> arguments;
  while(!ss.eof())
  {
    string arg;
    ss >> arg;
    arguments.push_back(arg);
  }
*/
  if(dictionary.count(operation) == 1)
  {
    dictionary[operation](command.substr(operation.size(),command.size()));
  }
}
