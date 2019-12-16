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

#define THREADIMPLEMENTATION
#include "include/threads.h"
#include "include/world.h"
#include "include/drawer.h"
#include "include/messenger.h"


#include "../Character/include/mainchar.h"
#include "../Character/include/gui.h"
#include "../Character/Widgets/widgets.h"

#include "../Objects/include/objects.h"
#include "../Settings/settings.h"


World PlayerWorld;


GLFWwindow* ThreadHandler::createWindow(int width, int height)
{
  //Initial windows configs


  glfwWindowHint(GLFW_SAMPLES, 1);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //Create the window
  window = glfwCreateWindow(width, height, "The Abyss", glfwGetPrimaryMonitor(), nullptr);
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

void ThreadHandler::enableCursor()
{
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void ThreadHandler::disableCursor()
{
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void ThreadHandler::endThreads()
{
  threadsOn=false;
}

void ThreadHandler::dispatchThreads()
{
  threadsOn = true;
  renderThread = std::thread(render);
  deleteThread = std::thread(del);
  sendThread = std::thread(send);
  receiveThread = std::thread(receive);
  logicThread = std::thread(logic);

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
}


void ThreadHandler::initWorld(const std::string &ip, const std::string &userName)
{

  PlayerWorld.connectToServer(ip,userName);
  std::cout << "Connected\n";
  PlayerWorld.initWorld();
  MainChar::initMainChar(glm::vec3(0,50,0));
}

void ThreadHandler::closeGame()
{
  glfwSetWindowShouldClose(window, true);
  PlayerWorld.buildQueue.notify_one();
  PlayerWorld.chunkDeleteQueue.notify_one();
}


void ThreadHandler::draw()
{

  glfwMakeContextCurrent(window);



  float deltaTime;
  float lastFrame;
  //PlayerWorld.drawer->createDirectionalLight(glm::vec3(-0.0f,-1.0f,-0.0001f),glm::vec3(0.8f,0.8f,0.8f));
  PlayerWorld.drawer->updateDirectionalLight(glm::vec3(-0.5,-1.0f,-0.5f),glm::vec3(0.2f),glm::vec3(0.5f),glm::vec3(1.0f));

  PlayerWorld.drawer->addCube(glm::vec3(0,50,0));
  PlayerWorld.drawer->addCube(glm::vec3(0,0,0));

  PlayerWorld.drawer->addLight(glm::vec3(0,50,0),glm::vec3(1.0f,1.0f,1.0f));
  PlayerWorld.drawer->addLight(glm::vec3(0,0,0),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.5f,0.5f),1.0,0.045,0.0075);
  bool show_demo_window = true;
  bool show_another_window = false;
  //PlayerWorld.addLight(glm::vec3(10,50,10));
  glm::vec3 terrainColor = glm::vec3(1.0f);
  float sunX=0.01,sunZ=0.01;
  bool shadowsOn = true;
  while(!glfwWindowShouldClose(window) && threadsOn)
  {

    static const char* current_item = "1.0";
    static const char* old_item = "1.0";
    if(current_item!=old_item)
    {
      old_item = current_item;
      std::cout << "Updating to " << std::stof(current_item) << "\n";
      //PlayerWorld.drawer->updateAntiAliasing(std::stof(current_item));
    }
    PlayerWorld.drawer->setTerrainColor(terrainColor);
    glm::vec3 dirLight(0.1,-1.0f,0.1);
    glfwPollEvents();
    MainChar::update();

    PlayerWorld.drawer->updateDirectionalLight(dirLight,glm::vec3(0.8f,0.8f,0.8f));

    PlayerWorld.drawnChunks = 0;
    PlayerWorld.drawer->chunksToDraw = NULL;
    PlayerWorld.deleteChunksFromQueue();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //PlayerWorld.drawer->updateCameraMatrices(MainChar::getCamera());

    float findstart = glfwGetTime();
    PlayerWorld.calculateViewableChunks();
    float findend = glfwGetTime();

    float directstart = glfwGetTime();
    //if(shadowsOn) PlayerWorld.drawer->renderDirectionalShadows();
    float directend = glfwGetTime();
    PlayerWorld.drawer->renderGBuffer();
    //glDisable(GL_BLEND);
    PlayerWorld.drawer->drawFinal();
    GUI::drawGUI();
    glfwSwapBuffers(window);
  }
  PlayerWorld.buildQueue.notify_one();
  std::cout << "Draw thread done\n";
}

void ThreadHandler::render()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 27000;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    lastFrame = currentFrame;
    currentFrame = glfwGetTime();
    //std::cout << currentFrame << ':' << lastFrame << "\n";
    double deltaFrame = currentFrame-lastFrame;

    int waitTime = (tickRate-deltaFrame)*1000;
    //std::cout << deltaFrame << ":" << waitTime ;
    //std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    currentFrame = glfwGetTime();
    PlayerWorld.renderWorld(MainChar::getPosition());
  }


}

void ThreadHandler::del()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 0.2f;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    lastFrame = currentFrame;
    currentFrame = glfwGetTime();
    //std::cout << currentFrame << ':' << lastFrame << "\n";
    double deltaFrame = currentFrame-lastFrame;

    int waitTime = (tickRate-deltaFrame)*1000;
    //std::cout << deltaFrame << ":" << waitTime ;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    currentFrame = glfwGetTime();
    PlayerWorld.delScan(MainChar::getPosition());
  }
  std::cout << "Delete Thread Done\n";
}

