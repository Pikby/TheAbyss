#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/shaders.h"
#include "headers/block.h"
#include "headers/openGL.h"



GLFWwindow* createWindow(int width, int height)
{
  //Initial windows configs
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  //Create the window
  GLFWwindow* window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  return window;
}

/*
void saveLevel(std::vector<WorldBlk>* level, const char* path)
{
  std::ofstream levelFile;
  levelFile.open(path);
  int numItems;
  numItems = level->size();
  levelFile << numItems;
  levelFile << "\n";
  for(int i = 0; i<numItems;i++)
  {
    levelFile << level->at(i).xpos*10 << "\n";
    levelFile << level->at(i).ypos*10 << "\n";
    levelFile << level->at(i).zpos*10 << "\n";
    levelFile << level->at(i).id << "\n";
  }
  levelFile.close();
}


std::vector <WorldBlk> loadLevel( const char* path)
{
  std::vector <WorldBlk> level;
  std::ifstream levelFile;
  levelFile.open(path);

  unsigned int numItems;
  levelFile >> numItems;

  //std::cout << "numItems is: " << numItems << "\n";
  for(int i=0;i<numItems;i++)
  {
    float xpos,ypos,zpos;
    char texture[100];

    levelFile >> xpos;
    levelFile >> ypos;
    levelFile >> zpos;
    levelFile >> id;

    //std::cout << "xPos is :" << xpos << " yPos is : " << ypos << texture << "\n";
    level.push_back(Block(xpos,ypos,zpos,texture,0));
  }


  levelFile.close();
  return level;
}

*/
