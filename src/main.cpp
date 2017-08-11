enum Type {STATIC,DYNAMIC,STREAM};

#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <pthread.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/openGL.h"




int main()
{
  std::cout << -16 % 16 << "\n";
  glfwInit();

  int winWidth = 1920;
  int winHeight = 1080;
  GLFWwindow* newWindow = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(newWindow);
  glfwSetKeyCallback(newWindow, key_callback);
  glfwSetCursorPosCallback(newWindow, mouse_callback);
  glfwSetScrollCallback(newWindow, scroll_callback);
  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings

  glewExperimental = GL_TRUE;
  glewInit();

  glViewport(0, 0, winWidth, winHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //Cull face unrenders the back side of polygons
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  initWorld();
  std::cout << "staring main\n";
  pthread_t threads[1];

  pthread_create(&threads[1],NULL,render,NULL);

  std::cout << "World initalized begin drawing\n";
  draw(NULL);

  pthread_exit(NULL);

}
