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
#include "../headers/world.h"
#include "../headers/mainchar.h"
#include "../headers/inputhandling.h"
#include "../headers/objects.h"

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

  int error = glGetError();
  if(error != 0)
  {
    std::cout << "OPENGL ERRORa" << error << ":" << std::hex << error << "\n";
    //glfwSetWindowShouldClose(window, true);
    //return;
  }
  return window;
}

void initWorld(int numbBuildThreads, int width,  int height)
{
  //glfwMakeContextCurrent(window);
  newWorld = new World(numbBuildThreads,width,height);
  mainCharacter = new MainChar(10,50,10,newWorld);
  initializeInputs(mainCharacter);

  //newWorld->requestChunk(0,0,0);
  /*
  for(int x =0;x<20;x++)
    for(int y =0;y<20;y++)
      for(int z=0;z<20;z++)newWorld->requestChunk(x-10,y-10,z-10);
      */
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
  newWorld->createDirectionalLight(glm::vec3(0,-1.0f,0));
  SkyBox skyBox;
  Camera* mainCam = &(mainCharacter->mainCam);
  {
  int error = glGetError();
  if(error != 0)
  {
    std::cout << "OPENGL ERRORb" << error << ":" << std::hex << error << "\n";
    //glfwSetWindowShouldClose(window, true);
    //return;
  }}
  while(!glfwWindowShouldClose(window))
  {
    updateInputs();
    //std::cout << newWorld->drawnChunks << "\n";
    newWorld->drawnChunks = 0;

	   // update the delta time each frame
	 float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //Create a time for every second and displays the FPS

    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainCharacter->update();

    glm::mat4 view = mainCam->getViewMatrix();
    newWorld->updateViewMatrix(view,mainCam->getHSRMatrix(),mainCam->getPosition());

    glViewport(0,0,1920,1080);

    //glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0,0,1920,1080);
    newWorld->drawTerrain();
    //newWorld->drawObjects();

    int error = glGetError();
    if(error != 0)
    {
      std::cout << "OPENGL ERROR" << error << ":" << std::hex << error << "\n";
      glfwSetWindowShouldClose(window, true);
      return;
    }
    skyBox.draw(&view);
    mainCharacter->drawHud();


    glfwSwapBuffers(window);



    //std::cout << newWorld->drawnChunks << "\n";
  }
  std::cout << "exiting draw thread \n";
}

void render()
{
  int renderLoop = 0;
  while(!glfwWindowShouldClose(window))
  {
    //std::cout << "finished render loop\n";
    newWorld->renderWorld(&mainCharacter->xpos,&mainCharacter->ypos,&mainCharacter->zpos);
    //std::cout << "Finished render loop" << renderLoop << "\n";
  }
  newWorld->saveWorld();
  std::cout << "exiting render thread \n";

}

void del()
{
  while(!glfwWindowShouldClose(window))
  {
    //std::cout << "finished delete scan\n";
    newWorld->delScan(&mainCharacter->xpos,&mainCharacter->ypos,&mainCharacter->zpos);
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
    newWorld->createMoveRequest(mainCharacter->xpos,mainCharacter->ypos,mainCharacter->zpos);
  }
}

void send()
{
  while(!glfwWindowShouldClose(window))
  {
    if(newWorld->messageQueue.empty()) continue;
    newWorld->msgQueueMutex.lock();
    Message msg = newWorld->messageQueue.front();
    newWorld->messageQueue.pop();
    newWorld->msgQueueMutex.unlock();
    uchar opcode = msg.opcode;
    switch(opcode)
    {
      case(0):
        newWorld->requestChunk(msg.x,msg.y,msg.z);
        break;
      case(1):
        newWorld->requestDelBlock(msg.x,msg.y,msg.z);
        break;
      case(2):
        newWorld->requestAddBlock(msg.x,msg.y,msg.z,msg.ext1);
        break;
      case(91):
        newWorld->requestMove(*(float*)&msg.x,*(float*)&msg.y,*(float*)&msg.z);
        break;
      default:
        std::cout << "Sending unknown opcode" << (int)msg.opcode << "\n";

    }
  }
  newWorld->requestExit();
  std::cout << "exiting server send thread\n";
}

void receive()
{
  while(!glfwWindowShouldClose(window))
  {
    Message msg = newWorld->receiveAndDecodeMessage();
  //std::cout << "Opcode is: " << (int)msg.opcode << ":" << (int)msg.ext1 << "\n";

    switch(msg.opcode)
    {
      case(0):
        newWorld->receiveChunk(msg.x,msg.y,msg.z,msg.length);
        break;
      case(1):
        newWorld->delBlock(msg.x,msg.y,msg.z);
        break;
      case(2):
        newWorld->addBlock(msg.x,msg.y,msg.z,msg.ext1);
        break;
      case(10):
        //mainCharacter->(msg.x,msg.y,msg.z);
        break;
      case(90):
        newWorld->addPlayer(*(float*)&msg.x,*(float*)&msg.y,*(float*)&msg.z,msg.ext1);
        break;
      case(91):
        if(msg.ext1 == newWorld->mainId)
        {
          mainCharacter->setPosition(*(float*)&msg.x,*(float*)&msg.y,*(float*)&msg.z);
        }
        else newWorld->movePlayer(*(float*)&msg.x,*(float*)&msg.y,*(float*)&msg.z,msg.ext1);
        break;
      case(99):
        newWorld->removePlayer(msg.ext1);
        break;
      case(0xFF):
        std::cout << "Received exit message\n";
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
