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

#include <bullet/btBulletDynamicsCommon.h>

#include "../headers/shaders.h"

#define THREADIMPLEMENTATION
#include "include/threads.h"
#include "include/world.h"
#include "include/drawer.h"
#include "include/messenger.h"
#include "include/timer.h"
#include "include/bsp.h"


#include "../Character/include/mainchar.h"
#include "../Character/include/gui.h"
#include "../Character/Widgets/widgets.h"

#include "../Objects/include/objects.h"
#include "../Settings/settings.h"


World PlayerWorld;


GLFWwindow* ThreadHandler::createWindow(const GLFWvidmode* mode)
{
  //Initial windows configs
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  glfwWindowHint(GLFW_SAMPLES, 1);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //Create the window
  window = glfwCreateWindow(mode->width, mode->height, "The Abyss", glfwGetPrimaryMonitor(), nullptr);
  if(window == NULL)
  {
    std::cout << "Window creation failed\n";
    throw -1;
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
  renderThread.join();
  deleteThread.join();
  sendThread.join();
  receiveThread.join();
  logicThread.join();
  buildThread.join();

  std::cout << "All thread ended\n";
}

void ThreadHandler::dispatchThreads()
{
  threadsOn = true;
  renderThread = std::thread(render);
  deleteThread = std::thread(del);
  sendThread = std::thread(send);
  receiveThread = std::thread(receive);
  logicThread = std::thread(logic);
  buildThread = std::thread(build);



  std::cout << "Render threads created \n";

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


  PlayerWorld.drawer->updateDirectionalLight(glm::vec3(-0.5,-1.0f,-0.5f),glm::vec3(0.2f),glm::vec3(0.5f),glm::vec3(1.0f));
  PlayerWorld.drawer->addCube(glm::vec3(0,50,0));
  PlayerWorld.drawer->addCube(glm::vec3(0,0,0));
  PlayerWorld.drawer->addLight(glm::vec3(0,50,0),glm::vec3(1.0f,1.0f,1.0f));
  PlayerWorld.drawer->addLight(glm::vec3(0,0,0),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.5f,0.5f),1.0,0.045,0.0075);


  glm::vec3 terrainColor = glm::vec3(1.0f);
  float sunX=0.01,sunZ=0.01;
  bool shadowsOn = true;
  while(!glfwWindowShouldClose(window) && threadsOn)
  {

    PlayerWorld.drawer->setTerrainColor(terrainColor);
    glm::vec3 dirLight(0.1,-1.0f,0.1);
    glfwPollEvents();
    MainChar::update();

    PlayerWorld.drawer->updateDirectionalLight(dirLight,glm::vec3(0.8f,0.8f,0.8f));
    PlayerWorld.drawnChunks = 0;
    PlayerWorld.drawer->chunksToDraw = NULL;
    PlayerWorld.deleteChunksFromQueue();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PlayerWorld.calculateViewableChunks();
    PlayerWorld.drawer->renderGBuffer();
    PlayerWorld.drawer->drawFinal();
    GUI::drawGUI();
    glfwSwapBuffers(window);
  }
  PlayerWorld.buildQueue.notify_one();
  std::cout << "TOtalchunks: " << BSPNode::totalChunks << "\n";
  std::cout << "Draw thread done\n";
}

void ThreadHandler::render()
{
  Timer timer(27000);
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    timer.wait();
    PlayerWorld.renderWorld(MainChar::getPosition());
  }


}

void ThreadHandler::del()
{
  Timer timer(0.2);
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    timer.wait();
    PlayerWorld.delScan(MainChar::getPosition());
  }
  std::cout << "Delete Thread Done\n";
}

void ThreadHandler::logic()
{
  Timer timer(60);
/*
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
  btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
  ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
  btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
  ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
  btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
  btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

  dynamicsWorld->setGravity(btVector3(0, -10, 0));




  {
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}
  */
  while(!glfwWindowShouldClose(window) && threadsOn)
  {
    timer.wait();
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
          PlayerWorld.generateChunkFromString(glm::ivec3(msg.x.i,msg.y.i,msg.z.i),buf);
          delete[] buf;
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
  PlayerWorld.messenger->messageQueue.notify_one();
  std::cout << "RECEIVE THREAD DONE\n";
}

void ThreadHandler::build()
{
  while(!glfwWindowShouldClose(window)  && threadsOn)
  {
    PlayerWorld.buildWorld();
  }
  std::cout << "Ending build thread\n";
}
