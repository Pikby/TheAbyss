#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "widgets.h"
#include "../include/gui.h"
ImageGrid::ImageGrid(glm::vec2 Origin,int Rows, int Cols,glm::vec2 CellDimensions, glm::vec2 CellOffset)
          : Widget(Origin), rows(Rows), columns(Cols),cellDimensions(CellDimensions)  ,cellOffset(CellOffset)
{
  textureArray = new uint[rows * columns];
  memset(textureArray,0,sizeof(uint)*rows*columns);
  dimensions = glm::vec2(rows,-columns)*(cellDimensions+cellOffset);
}

void ImageGrid::draw()
{
  for(int y=0;y<columns;y++)
  {
    for(int x=0;x<rows;x++)
    {
      glm::vec2 pos = glm::vec2(x,-y);
      glm::vec2 bl = origin + pos*(cellDimensions+cellOffset);
      glm::vec2 tr = origin + cellDimensions+ pos*(cellDimensions+cellOffset);
      GUI::drawQuad(bl,tr,glm::vec4(1));
    }
  }
 
  glm::vec2 bl = origin+glm::vec2(0,cellDimensions.y+cellOffset.y+dimensions.y);
  glm::vec2 tr = origin+glm::vec2(-cellOffset.x+dimensions.x,cellDimensions.y);
  GUI::drawQuad(bl,tr,glm::vec4(0.3,0.6,0.3,0.5));
}
bool ImageGrid::isIn(const glm::vec2 pos)
{
  glm::vec2 bottomLeft = origin+glm::vec2(0,cellDimensions.y+cellOffset.y+dimensions.y);
  glm::vec2 topRight = origin+glm::vec2(-cellOffset.x+dimensions.x,cellDimensions.y);
  bool horzIn = pos.x > bottomLeft.x && pos.x < topRight.x;
  bool vertIn = pos.y > bottomLeft.y && pos.y < topRight.y;
  return horzIn && vertIn;
}
void ImageGrid::handleMouseHover(const glm::vec2 &mousePos)
{
  for(int y=0;y<columns;y++)
  {
    for(int x=0;x<rows;x++)
    {
      glm::vec2 pos = glm::vec2(x,-y);
      glm::vec2 bl = origin + pos*(cellDimensions+cellOffset);
      glm::vec2 tr = origin + cellDimensions+ pos*(cellDimensions+cellOffset);
      bool horzIn = mousePos.x > bl.x && mousePos.x < tr.x;
      bool vertIn = mousePos.y > bl.y && mousePos.y < tr.y;
      //std::cout << horzIn << ":" << vertIn << "\n";
      if(horzIn && vertIn)
      {
        //std::cout << "Rendering\n";
        GUI::renderText(std::to_string(x+y*columns),mousePos+glm::vec2(0.01));
      }
    }
  }
}


WidgetList::WidgetList(const glm::vec2 &Origin) : Widget(Origin)
{
  selected = 0;
}

void WidgetList::addWidget(Widget* widget)
{
  widgetList.push_back(widget);
}

void WidgetList::draw()
{
  int i = 0;

  auto func = offsetFunction;
  if(offsetFunction == NULL)
  {
    offsetFunction = [](Widget* parent, int i)
    {
      return parent->getOrigin() + float(i)*glm::vec2(0,-0.1);
    };
  }
  func = [](Widget* parent,int i)
  {
    return parent->getOrigin() + glm::vec2(0,-1)*(float)i;
  };
  for(auto itr = widgetList.begin();itr != widgetList.end();itr++)
  {
    glm::vec2 widgetPos = offsetFunction(this,i);
    if(selected == i)
    {
      glm::vec2 cursorOrigin = widgetPos - glm::vec2(0.02,0);
      GUI::drawQuad(cursorOrigin, cursorOrigin + glm::vec2(0.02,(*itr)->getDimensions().y),glm::vec4(0,0,0,1),CIRCLEQUAD);
    }

    (*itr)->setOrigin(offsetFunction(this,i));
    (*itr)->draw();
    i++;
  }
}

void WidgetList::handleKeyInput(int key, int action)
{
  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    switch(key)
    {

      case(GLFW_KEY_S):
        selected++;
        if(selected >= widgetList.size()) selected = 0;
        break;
      case(GLFW_KEY_W):
        selected--;
        if(selected < 0) selected = widgetList.size()-1;
        break;
      case(GLFW_KEY_ENTER):
      {
        auto itr = widgetList.begin();
        for(int i=0;i<selected;i++)
        {
          itr++;
        }
        (*itr)->submitEvent();
        break;

      }
    }
  }

}
