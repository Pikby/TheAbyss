#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "include/menus.h"
#include "include/gui.h"
#include "include/mainchar.h"
#include "Widgets/widgets.h"

Menu::~Menu()
{
  for(auto itr = viewableList.begin(); itr!= viewableList.end();itr++)
  {
    delete *itr;
  }
}

void Menu::setFocusTarget(Widget* widget)
{
  focusTarget = widget;
}

void Menu::GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if(focusTarget == NULL) return;
  focusTarget->handleKeyInput(key,action);
}

void Menu::GLFWCharCallBack(GLFWwindow* window, uint character)
{
  if(focusTarget == NULL) return;
  focusTarget->handleCharInput(character);
}

void Menu::handleMouseHover(const glm::vec2 &mousePos)
{
  for(auto itr = viewableList.begin();itr != viewableList.end();itr++)
  {
    Widget* widget = (*itr);
    if(widget->isIn(mousePos))
    {
      widget->handleMouseHover(mousePos);
    }
  }
}


void Menu::GLFWMouseButtonCallback(GLFWwindow* window, int button,int action, int mods)
{

  if(dynamic_cast<InGame*>(focusTarget) != NULL)
  {
    focusTarget->handleMouseInput(button,action);
  }
  for(auto itr = viewableList.begin();itr != viewableList.end();itr++)
  {
    Widget* widget = (*itr);
    if(widget->isIn(GUI::getMousePos()))
    {
      widget->handleMouseInput(button,action);
      if(widget->isFocusable())
      {
        if(focusTarget != NULL)
        {
          focusTarget->setFocused(false);
        }
        focusTarget = widget;
        widget->setFocused(true);
      }
    }
  }
}

void Menu::drawGUI()
{
  for(auto itr = viewableList.begin(); itr!= viewableList.end();itr++)
  {
    (*itr)->draw();
  }
}

void Menu::GLFWCursorCallback(GLFWwindow* window, double xpos,double ypos)
{
  if(focusTarget == NULL) return;
  focusTarget->handleMouseMovement(xpos,ypos);
}

void Menu::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(focusTarget == NULL) return;
  focusTarget->handleScrollInput(xoffset,yoffset);
}
