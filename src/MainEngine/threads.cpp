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
#include "../Objects/include/objects.h"
#include "../Settings/settings.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

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


  World::drawer.addCube(glm::vec3(0,50,0));
  World::drawer.addCube(glm::vec3(0,0,0));

  World::drawer.addLight(glm::vec3(0,50,0),glm::vec3(1.0f,1.0f,1.0f));
  World::drawer.addLight(glm::vec3(0,0,0),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.5f,0.5f),1.0,0.045,0.0075);
  bool show_demo_window = true;
  bool show_another_window = false;
  //World::addLight(glm::vec3(10,50,10));
  glm::vec3 terrainColor = glm::vec3(1.0f);
  float sunX=0.01,sunZ=0.01;
  bool shadowsOn = true;
  while(!glfwWindowShouldClose(window))
  {
    static const char* current_item = "1.0";
    static const char* old_item = "1.0";
    if(current_item!=old_item)
    {
      old_item = current_item;
      std::cout << "Updating to " << std::stof(current_item) << "\n";
      World::drawer.updateAntiAliasing(std::stof(current_item));
    }
    World::drawer.setTerrainColor(terrainColor);
    glm::vec3 dirLight(sunX,-1.0f,sunZ);


    World::drawer.updateDirectionalLight(dirLight,glm::vec3(0.8f,0.8f,0.8f));

    World::drawnChunks = 0;
    World::drawer.chunksToDraw = NULL;





    World::deleteChunksFromQueue();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    World::drawer.updateCameraMatrices(mainCam);

    float findstart = glfwGetTime();
    World::calculateViewableChunks();
    float findend = glfwGetTime();

    float directstart = glfwGetTime();
    //if(shadowsOn) World::drawer.renderDirectionalShadows();
    float directend = glfwGetTime();

    ImGui::GetIO().WantCaptureMouse = true;
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::SliderFloat("SunX", &sunX, -10.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("SunZ", &sunZ, -10.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("Terrain Color", (float*)&terrainColor); // Edit 3 floats representing a color

        char heldBlock = MainChar::heldItem;




        static uint id = 1;
        if(ImGui::Button("<"))
        {
          id--;
          if(id <= 0) id = 1;
        }
        uint texId  = World::drawer.getTextureAtlasID();

        int blkId = ItemDatabase::blockDictionary[heldBlock].getTop();
        int width = World::drawer.textureAtlasDimensions.x/128;
        int height = World::drawer.textureAtlasDimensions.y/128;
        int xcoord = (blkId % width);
        int ycoord = (blkId/height);
        ImGui::SameLine();
        ImGui::Image((void*)(intptr_t)texId,ImVec2(128,128),ImVec2(xcoord/3.0f,ycoord/3.0f),ImVec2(xcoord/3.0f + 1/3.0f,ycoord/3.0f + 1.0f/3.0f));
        ImGui::SameLine();
        if(ImGui::Button(">"))
        {
          id++;
        }

        MainChar::heldItem = id;
        std::string buttonStr = shadowsOn ? "On" : "Off";
        if (ImGui::Button(std::string("Shadows" + buttonStr).c_str()))     // Buttons return true when clicked (most widgets return true when edited/activated)
        {
          if(shadowsOn) shadowsOn = false;
          else shadowsOn = true;
          World::drawer.updateShadows(shadowsOn);
        }
        ImGui::Text("MSAA: ");

        const char* items[] = {"0.0625","0.125","0.25" ,"0.5", "1.0", "2.0", "4.0","8.0"};

        if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n], is_selected)) current_item = items[n];
                if (is_selected) ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
        }


        glm::vec3 pos = mainCam->getPosition();

        static float exposure = 1.0f;
        ImGui::SliderFloat("Exposure:", &exposure, 0.0f, 10.0f);
        World::drawer.setExposure(exposure);


        ImGui::Text("WorldInfo:\nPlayerPos: x:%.2f y:%.2f z:%.2f",pos.x,pos.y,pos.z);
        ImGui::Text("Chunks Loaded:%d",BSPNode::totalChunks);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Took %.6f ms to find all viewable chunks",1000.0f*(findend-findstart));
        ImGui::Text("Took %.6f ms to render directional shadows",1000.0f*(directend-directstart));
        static int curText = 9;

        static glm::vec3 lastPos;
        glm::vec3 curPos = MainChar::mainCam.position;
        double distance = glm::length(lastPos-curPos)*ImGui::GetIO().Framerate;

        ImGui::Text("You are traveling %.6f ms",distance);

        lastPos = curPos;
        if(ImGui::Button("<-"))
        {
          curText--;
          if(curText <= 0) curText = 1;
          std::cout << curText << "\n";
        }
        ImGui::SameLine();
        ImGui::Image((void*)(intptr_t)curText,ImVec2(128,128),ImVec2(0.0f,1.0f),ImVec2(1.0f,0.0f));
        ImGui::SameLine();
        if(ImGui::Button("->"))
        {
          curText++;
          std::cout << curText << "\n";
        }
        ImGui::Text("Current texturd Id: %d",curText);
        if(ImGui::Button("Exit Game"))
        {
          closeGame();
        }

        ImGui::End();
    }




    World::drawer.renderGBuffer();


    if(BSP::geometryChanged == true)
    {


      BSP::geometryChanged = false;
    }
    glDisable(GL_BLEND);
    World::drawer.drawFinal();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


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
    MainChar::update();
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
          //MainChar::addChatLine(line);
          delete[] buf;
        }
        break;
      case(101):
        {
          char* buf = new char[msg.length];
          World::messenger.receiveMessage(buf,msg.length);
          std::string line = std::string(buf,msg.length);
          std::cout << line;
          //MainChar::addChatLine(line);
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
