#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "widgets.h"
#include "../include/gui.h"


ChatBox::ChatBox(const glm::vec2 &newOrigin,const glm::vec2 &dimensions,double newSize) : Widget(newOrigin,dimensions),characterScale(newSize)
{
  padding = 0.005;
  focusable = true;
  focused = false;
  focusTarget = history.begin();

  inputBox.setColor(glm::vec4(glm::vec3(0),1));
  auto submit = [&](std::string str)
  {
    addLineToHistory( GUI::username + ":" + str);
  };

  inputBox = EditBox("",origin,glm::vec2(dimensions.x,(characterScale*64)/GUI::dimensions.y),characterScale,submit);
}

void ChatBox::draw()
{
  inputBox.setColor(glm::vec4(glm::vec3(0),1));
  inputBox.draw();
  GUI::drawQuad(origin-glm::vec2(padding),origin+dimensions+glm::vec2(padding),glm::vec4(0.5));
  double curTime = glfwGetTime();
  bool extended = curTime < extendTime;
  glm::vec2 offset = glm::vec2(0.0f,(64.0f/GUI::dimensions.y)*characterScale);
  int numbOfMainLines = ceil((dimensions.y/offset.y));
  int linesToDraw;
  if(extended)
  {
    linesToDraw = ceil((1-origin.y)/offset.y);
  }
  else
  {
    linesToDraw = numbOfMainLines;
  }
  auto target = focusTarget;
  int i = 2 ;
  while(target != history.end() && i < linesToDraw)
  {
    double alpha = 1;
    if(extended && extendTime - curTime < 1 && i >= numbOfMainLines)
    {
      alpha = extendTime-curTime;
    }

    std::string originText = (*target);


    std::vector<std::string> lines = GUI::textRenderer.splitString(originText,characterScale,dimensions.x*GUI::dimensions.x);
    for(auto itr = lines.rbegin(); itr != lines.rend();itr++)
    {
      std::string displayText = *itr;
      GUI::renderText(displayText,origin + (float)i*offset,characterScale,glm::vec4(glm::vec3(color),alpha));
      i++;
    }
    target++;


  }
}
void ChatBox::addLineToHistory(const std::string &line)
{
  history.push_front(line);
  focusTarget = history.begin();
  extendFor(5);
}

void ChatBox::handleCharInput(uint character)
{
  extendFor(5);
  inputBox.handleCharInput(character);
}
void ChatBox::extendFor(double seconds)
{
  extendTime = glfwGetTime()+seconds;
}
void ChatBox::handleScrollInput(double xoffset, double yoffset)
{
  extendFor(5);
  if(yoffset > 0 && focusTarget != history.end())
  {
    focusTarget++;
  }
  if(yoffset < 0 && focusTarget != history.begin())
  {
    focusTarget--;
  }
}

void ChatBox::handleKeyInput(int key, int action)
{
  inputBox.handleKeyInput(key,action);
  if(key == GLFW_KEY_ENTER && action == GLFW_PRESS) inputBox.updateText("");
}
