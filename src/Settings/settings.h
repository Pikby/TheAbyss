#include <string>
#include "../headers/parser.h"
class Settings
{
private:
  static Parser definitions;
  static std::string toLowerCase(std::string in);
public:
  static void initSettings();
  static void save();
  static std::string get(std::string key);
  static void set(std::string key, std::string val);
  static void print();
};
