#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../MainEngine/include/world.h"
#include "../include/gui.h"
#include "../Widgets/widgets.h"

void GUI::initDebugMenu()
{
  InGame* inGame = new InGame;
  Label* test = new Label("DEBUG!!!",glm::vec2(0.5),12.0/64.0);

  test->setOnDraw([](Widget* parent)
  {
    double x = glfwGetTime();
    glm::vec3 color = glm::vec3((sin(x)+1)/2.0,(cos(x)+1)/2.0 ,1);
    parent->setColor(glm::vec4(color,1.0));
  });



  Label* fps = new Label("STUFF",glm::vec2(0,1.0-0.02)  ,24.0/64.0);
  fps->setColor(glm::vec4(0.7,0.7,0.7,1.0));
  fps->setOnDraw([](Widget* parent)
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
  Label* ping = new Label("STUFF",glm::vec2(0.1,1.0-0.02),24.0/64.0);
  ping->setColor(glm::vec4(0.7,0.7,0.7,1.0));
  ping->setOnDraw([](Widget* parent)
  {

    std::string pingStr = "Ping: " + std::to_string((int)World::worldStats.pingInMS) + " ms";
    ((Label*)parent)->updateLabel(pingStr);
  });


  debugMenu.addWidget(ping);
  debugMenu.addWidget(fps);
  debugMenu.addWidget(test);
  debugMenu.addWidget(inGame);
  debugMenu.setFocusTarget(inGame);
  debugMenu.setKeyMapping(GLFW_KEY_F1,[]()
  {
    setMenu(&gameMenu);
  });
  inGame = new InGame;
  gameMenu.addWidget(inGame);
  gameMenu.setFocusTarget(inGame);
  gameMenu.setKeyMapping(GLFW_KEY_F1,[]()
  {
    setMenu(&debugMenu);
  });

  setMenu(&debugMenu);

}
