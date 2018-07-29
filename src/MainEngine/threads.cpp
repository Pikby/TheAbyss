//OpenGL libraries
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


//Global maincharacter reference which encapsulates the camera
static World* newWorld;
static GLFWwindow* window;
static MainChar* mainCharacter;




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
  newWorld = new World(numbBuildThreads,width,height);
  mainCharacter = new MainChar(0,50,0,newWorld);
  initializeInputs(mainCharacter);
}

void closeGame()
{
  glfwSetWindowShouldClose(window, true);
}


void draw()
{

  glfwMakeContextCurrent(window);
  float deltaTime;
  float lastFrame;
  newWorld->drawer.createDirectionalLight(glm::vec3(-0.2f,-1.0f,-0.3f));
  SkyBox skyBox;
  Camera* mainCam = &(mainCharacter->mainCam);

  Cube cube;
  cube.render();

  Player player;
  player.render();

  newWorld->drawer.addCube(glm::vec3(0,50,0));
  //newWorld->addLight(glm::vec3(10,50,10));
  while(!glfwWindowShouldClose(window))
  {

    updateInputs();
    //std::cout << newWorld->drawnChunks << "\n";
    newWorld->drawnChunks = 0;
    newWorld->deleteChunksFromQueue();
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainCharacter->update();


    newWorld->drawer.updateCameraMatrices(mainCam);

    //if(std::stoi(Settings::get("unlockCamera")))
    {
      newWorld->calculateViewableChunks();
    }


    /*
    if(BSP::geometryChanged == true)
    {
      //std::cout << "renderingShadows\n";
      newWorld->drawer.renderDirectionalShadows();
      BSP::geometryChanged = false;
    }
    */

    newWorld->drawer.drawFinal();
    newWorld->drawer.drawObjects();
    newWorld->drawer.drawPlayers();

    int error = glGetError();
    if(error != 0)
    {
      std::cout << "OPENGL ERROR" << error << ":" << std::hex << error << std::dec << "\n";

    }

    glm::mat4 view = mainCam->getViewMatrix();
    skyBox.draw(&view);

    mainCharacter->drawHud();

    glfwSwapBuffers(window);

  }
  std::cout << "exiting draw thread \n";
}

void render()
{
  int renderLoop = 0;
  while(!glfwWindowShouldClose(window))
  {
    //std::cout << "finished render loop\n";
    newWorld->renderWorld(mainCharacter->xpos,mainCharacter->ypos,mainCharacter->zpos);
    //std::cout << "Finished render loop" << renderLoop << "\n";
  }
  std::cout << "exiting render thread \n";

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
    newWorld->delScan(mainCharacter->xpos,mainCharacter->ypos,mainCharacter->zpos);
  }
  std::cout << "exiting delete thread \n";
}

void logic()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 20;
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

    //DO the game logic
    newWorld->messenger.createMoveRequest(mainCharacter->xpos,mainCharacter->ypos,mainCharacter->zpos);
  }
}

void send()
{
  while(!glfwWindowShouldClose(window))
  {
    //newWorld->messageQueue.waitForData();
    if(newWorld->messenger.messageQueue.empty()) continue;
    Message msg = newWorld->messenger.messageQueue.front();
    newWorld->messenger.messageQueue.pop();
    uchar opcode = msg.opcode;
    //std::cout << std::dec <<  "Opcode is: " << (int)msg.opcode << ":" << (int)msg.ext1 << "\n";
    switch(opcode)
    {
      case(0):
        newWorld->messenger.requestChunk(msg.x.i,msg.y.i,msg.z.i);
        break;
      case(1):
        newWorld->messenger.requestDelBlock(msg.x.i,msg.y.i,msg.z.i);
        break;
      case(2):
        newWorld->messenger.requestAddBlock(msg.x.i,msg.y.i,msg.z.i,msg.ext1);
        break;
      case(91):
        newWorld->messenger.requestMove(msg.x.f,msg.y.f,msg.z.f);
        break;
      default:
        std::cout << "Sending unknown opcode" << std::dec << (int)msg.opcode << "\n";

    }
  }
  newWorld->messenger.requestExit();
  std::cout << "exiting server send thread\n";
}

void receive()
{
  while(!glfwWindowShouldClose(window))
  {
    Message msg;
    try
    {
      msg = newWorld->messenger.receiveAndDecodeMessage();
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
          newWorld->messenger.receiveMessage(buf,msg.length);
          std::thread chunkThread(&World::generateChunkFromString,newWorld,msg.x.i,msg.y.i,msg.z.i,buf);
          //newWorld->generateChunkFromString(msg.x.i,msg.y.i,msg.z.i,std::string(buf,msg.length));
          chunkThread.detach();
          //delete[] buf;
        }
        break;
      case(1):
        newWorld->delBlock(msg.x.i,msg.y.i,msg.z.i);
        break;
      case(2):
        newWorld->addBlock(msg.x.i,msg.y.i,msg.z.i,msg.ext1);
        break;
      case(10):
        //mainCharacter->(msg.x,msg.y,msg.z);
        break;
      case(90):
        newWorld->addPlayer(msg.x.f,msg.y.f,msg.z.f,msg.ext1);
        break;
      case(91):
        if(msg.ext1 == newWorld->mainId)
        {
          mainCharacter->setPosition(msg.x.f,msg.y.f,msg.z.f);
        }
        else newWorld->movePlayer(msg.x.f,msg.y.f,msg.z.f,msg.ext1);
        break;
      case(99):
        newWorld->removePlayer(msg.ext1);
        break;
      case(100):
        {
          char* buf = new char[msg.length];
          newWorld->messenger.receiveMessage(buf,msg.length);
          std::string line = "[Server]: ";
          line.append(buf);
          mainCharacter->addChatLine(line);
          delete[] buf;
        }
        break;
      case(0xFF):
        std::cout << "Received exit message\n";
        glfwSetWindowShouldClose(window, true);
        break;
      default:
        std::cout << "Receiving unknown opcode " << (int)msg.opcode << "\n";
    }
  }
  std::cout << "Exiting receive thread \n";
}

void build(char threadNumb)
{
  while(!glfwWindowShouldClose(window))
  {
    newWorld->buildWorld(threadNumb);
  }
  std::cout << "exiting build thread #" << threadNumb << "\n";
}
