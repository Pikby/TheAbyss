#pragma once
#include <stack>
#include "../../headers/shaders.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/DefaultResourceProvider.h>
#include "../../headers/threadSafeQueue.h"
class ChatConsole
{
private:
  bool openChat(const CEGUI::EventArgs &e);
  void parseText(CEGUI::String msg);

  CEGUI::Window *consoleWindow;


  bool isOpen;
public:
  std::string curMsg;
  std::string userName;
  TSafeQueue<std::string>* incomingMessages;
  void addChatLine(std::string msg, CEGUI::Colour colour = CEGUI::Colour( 0xFFFFFFFF));
  void sendCurrentMessage();
  void createChatWindow();
  void addCharacterToChat(int c,bool shiftHeld);
  void removeCharacterFromChat();
  void update();
  ChatConsole()
  {
    consoleWindow = NULL;
    isOpen = false;
    incomingMessages = new TSafeQueue<std::string>;
  }
  void open(bool visible);

};

class GUIRenderer
{
private:
  int width, height;
public:
  ChatConsole chatConsole;
  GUIRenderer(){};
  GUIRenderer(int Width, int Height,std::string userName);
  void switchToGameGUI();
  void switchToInventoryGUI();
  void initFreetype();
  void initResourcePaths();
  void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(0,0,0));
  void addCharacterToChat(int c);
};
