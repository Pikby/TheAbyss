#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "widgets.h"
#include "../include/gui.h"



Button::Button(const glm::vec2& Origin, const glm::vec2& Dimensions,const std::string& Text,std::function<void(int)> ClickHandler)
    : Widget(Origin,Dimensions),text(Text),clickHandler(ClickHandler)
  {

    glm::vec3 stringDims = GUI::calculateStringDimensions(text,1);
    textScale = 64*(dimensions.x-padding)/(stringDims.x/GUI::dimensions.x);
  }

void Button::draw()
{

  glm::vec4 drawColor = color;
  if(glfwGetTime() < pressedTime) drawColor = color*0.5f;
  glm::vec2 textOrigin = origin + glm::vec2(0,dimensions.y/2);
  GUI::drawQuad(origin,origin+ dimensions,drawColor,CIRCLEQUAD);
  GUI::renderText(text,textOrigin,textScale,textColor);
}


void Button::submitEvent()
{
  clickHandler(GLFW_MOUSE_BUTTON_1);
}

void Button::handleMouseInput(int button, int mode)
{
  if(clickHandler == NULL) return;
  if(mode == GLFW_PRESS)
  {
    pressedTime = glfwGetTime() + 1;
    submitEvent();
  }
  else
  {
    pressedTime = glfwGetTime();
  }

}
