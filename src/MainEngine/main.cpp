#include "../headers/all.h"


int main()
{
  glfwInit();

  const int numbBuildThreads = 1;



  int winWidth = 1280;
  int winHeight = 720;
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
  pthread_create(&mainThreads[1],NULL,render,NULL);
  pthread_create(&mainThreads[2],NULL,del,NULL);
  pthread_create(&mainThreads[3],NULL,send,NULL);
  pthread_create(&mainThreads[4],NULL,receive,NULL);
  std::cout << "Render threads created \n";
  pthread_t buildThreads[numbBuildThreads];
  for(int i = 0;i<numbBuildThreads;i++)
  {
    int *x = new int;
    *x = i;
    pthread_create(&buildThreads[i],NULL,build,(void*)x );
  }

  std::cout << "World initalized begin drawing\n";
  draw();
  pthread_exit(NULL);

  glfwTerminate();
}
