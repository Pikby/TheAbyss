//OpenGL libraries
#include <CEGUI/CEGUI.h>
#define GLEW_STATIC
#define PI 3.14159265

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "../headers/shaders.h"
#include "include/world.h"
#include "../Character/include/mainchar.h"
#include "../InputHandling/include/inputhandling.h"
#include "../Objects/include/objects.h"
#include "../Settings/settings.h"



static GLFWwindow* window;




GLFWwindow* createWindow(int width, int height)
{
  //Initial windows configs

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //Create the window
  window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);
  if(window == NULL)
  {
    std::cout << "Window creation failed\n";
  }
  int error = glGetError();
  if(error != 0)
  {
    std::cout << "OPENGL ERRORa" << error << ":" << std::hex << error << "\n";
  }
  return window;
}

void initWorld(int numbBuildThreads, int width,  int height)
{
  World::initWorld(numbBuildThreads,width,height);
  MainChar::initMainChar(0,50,0);
}

void closeGame()
{
  glfwSetWindowShouldClose(window, true);
  World::buildQueue.notify_one();
  World::chunkDeleteQueue.notify_one();
}


void draw()
{

  glfwMakeContextCurrent(window);
  float deltaTime;
  float lastFrame;
  //World::drawer.createDirectionalLight(glm::vec3(-0.0f,-1.0f,-0.0001f),glm::vec3(0.8f,0.8f,0.8f));
  World::drawer.updateDirectionalLight(glm::vec3(-0.5,-1.0f,-0.5f),glm::vec3(0.8f,0.8f,0.8f));
  SkyBox skyBox("../assets/alps");
  Camera* mainCam = &(MainChar::mainCam);

  Cube cube;
  cube.render();

  Player player;
  player.render();

  World::drawer.addCube(glm::vec3(0,0,0));
  World::drawer.addCube(glm::vec3(0,50,0));
  World::drawer.addCube(glm::vec3(0,70,0));
  World::drawer.addCube(glm::vec3(0,100,0));

  //World::addLight(glm::vec3(10,50,10));
  while(!glfwWindowShouldClose(window))
  {
    static glm::vec3 dirLight(0,-1.0f,0);
    dirLight += glm::vec3(0.0001f,0,0.0001f);

    World::drawer.updateDirectionalLight(dirLight,glm::vec3(0.8f,0.8f,0.8f));

    World::drawnChunks = 0;
    World::drawer.chunksToDraw = NULL;
    World::deleteChunksFromQueue();
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    MainChar::update();

    World::drawer.updateCameraMatrices(mainCam);
    World::calculateViewableChunks();

    World::drawer.renderDirectionalShadows();
    World::drawer.renderGBuffer();


    if(BSP::geometryChanged == true)
    {


      BSP::geometryChanged = false;
    }

    World::drawer.drawFinal();

    glEnable(GL_BLEND);
    glm::mat4 view = mainCam->getViewMatrix();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CEGUI::System::getSingleton().renderAllGUIContexts();
    glfwSwapBuffers(window);

  }
  std::cout << "Draw thread done\n";
}

void render()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 27000;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window))
  {
    lastFrame = currentFrame;
    currentFrame = glfwGetTime();
    //std::cout << currentFrame << ':' << lastFrame << "\n";
    double deltaFrame = currentFrame-lastFrame;

    int waitTime = (tickRate-deltaFrame)*1000;
    //std::cout << deltaFrame << ":" << waitTime ;
    //std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    currentFrame = glfwGetTime();
    World::renderWorld(MainChar::xpos,MainChar::ypos,MainChar::zpos);
  }


}

void del()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 0.2f;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window))
  {
    lastFrame = currentFrame;
    currentFrame = glfwGetTime();
    //std::cout << currentFrame << ':' << lastFrame << "\n";
    double deltaFrame = currentFrame-lastFrame;

    int waitTime = (tickRate-deltaFrame)*1000;
    //std::cout << deltaFrame << ":" << waitTime ;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    currentFrame = glfwGetTime();
    World::delScan(MainChar::xpos,MainChar::ypos,MainChar::zpos);
  }

}

