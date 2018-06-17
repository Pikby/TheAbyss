#include <map>
#include <string>
#include <fstream>
class Parser
{
private:
  std::map<string, string> dictionary;
public:
  Parser(std::string filePath)
  {
    using namespace std;
    ifstream file(filePath);

    if(!file.is_open())
    {
      std::cout << "Error opening dicionary at: " << filePath << "\n";
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
