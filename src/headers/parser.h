#pragma once
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
class Parser
{
private:
  std::string filePath;
  std::map<std::string, std::string> dictionary;
public:
  Parser(){};
  Parser(std::string filepath)
  {
    using namespace std;
    filePath = filepath;
    ifstream file(filePath);

    if(!file.is_open())
    {
      std::cout << "Error opening dicionary at: " << filePath << "\n";
      return;
    }

    string line;

    while(getline(file,line))
    {


      int colonPos = line.find(':');
      if(colonPos == std::string::npos) continue;

      string key = line.substr(0,colonPos-1);
      string value = line.substr(colonPos+1,line.length());
      key.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );
      value.erase( std::remove_if( value.begin(), value.end(), ::isspace ), value.end() );

      dictionary[key] = value;
    }
  }
  /*
  ~Parser()
  {
      std::ofstream file(filePath);
      if(!file.is_open())
      {
        std::cout << "Error opening dicionary at: " << filePath << "\n";
        return;
      }
      for(auto it = dictionary.cbegin(); it != dictionary.cend(); ++it)
      {
        std::string line = it->first + ":" + it->second + "\n";
        file.write(line.c_str(),line.size());
      }
  }
*/
  void add(std::string key, std::string val)
  {
    dictionary[key] = val;
  }

  bool exists(std::string key)
  {
    return dictionary.count(key);
  }
  std::string get(std::string key)
  {
    if(dictionary.count(key) == 1)
    {
      return dictionary[key];
    }
    else return "";
  }

  void print()
  {
    for(auto it = dictionary.cbegin(); it != dictionary.cend(); ++it)
    {
      std::cout << it->first << ":" << it->second <<  "\n";
    }
  }
};
