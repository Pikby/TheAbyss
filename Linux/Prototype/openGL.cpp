#include <string>
#include "SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "openGL.h"

GLFWwindow* createWindow(int width, int height)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


  GLFWwindow* window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);
  if(window == nullptr)
  {
    glfwTerminate();
    return nullptr;
  }

  return window;
}
