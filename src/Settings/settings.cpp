#include "../headers/parser.h"
#include "settings.h"
#include <algorithm>
#include <sstream>

Parser Settings::definitions;

const char defaultSettings[] =
  "horzrenderdistance:2\n"
  "vertrenderdistance:2\n"
  "ip:127.0.0.1\n"
  "port:3030\n"
  "renderbuffer:3\n"
  "username:kibby\n"
  "windowheight:768\n"
  "windowwidth:1366\n"
  "worldname:default\n";



std::string Settings::toLowerCase(std::string input)
{
  std::transform(input.begin(),input.end()+1,input.begin(), ::tolower);
  return input;
}

void Settings::initSettings()
{
  std::cout << defaultSettings;
  using namespace std;
  std::string filePath = "../assets/settings.ini";
  ifstream file(filePath);

  if(!file.is_open())
  {
    file.close();
    ofstream outFile(filePath);
    outFile << defaultSettings;
    outFile.close();
    file = ifstream(filePath);
  }
 definitions = Parser(&file);
 file.close();
}

void Settings::save()
{
  std::cout << "Saving\n";
  definitions.save("../assets/settings.ini");
}


std::string Settings::get(std::string key)
{
  key = toLowerCase(key);

  return definitions.get(key);
}


void Settings::set(std::string val,std::string key)
{
  key = toLowerCase(key);
  val = toLowerCase(val);

  definitions.add(val,key);
}

void Settings::print()
{
  definitions.print();
}
