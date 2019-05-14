#ifndef MENULIBRARY
#define MENULIBRARY

#include "gui.h"

class Menu
{
private:
  std::list<Widget*> viewableList;
  Widget* focusTarget = NULL;
public:
  Menu(){}
  ~Menu();
  void setFocusTarget(Widget* widget);
  void GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
  void GLFWCharCallBack(GLFWwindow* window, uint character);
  void GLFWCursorCallback(GLFWwindow* window, double xpos, double ypos);
  void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  void handleMouseHover(const glm::vec2 &mousePos);
  void drawGUI();

  //addWidget transfers ownership to the menu, and the menu becomes in charge of freeing
  uint addWidget(Widget* widget){viewableList.push_back(widget);return viewableList.size()-1;}
  Widget* findWidgetAtMouse();
};

#endif
