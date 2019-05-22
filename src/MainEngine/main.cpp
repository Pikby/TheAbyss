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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  if(severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
  {
    return;
  }
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}



int main()
{
  glfwInit();
  const int numbBuildThreads = 2;
  int winWidth = std::stoi(Settings::get("windowWidth"));
  int winHeight = std::stoi(Settings::get("windowHeight"));

  const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  winWidth = mode->width;
  winHeight = mode->height;

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  GLFWwindow* window = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, GUI::GLFWKeyCallBack);
  glfwSetCharCallback(window, GUI::GLFWCharCallBack);
  glfwSetCursorPosCallback(window, GUI::GLFWCursorCallback);
  glfwSetMouseButtonCallback(window, GUI::GLFWMouseButtonCallback);
  glfwSetScrollCallback(window, GUI::GLFWScrollCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.WantCaptureMouse = true;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");


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
  GUI::initGUI(glm::vec2(winWidth,winHeight));

  try
  {
      initWorld(numbBuildThreads,winWidth,winHeight);
  }
  catch(const char* err)
  {
    std::cout << err << "\n";
    return -1;
  }
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
  //std::thread buildThread2(build,1);
  //buildThread2.detach();
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
