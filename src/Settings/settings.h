#include <string>
#include "parser.h"
class Settings
{
private:
  static Parser definitions;
public:
  Settings();
  static std::string get(std::string key);
  static void add(std::string key, std::string val);
  static void print();
};
