#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "widgets.h"
#include "../include/gui.h"



Button::Button(const glm::vec2& Origin, const glm::vec2& Dimensions,const std::string& Text,std::function<void(int)> ClickHandler)
    : Widget(Origin,Dimensions),text(Text),clickHandler(ClickHandler)
  {
    textOrigin = origin + glm::vec2(0,dimensions.y/2);
    glm::vec3 stringDims = GUI::textRenderer.calculateStringDimensions(text,1);
    textScale = (dimensions.x-padding)/(stringDims.x/GUI::dimensions.x);
  }

void Button::draw()
{
  glm::vec4 color = glm::vec4(1);
  if(glfwGetTime() < pressedTime) color = glm::vec4(glm::vec3(0.5),1);

  GUI::drawQuad(origin,origin+ dimensions,color,CIRCLEQUAD);
  GUI::renderText(text,textOrigin,textScale,glm::vec4(1,0,0,1));
}

void Button::handleMouseInput(int button, int mode)
{
  if(clickHandler == NULL) return;
  if(mode == GLFW_PRESS)
  {
    pressedTime = glfwGetTime() + 1;
    clickHandler(button);
  }
  else
  {
    pressedTime = glfwGetTime();
  }

}
