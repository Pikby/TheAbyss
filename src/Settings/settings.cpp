#include "parser.h"
#include "settings.h"
Parser Settings::definitions("../assets/settings.ini");

Settings::Settings()
{
  definitions = Parser("../assets/settings.ini");
}

std::string Settings::get(std::string key)
{
  return definitions.get(key);
}

void Settings::print()
{
  definitions.print();
}
