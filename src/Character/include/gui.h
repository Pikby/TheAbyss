#pragma once
#include <stack>
#include "../../headers/shaders.h"
#include "../../headers/threadSafeQueue.h"

class GUIRenderer
{
private:
  int width, height;
public:
  GUIRenderer(){};
  GUIRenderer(int Width, int Height,std::string userName);
  void createMenuScreens();
  void openInventoryGUI();
  void closeInventoryGUI();
  void initFreetype();
  void initResourcePaths();
  void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(0,0,0));
  void addCharacterToChat(int c);
};
