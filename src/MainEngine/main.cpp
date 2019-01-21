#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>
#include "include/threads.h"
#include "../InputHandling/include/inputhandling.h"
#include "../Settings/settings.h"


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

// During init, enable debug output


int main()
{
  glfwInit();
  const int numbBuildThreads = 2;
  int winWidth = std::stoi(Settings::get("windowWidth"));
  int winHeight = std::stoi(Settings::get("windowHeight"));
  GLFWwindow* newWindow = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(newWindow);
  glfwSetKeyCallback(newWindow, key_callback);
  glfwSetCursorPosCallback(newWindow, mouse_callback);
  glfwSetScrollCallback(newWindow, scroll_callback);
  glfwSetMouseButtonCallback(newWindow, mousekey_callback);
  glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.WantCaptureMouse = true;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(newWindow, true);
  ImGui_ImplOpenGL3_Init("#version 330");


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

  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );
  //Enable antialiasing
  //glEnable(GL_MULTISAMPLE);

  //Cull face unrenders the back side of polygons
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);


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
