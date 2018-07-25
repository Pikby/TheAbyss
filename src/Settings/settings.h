#include <string>
#include "../headers/parser.h"
class Settings
{
private:
  static Parser definitions;
public:
  Settings();
  static std::string get(std::string key);
  static void set(std::string key, std::string val);
  static void print();
};
