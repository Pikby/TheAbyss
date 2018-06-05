#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>
#include "../headers/threads.h"

int main()
{
  glfwInit();

  const int numbBuildThreads = 1;

  int winWidth = 1280;
  int winHeight = 720;
  GLFWwindow* newWindow = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(newWindow);


  glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings

  glewExperimental = GL_TRUE;
  glewInit();

  glViewport(0, 0, winWidth, winHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //Cull face unrenders the back side of polygons


  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);


  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);



  initWorld(numbBuildThreads,winWidth,winHeight);
  std::cout << "staring main\n";
  pthread_t mainThreads[5];
  std::thread renderThread(render);
  std::thread delThread(del);
  std::thread sendThread(send);
  std::thread receiveThread(receive);

  renderThread.detach();
  delThread.detach();
  sendThread.detach();
  receiveThread.detach();
  std::cout << "Render threads created \n";
  /*
  pthread_t buildThreads[numbBuildThreads];
  for(int i = 0;i<numbBuildThreads;i++)
  {
    int *x = new int;
    *x = i;
    pthread_create(&buildThreads[i],NULL,build,(void*)x );
  }
  */
  std::thread buildThread(build,0);
  buildThread.detach();

  std::cout << "World initalized begin drawing\n";
  draw();


  glfwTerminate();
}
