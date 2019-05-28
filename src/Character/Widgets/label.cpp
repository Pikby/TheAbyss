#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "widgets.h"
#include "../include/gui.h"
Label::Label(const std::string& text,const glm::vec2& Origin,double CharacterScale, TextAlignment ali)
                         : label(text), Widget(Origin), characterScale(CharacterScale), textAlignment(ali)
{
}

void Label::draw()
{
  if(onDrawFunction != NULL) onDrawFunction(this);
  GUI::renderText(label,origin,characterScale,color,rotMat,textAlignment);
}

void Label::updateLabel(const std::string& str)
{
  label = str;
}

EditBox::EditBox(const std::string& text,const glm::vec2& Origin,const glm::vec2& Dims,double CharacterScale,std::function<void(std::string)> SubmitHandler)
                         : defaultText(text), Widget(Origin,Dims), characterScale(CharacterScale), submitHandler(SubmitHandler)
{
}

void EditBox::handleCharInput(uint character)
{
  text.insert(cursorPosition,(char*)&character);
  cursorPosition++;
}

void EditBox::handleKeyInput(int key,int action)
{
  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    switch(key)
    {
      case(GLFW_KEY_BACKSPACE):
        if(cursorPosition > 0) text.erase(--cursorPosition,1);
      break;

      case(GLFW_KEY_ENTER):
        if(submitHandler != NULL) submitHandler(text);
      break;

      case(GLFW_KEY_LEFT):
        if(cursorPosition>0) cursorPosition--;
      break;

      case(GLFW_KEY_RIGHT):
        if(cursorPosition<text.size()) cursorPosition++;
      break;

      case(GLFW_KEY_DELETE):
        if(cursorPosition<text.size()) text.erase(cursorPosition,1);
      break;
    }
  }
}

void EditBox::draw()
{
  int stringSize = text.size();
  bool hasCursor = ((int)round(glfwGetTime()*2)) % 2;

  if(text == "")
  {
    int displayCursorPosition = hasCursor && focused ? 0 : -1;
    GUI::renderText(defaultText+ ' ',origin,characterScale,glm::vec4(glm::vec3(color),1.0),glm::mat3(1),TEXTALILEFT,displayCursorPosition);
  }
  else
  {
    std::string displayText = "";
    int charCount = 0;
    int totalCharCount = 2*(dimensions.x/((64.0*characterScale)/GUI::dimensions.x));

    int positiveDir = 0;
    int negativeDir = 1;

    while(GUI::calculateStringDimensions(displayText,characterScale).x < dimensions.x*GUI::dimensions.x)
    {
      if(stringSize > cursorPosition + charCount)
      {
        displayText.push_back(text[cursorPosition+positiveDir]);
        positiveDir++;
      }
      else
      {
        displayText.insert(0,(char*)&text[cursorPosition-negativeDir],1);
        negativeDir++;
      }
      charCount++;
      if(stringSize < charCount) break;
    }

    int displayCursorPosition = negativeDir-1;
    if(hasCursor && focused) GUI::renderText(displayText+' ',origin,characterScale,glm::vec4(glm::vec3(color),1.0),glm::mat3(1),TEXTALILEFT,displayCursorPosition);
    else GUI::renderText(displayText+' ',origin,characterScale,glm::vec4(glm::vec3(color),1.0),glm::mat3(1),TEXTALILEFT,-1);
  }
  glDepthMask(GL_FALSE);
    GUI::drawQuad(origin-glm::vec2(padding),origin+dimensions+glm::vec2(padding),glm::vec4(glm::vec3(0.5),1));
    GUI::drawQuad(origin-glm::vec2(padding/2),origin+dimensions+glm::vec2(padding/2),glm::vec4(glm::vec3(0.3),1));
  glDepthMask(GL_TRUE);
}
