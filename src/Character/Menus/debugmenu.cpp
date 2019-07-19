#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iomanip>

#include "../../MainEngine/include/world.h"
#include "../../Character/include/mainchar.h"
#include "../include/gui.h"
#include "../include/menus.h"
#include "../Widgets/widgets.h"

void GUI::initDebugMenu()
{
  Label* test = new Label("DEBUG!!!\nnewlineDEBUG",glm::vec2(0.5),12.0);

  test->setOnDraw([](Widget* parent)
  {
    double x = glfwGetTime();
    glm::vec3 color = glm::vec3((sin(x)+1)/2.0,(cos(x)+1)/2.0 ,1);
    parent->setColor(glm::vec4(color,1.0));
  });

  Label* debugstring= new Label("DEBUdsd!!",glm::vec2(0,0.98),24.0);
  debugstring->setColor(glm::vec4(1));
  debugstring->setOnDraw([](Widget* parent)
  {
    const int strWidth = 40;
    stringstream ss;
    {

      static double lastTest = 0;
      static int frameCount = 0;
      frameCount++;
      double curTime = glfwGetTime();
      static int displayCount = 0;
      if(abs(lastTest-curTime) > 1)
      {
        displayCount = frameCount;
        frameCount = 0;
        lastTest = curTime;
      }

      std::string fpsStr = "FPS: " + std::to_string(displayCount);
      ss << fpsStr << '\n';
    }

    {
      std::string pingStr = "Ping: " + std::to_string((int)PlayerWorld.worldStats.pingInMS) + " ms\t";
      ss << pingStr << '\n';
    }


    {
      glm::vec3 p =  MainChar::getPosition();
      stringstream pcss;
      pcss << "Player Coords: " << std::fixed << std::setprecision(2) << p.x << ':' << p.y << ':' << p.z << "\t";
      ss << pcss.str() << '\n';

      pcss.str(std::string());
      p = MainChar::getViewDirection();
      pcss << "View Direction: " << std::fixed << std::setprecision(2) << p.x << ':' << p.y << ':' << p.z << "\t";
      ss << pcss.str() << '\n';

    }

    {
      glm::ivec3 p = World::toChunkCoords(MainChar::getPosition());
      stringstream ccss;
      ccss << "Chunk Coords: " << std::fixed << std::setprecision(2) << p.x << ':' << p.y << ':' << p.z;
      ss << ccss.str() << '\n';
    }

    {
      int chunksInQueue = PlayerWorld.buildQueue.size();

      static int lastVal = 0;
      static double lastTime = glfwGetTime();
      static int rate = 0;
      if(glfwGetTime() - lastTime > 1 )
      {
        rate = lastVal - chunksInQueue;
        lastVal = chunksInQueue;
        lastTime = glfwGetTime();
      }
      std::string str = "Chunks Building:" + std::to_string(chunksInQueue) + " Build Rate ps: " + std::to_string(rate);
      ss << str << '\n';
      int chunksInMemory = PlayerWorld.BSPmap.getSize();
      str = "Chunks In Memory " + std::to_string(chunksInMemory);
      ss << str << '\n';
    }



    ((Label*)parent)->updateLabel(ss.str());
  });
  /*
  std::list<std::function<void(Widget* parent)>> labelList;
  labelList.push_back([](Widget* parent)
  {
    static double lastFrame = 0;
    double curFrame = glfwGetTime();

    static double lastFps;
    double curFps = 1.0/(curFrame-lastFrame);

    double alpha = 0.9;
    double expectedFps = lastFps*alpha +curFps*(1-alpha);
    std::string fpsStr = "FPS: " + std::to_string((int)expectedFps);
    lastFrame=curFrame;
    lastFps = expectedFps;
    ((Label*)parent)->updateLabel(fpsStr);
  });

  labelList.push_back([](Widget* parent)
  {

    std::string pingStr = "Ping: " + std::to_string((int)PlayerWorld.worldStats.pingInMS) + " ms";
    ((Label*)parent)->updateLabel(pingStr);
  });

  labelList.push_back([](Widget* parent)
  {
    glm::vec3 p=  MainChar::getPosition();
    std::stringstream ss;

    ss << std::fixed << std::setprecision(2) << p.x << ':' << p.y << ':' << p.z;
    std::string posStr = ss.str();

    std::string pingStr = "Player Coords: " +  posStr;
    ((Label*)parent)->updateLabel(pingStr);
  });


  labelList.push_back([](Widget* parent)
  {
    glm::ivec3 p= World::toChunkCoords(MainChar::getPosition());
    std::stringstream ss;

    ss << std::fixed << std::setprecision(2) << p.x << ':' << p.y << ':' << p.z;
    std::string posStr = ss.str();

    std::string pingStr = "Chunk Coords: " +  posStr;
    ((Label*)parent)->updateLabel(pingStr);
  });


  glm::vec2 curPos = glm::vec2(0,0.98);
  for(auto itr = labelList.begin(); itr != labelList.end();itr++)
  {
    Label* lbl = new Label("",curPos,12/64.0);
    lbl->setColor(glm::vec4(0.7,0.7,0.7,1.0));
    lbl->setOnDraw(*itr);
    debugMenu->addWidget(lbl);

    curPos += glm::vec2(0.1,0);
    if(curPos.x > 0.9)
    {
      curPos.x = 0;
      curPos.y -= 0.02;
    }
  }
  */
  debugMenu->addWidget(test);
  debugMenu->addWidget(debugstring);
  debugMenu->addWidget(inGame.get());
  debugMenu->setFocusTarget(inGame.get());
  debugMenu->setKeyMapping(GLFW_KEY_F1,[]()
  {
    setMenu(gameMenu.get());
  });
  debugMenu->setKeyMapping(GLFW_KEY_ESCAPE,[]()
  {
    setMenu(pauseMenu.get());
  });
  debugMenu->setCursor(false);

  gameMenu->addWidget(chatBox.get());
  gameMenu->setCursor(false);
  gameMenu->setFocusTarget(inGame.get());
  gameMenu->setKeyMapping(GLFW_KEY_F1,[]()
  {
    setMenu(debugMenu.get());
  });
  gameMenu->setKeyMapping(GLFW_KEY_T,[]()
  {
    gameMenu->setFocusTarget(chatBox.get());
  });
  gameMenu->setKeyMapping(GLFW_KEY_ESCAPE,[]()
  {
    setMenu(pauseMenu.get());
  });

  setMenu(debugMenu.get());

}
