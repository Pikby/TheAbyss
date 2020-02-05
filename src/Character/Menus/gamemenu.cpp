#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iomanip>

#include "../../MainEngine/include/world.h"
#include "../../MainEngine/include/threads.h"
#include "../../Character/include/mainchar.h"
#include "../include/gui.h"
#include "../include/menus.h"
#include "../Widgets/widgets.h"

void initPauseMenu()
{
  GUI::pauseMenu = std::make_unique<Menu>();
  Button* startGame = new Button("Resume Game",glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),[](int action)
  {
    GUI::setMenu(GUI::gameMenu.get());
  });

  Button* controlsButton = new Button("Back To Main Menu",glm::vec2(0.3,0.5),glm::vec2(0.1,0.03),[](int action)
  {
    GUI::setMenu(GUI::mainMenu.get());
    ThreadHandler::endThreads();
    PlayerWorld.destroyWorld();
  });


  GUI::pauseMenu->addWidget(startGame);
  GUI::pauseMenu->addWidget(controlsButton);
  GUI::pauseMenu->setCursor(true);
}


void GUI::initGameMenus()
{
  initPauseMenu();
}
