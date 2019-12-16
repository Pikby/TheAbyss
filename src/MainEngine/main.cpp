#define GLEW_STATIC


#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

#include "include/threads.h"
#include "../Settings/settings.h"
#include "../Character/include/gui.h"
#include "../headers/shaders.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                GLsizei length, const GLchar* message,const void* userParam )
{
  if(severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );

}



int main()
{
  Settings::initSettings();

  glfwInit();
  const int numbBuildThreads = 2;

  const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int winWidth = mode->width;
  int winHeight = mode->height;
  Settings::set("windowWidth",std::to_string(winWidth));
  Settings::set("windowHeight",std::to_string(winHeight));

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  GLFWwindow* window = ThreadHandler::createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, GUI::GLFWKeyCallBack);
  glfwSetCharCallback(window, GUI::GLFWCharCallBack);
  glfwSetCursorPosCallback(window, GUI::GLFWCursorCallback);
  glfwSetMouseButtonCallback(window, GUI::GLFWMouseButtonCallback);
  glfwSetScrollCallback(window, GUI::GLFWScrollCallback);


  glEnable(GL_MULTISAMPLE);
  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings


  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0, 0, winWidth, winHeight);




  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  //Cull face unrenders the back side of polygons
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  Shader::setShaderDirectory("../src/Shaders/");
  GUI::initGUI(glm::vec2(winWidth,winHeight),Settings::get("userName"));

  Settings::print();
  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GUI::drawGUI();
    glfwSwapBuffers(window);
  }
  glfwTerminate();
  Settings::save();
}
