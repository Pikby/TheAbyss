#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>
#include "include/threads.h"
#include "../InputHandling/include/inputhandling.h"
#include "../Settings/settings.h"
int main()
{
  glfwInit();
  const int numbBuildThreads = 1;
  int winWidth = std::stoi(Settings::get("windowWidth"));
  int winHeight = std::stoi(Settings::get("windowHeight"));
  GLFWwindow* newWindow = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(newWindow);
  glfwSetKeyCallback(newWindow, key_callback);
  glfwSetCursorPosCallback(newWindow, mouse_callback);
  glfwSetScrollCallback(newWindow, scroll_callback);
  glfwSetMouseButtonCallback(newWindow, mousekey_callback);
  glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings


  glewExperimental = GL_TRUE;
  glewInit();
  {
  int error = glGetError();
  if(error != 0)
  {
    std::cout << "Getting rid of glews error:" << error << ":" << std::hex << error << "\n";
    //glfwSetWindowShouldClose(window, true);
    //return;
  }}
  glViewport(0, 0, winWidth, winHeight);


  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //Cull face unrenders the back side of polygons


  //glfwWindowHint(GLFW_SAMPLES, 4);
  //glEnable(GL_MULTISAMPLE);


  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);


  initWorld(numbBuildThreads,winWidth,winHeight);
  std::cout << "staring main\n";

  std::thread renderThread(render);
  std::thread deleteThread(del);
  std::thread sendThread(send);
  std::thread receiveThread(receive);
  std::thread logicThread(logic);

  renderThread.detach();
  deleteThread.detach();
  sendThread.detach();
  receiveThread.detach();
  logicThread.detach();

  std::cout << "Render threads created \n";
  std::thread buildThread(build,0);
  buildThread.detach();
  /*
  pthread_t buildThreads[numbBuildThreads];
  for(int i = 0;i<numbBuildThreads;i++)
  {
    int *x = new int;
    *x = i;
    pthread_create(&buildThreads[i],NULL,build,(void*)x );
  }
*/
  std::cout << "World initalized begin drawing\n";
  draw();
  glfwTerminate();
}