void ThreadHandler::logic()
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 60;
  double tickRate = 1.0f/ticksPerSecond;
  while(!glfwWindowShouldClose(window) && threadsOn)
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





    static double lastPingTime = 0;

    if(glfwGetTime() - lastPingTime > 1)
    {
      PlayerWorld.messenger->createPingRequest();
      lastPingTime = glfwGetTime();
    }

  }
  std::cout << "Ending logic thread\n";
}

void ThreadHandler::send()
{
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    PlayerWorld.messenger->messageQueue.waitForData();
    while(!PlayerWorld.messenger->messageQueue.empty())
    {
      OutMessage msg = PlayerWorld.messenger->messageQueue.front();
      PlayerWorld.messenger->messageQueue.pop();
      uint8_t opcode = msg.opcode;
      switch(opcode)
      {
        case(0):
          PlayerWorld.messenger->requestChunk(msg.x.i,msg.y.i,msg.z.i);
          break;
        case(1):
          PlayerWorld.messenger->requestDelBlock(msg.x.i,msg.y.i,msg.z.i);
          break;
        case(2):
          PlayerWorld.messenger->requestAddBlock(msg.x.i,msg.y.i,msg.z.i,msg.ext1);
          break;
        case(91):
          PlayerWorld.messenger->requestMove(msg.x.f,msg.y.f,msg.z.f);
          break;
        case(92):
          PlayerWorld.messenger->requestViewDirectionChange(msg.x.f,msg.y.f,msg.z.f);
          break;
        case(100):
          PlayerWorld.messenger->sendChatMessage(msg.data);
          break;
        case(250):
          PlayerWorld.messenger->pingStart();
          break;
        default:
          std::cout << "Sending unknown opcode" << std::dec << (int)msg.opcode << "\n";

      }
    }
  }
  PlayerWorld.messenger->requestExit();
  std::cout << "Send Thread Done\n";
}

void ThreadHandler::receive()
{
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    InMessage msg;
    try
    {
      msg = PlayerWorld.messenger->receiveAndDecodeMessage();
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
          PlayerWorld.messenger->receiveMessage(buf,msg.length);
          //std::thread chunkThread(&PlayerWorld.generateChunkFromString,newWorld,msg.x.i,msg.y.i,msg.z.i,buf);
          //chunkThread.detach();

          PlayerWorld.generateChunkFromString(glm::ivec3(msg.x.i,msg.y.i,msg.z.i),buf);
        }
        break;
      case(1):
        PlayerWorld.delBlock(glm::ivec3(msg.x.i,msg.y.i,msg.z.i));
        break;
      case(2):
        PlayerWorld.addBlock(glm::ivec3(msg.x.i,msg.y.i,msg.z.i),msg.ext1);
        break;
      case(10):
        //MainChar::(msg.x,msg.y,msg.z); && threadsOn
        break;
      case(90):
        PlayerWorld.addPlayer(glm::vec3(msg.x.f,msg.y.f,msg.z.f),msg.ext1);
        break;
      case(91):
        //std::cout << "Moving player" << msg.x.f << msg.y.f << msg.z.f << "\n";
        if(msg.ext1 == PlayerWorld.mainId)
        {
          std::cout << "Moving" << msg.x.f << ":" << msg.y.f << ":" << msg.z.f << "\n";
          MainChar::setPosition(glm::vec3(msg.x.f,msg.y.f,msg.z.f));
        }
        else PlayerWorld.movePlayer(glm::vec3(msg.x.f,msg.y.f,msg.z.f),msg.ext1);
        break;
      case(92):
        if(msg.ext1 == PlayerWorld.mainId)
        {

        }
        else PlayerWorld.updatePlayerViewDirection(glm::vec3(msg.x.f,msg.y.f,msg.z.f),msg.ext1);
        break;

      case(99):
        PlayerWorld.removePlayer(msg.ext1);
        break;
      case(100):
        {
          char* buf = new char[msg.length + 1];
          buf[msg.length] = 0;
          PlayerWorld.messenger->receiveMessage(buf,msg.length);
          std::string line = "(Server): ";
          line.append(buf);
          std::cout << line << "\n";
          GUI::chatBox->addLineToHistory(line);
          delete[] buf;
        }
        break;
      case(101):
        {
          char* buf = new char[msg.length + 1];
          buf[msg.length] = 0;
          PlayerWorld.messenger->receiveMessage(buf,msg.length);
          std::string line = std::string(buf,msg.length);
          std::cout << line;
          GUI::chatBox->addLineToHistory(line);
          delete[] buf;
        }
        break;

     case(250):
       {
         PlayerWorld.worldStats.pingInMS =  PlayerWorld.messenger->pingEnd();
         break;
       }
      case(255):
        std::cout << "Received exit message\n";
        //closeGame();
        break;
      default:
        std::cout << "Receiving unknown opcode " << (int)msg.opcode << "\n";
    }
  }
  std::cout << "RECEIVE THREAD DONE\n";
}

void ThreadHandler::build(char threadNumb)
{
  while(!glfwWindowShouldClose(window)  && threadsOn)
  {
    PlayerWorld.buildWorld(threadNumb);
  }
  std::cout << "Ending build thread\n";
}