void logic()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 60;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window))
  {
    lastFrame = currentFrame;
    currentFrame = glfwGetTime();
    //std::cout << currentFrame << ':' << lastFrame << "\n";
    double deltaFrame = currentFrame-lastFrame;

    int waitTime = (tickRate-deltaFrame)*1000;
    //std::cout << deltaFrame << ":" << waitTime ;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    currentFrame = glfwGetTime();
    updateInputs();

    //DO the game logic
    World::messenger.createMoveRequest(MainChar::xpos,MainChar::ypos,MainChar::zpos);
  }
  std::cout << "Ending logic thread\n";
}

void send()
{
  while(!glfwWindowShouldClose(window))
  {
    World::messenger.messageQueue.waitForData();
    while(!World::messenger.messageQueue.empty())
    {
      OutMessage msg = World::messenger.messageQueue.front();
      World::messenger.messageQueue.pop();
      uchar opcode = msg.opcode;
      switch(opcode)
      {
        case(0):
          World::messenger.requestChunk(msg.x.i,msg.y.i,msg.z.i);
          break;
        case(1):
          World::messenger.requestDelBlock(msg.x.i,msg.y.i,msg.z.i);
          break;
        case(2):
          World::messenger.requestAddBlock(msg.x.i,msg.y.i,msg.z.i,msg.ext1);
          break;
        case(91):
          World::messenger.requestMove(msg.x.f,msg.y.f,msg.z.f);
          break;
        case(100):
          World::messenger.sendChatMessage(msg.data);
          break;
        default:
          std::cout << "Sending unknown opcode" << std::dec << (int)msg.opcode << "\n";

      }
    }
  }
  World::messenger.requestExit();
}

void receive()
{
  while(!glfwWindowShouldClose(window))
  {
    InMessage msg;
    try
    {
      msg = World::messenger.receiveAndDecodeMessage();
    }
    catch(...)
    {
      std::cout << "ERROR: RECEIVE MESSAGE, ERRNO: " << errno << "\n";
      return;
    }


    switch(msg.opcode)
    {
      case(0):
        {
          char* buf = new char[msg.length];
          World::messenger.receiveMessage(buf,msg.length);
          //std::thread chunkThread(&World::generateChunkFromString,newWorld,msg.x.i,msg.y.i,msg.z.i,buf);
          //chunkThread.detach();

          World::generateChunkFromString(glm::ivec3(msg.x.i,msg.y.i,msg.z.i),buf);
        }
        break;
      case(1):
        World::delBlock(glm::ivec3(msg.x.i,msg.y.i,msg.z.i));
        break;
      case(2):
        World::addBlock(glm::ivec3(msg.x.i,msg.y.i,msg.z.i),msg.ext1);
        break;
      case(10):
        //MainChar::(msg.x,msg.y,msg.z);
        break;
      case(90):
        World::addPlayer(glm::vec3(msg.x.f,msg.y.f,msg.z.f),msg.ext1);
        break;
      case(91):
        std::cout << "Moving player" << msg.x.f << msg.y.f << msg.z.f << "\n";
        if(msg.ext1 == World::mainId)
        {
          MainChar::setPosition(glm::vec3(msg.x.f,msg.y.f,msg.z.f));
        }
        else World::movePlayer(glm::vec3(msg.x.f,msg.y.f,msg.z.f),msg.ext1);
        break;
      case(99):
        World::removePlayer(msg.ext1);
        break;
      case(100):
        {
          char* buf = new char[msg.length];
          World::messenger.receiveMessage(buf,msg.length);
          std::string line = "(Server): ";
          line.append(buf);
          MainChar::addChatLine(line);
          delete[] buf;
        }
        break;
      case(101):
        {
          char* buf = new char[msg.length];
          World::messenger.receiveMessage(buf,msg.length);
          std::string line = std::string(buf,msg.length);
          std::cout << line;
          MainChar::addChatLine(line);
          delete[] buf;
          break;
        }
      case(0xFF):
        std::cout << "Received exit message\n";
        closeGame();
        break;
      default:
        std::cout << "Receiving unknown opcode " << (int)msg.opcode << "\n";
    }
  }
  std::cout << "RECEIVE THREAD DONE\n";
}

void build(char threadNumb)
{
  while(!glfwWindowShouldClose(window))
  {
    World::buildWorld(threadNumb);
  }
  std::cout << "Ending build thread\n";
}
