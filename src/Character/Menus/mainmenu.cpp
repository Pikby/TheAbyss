#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/gui.h"
#include "../include/menus.h"
#include "../../MainEngine/include/world.h"
#include "../Widgets/widgets.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void GUI::initMainMenu()
{
  Label* title = new Label("The Abyss",glm::vec2(0.5,0.9),2.0,TEXTALICENTER);

  //Button* testButton = new Button(glm::vec2(0.5,0.5));

  title->setOnDraw([](Widget* parent)
  {
    double x = sin(glfwGetTime());
    glm::mat3 rot = glm::rotate(glm::mat3(1),(float)(x*glm::radians(5.0)));
    ((Label*)parent)->setRotMat(rot);
  });


  Quad* quad = new Quad(glm::vec2(0),glm::vec2(1));
  const glm::vec2 offset = glm::vec2(0.1);
  Button* startGame = new Button(glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),"Start Game",[](int action)
  {
    std::cout << "PRESSED START\n";
  });

  Button* controlsButton = new Button(glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),"Controls",[](int action)
  {
    std::cout << "Control\n";
  });

  Button* optionsButton = new Button(glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),"Settings",[](int action)
  {
    std::cout << "Options\n";
  });

  WidgetList* menuList = new WidgetList(glm::vec2(0.5,0.5));
  menuList->addWidget(startGame);
  menuList->addWidget(controlsButton);
  menuList->addWidget(optionsButton);

  startGame->setColor(glm::vec4(COLORPALETTE[1],1));

  quad->setColor(glm::vec4(COLORPALETTE[4],1));
  title->setColor(glm::vec4(COLORPALETTE[0],1));


  mainMenu->addWidget(quad);
  mainMenu->addWidget(menuList);
  mainMenu->addWidget(title);
  mainMenu->setFocusTarget(menuList);
  //mainMenu.addWidget(startGame);

  //setMenu(&mainMenu);
}
