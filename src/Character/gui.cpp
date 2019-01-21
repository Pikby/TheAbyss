#include <iostream>
#include <map>
#include <string>
#define GLEW_STATIC

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../headers/shaders.h"
#include "../headers/camera.h"
#include "include/gui.h"


GUIRenderer::GUIRenderer(int Width, int Height, std::string userName)
{


}

void GUIRenderer::openInventoryGUI()
{

}

void GUIRenderer::closeInventoryGUI()
{
}

void GUIRenderer::createMenuScreens()
{
}

void GUIRenderer::initResourcePaths()
{
}

char keyToASCII(int key,bool shiftHeld)
{
  char c = key;
  if(shiftHeld)
  {
    switch(key)
    {
      case '`': c = '~'; break;
      case '1': c = '!'; break;
      case '2': c = '@'; break;
      case '3': c = '#'; break;
      case '4': c = '$'; break;
      case '5': c = '%'; break;
      case '6': c = '^'; break;
      case '7': c = '&'; break;
      case '8': c = '*'; break;
      case '9': c = '('; break;
      case '0': c = ')'; break;
      case '-': c = '_'; break;
      case '=': c = '+'; break;
      case '[': c = '{'; break;
      case ']': c = '}'; break;
      case '\\': c = '|'; break;
      case ';': c = ':'; break;
      case '\'': c = '\"'; break;
      case ',': c = '<'; break;
      case '/': c = '?'; break;
    }
  }
  else
  {
    switch(key)
    {
      case 65 ... 90: c += 32; break;
    }
  }
  return c;
}
