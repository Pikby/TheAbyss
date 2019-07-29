#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/gui.h"
#include "../include/menus.h"
#include "../../MainEngine/include/world.h"
#include "../Widgets/widgets.h"
#include "../../MainEngine/include/threads.h"
#include "../../Settings/settings.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>



Menu gameJoinMenu;
EditBox* ipEditBox;
EditBox* userNameBox;

void GUI::initGameJoinMenu()
{
  Quad* background = new Quad(glm::vec2(0),glm::vec2(1));
  ipEditBox = new EditBox("Enter Ip",glm::vec2(0.4,0.5),glm::vec2(0.2,0.05),32,[](std::string input)
  {
    std::cout << input << "\n";
    Settings::set("Ip",input);

  });
  ipEditBox->updateText(Settings::get("ip"));
  userNameBox = new EditBox("Enter Username",glm::vec2(0.4,0.7),glm::vec2(0.2,0.05),32,[](std::string input)
  {
    Settings::set("UserName",input);
  });

  Label* txt = new Label("I dont think trying to code a 3d engine by myself from the ground up was the best idea ive had",glm::vec2(0.8,0.8),32,TEXTALICENTER);
  txt->setOnDraw([](Widget* parent)
  {
    double x = glfwGetTime();
    glm::mat3 rot = glm::rotate(glm::mat3(1),(float)x);
    ((Label*)parent)->setRotMat(rot);
    parent->setOrigin(glm::vec2(0.5+0.3*sin(x),0.5+0.5*cos(x)));
  });
  txt->setColor(glm::vec4(glm::vec3(0),1));



  Button* joinGameButton = new Button("Join Game!",glm::vec2(0.4,0.2),glm::vec2(0.2,0.05),[](int action)
  {
    std::cout << "Kero\n";
    try
    {
      ThreadHandler::initWorld(ipEditBox->getText(),userNameBox->getText());
    }
    catch(const char* err)
    {
      std::cout << err << "\n";
      return;
    }
    ThreadHandler::dispatchThreads();

    setMenu(debugMenu.get());
    ThreadHandler::draw();
    std::cout << "done\n";
  });



  userNameBox->updateText(Settings::get("username"));
  gameJoinMenu.addWidget(background);
  gameJoinMenu.addWidget(userNameBox);
  gameJoinMenu.addWidget(joinGameButton);
  gameJoinMenu.addWidget(txt);
  background->setColor(glm::vec4(COLORPALETTE[4],1));
  gameJoinMenu.addWidget(ipEditBox);
  gameJoinMenu.setFocusTarget(ipEditBox);
  gameJoinMenu.setCursor(true);
}



void GUI::initMainMenu()
{
  initGameJoinMenu();
  Label* title = new Label("The Abyss",glm::vec2(0.5,0.9),128.0,TEXTALICENTER);

  title->setOnDraw([](Widget* parent)
  {
    double x = sin(glfwGetTime());
    glm::mat3 rot = glm::rotate(glm::mat3(1),(float)(x*glm::radians(5.0)));
    ((Label*)parent)->setRotMat(rot);
  });


  Quad* quad = new Quad(glm::vec2(0),glm::vec2(1));
  const glm::vec2 offset = glm::vec2(0.1);
  Button* startGame = new Button("Start Game",glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),[](int action)
  {
    std::cout << "PRESSED START\n";
    setMenu(&gameJoinMenu);
  });

  Button* controlsButton = new Button("Controls",glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),[](int action)
  {
    std::cout << "Control\n";
  });

  Button* optionsButton = new Button("Settings",glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),[](int action)
  {
    std::cout << "Options\n";
  });

  Button* exitButton = new Button("Exit Button",glm::vec2(0.3,0.7),glm::vec2(0.1,0.03),[](int action)
  {
    exit(1);
  });



  WidgetList* menuList = new WidgetList(glm::vec2(0.5,0.5));
  menuList->addWidget(startGame);
  menuList->addWidget(controlsButton);
  menuList->addWidget(optionsButton);
  menuList->addWidget(exitButton);

  startGame->setColor(glm::vec4(COLORPALETTE[1],1));
  quad->setColor(glm::vec4(COLORPALETTE[4],1));
  title->setColor(glm::vec4(COLORPALETTE[0],1));


  mainMenu->addWidget(quad);
  mainMenu->addWidget(menuList);
  mainMenu->addWidget(title);
  mainMenu->setFocusTarget(menuList);
  mainMenu->setCursor(true);
  //mainMenu.addWidget(startGame);

  setMenu(mainMenu.get());
}
