#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "widgets.h"
#include "../include/gui.h"
Quad::Quad(const glm::vec2& Origin,const glm::vec2& Dims,uint id)
                         : Widget(Origin,Dims), textId(id)
{
  focusable = false;
}

void Quad::draw()
{
  if(onDrawFunction != NULL) onDrawFunction(this);

  GUI::setQuadDepth(0.5);
  GUI::drawQuad(origin,origin+dimensions,color,type);
  GUI::setQuadDepth(0);
}
