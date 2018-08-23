#include <iostream>
#include <map>
#include <string>
#define GLEW_STATIC

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../headers/shaders.h"
#include "../headers/camera.h"
#include "include/gui.h"

bool test(const CEGUI::EventArgs& /*e*/)
{
  std::cout << "You clicked the fucking button good job idiot\n";
}



GUIRenderer::GUIRenderer(int Width, int Height, std::string userName)
{
  chatConsole.userName = userName;
  std::cout << "Making guirendered\n";
  width = Width;
  height = Height;
  using namespace CEGUI;
  CEGUI::OpenGL3Renderer& myRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
  initResourcePaths();
  CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );

  System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
  createMenuScreens();

  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
  WindowManager& wmgr = WindowManager::getSingleton();
  chatConsole.createChatWindow();

}

void GUIRenderer::openInventoryGUI()
{
  using namespace CEGUI;
  System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
  System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(Vector2f(width/2,height/2));
  invWindow->activate();
  invWindow->show();
}

void GUIRenderer::closeInventoryGUI()
{
  invWindow->deactivate();
  invWindow->hide();
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
}

void GUIRenderer::createMenuScreens()
{
  using namespace CEGUI;
  gameWindow = WindowManager::getSingleton().loadLayoutFromFile( "game.layout" );
  //gameWindow->hide();
  System::getSingleton().getDefaultGUIContext().setRootWindow(gameWindow);
  invWindow = WindowManager::getSingleton().loadLayoutFromFile( "inventory.layout" );
  invWindow->deactivate();
  invWindow->hide();
  System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild( invWindow );

  optionsWindow = WindowManager::getSingleton().loadLayoutFromFile( "options.layout" );
  optionsWindow->deactivate();
  optionsWindow->hide();
  System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild( optionsWindow );
}

void GUIRenderer::initResourcePaths()
{

  CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
  rp->setResourceGroupDirectory("imagesets", "../gui/imagesets/");
  rp->setResourceGroupDirectory("schemes", "../gui/schemes/");
  rp->setResourceGroupDirectory("fonts", "../gui/fonts/");
  rp->setResourceGroupDirectory("looknfeels", "../gui/looknfeel/");
  rp->setResourceGroupDirectory("layouts","../gui/layouts/");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
  CEGUI::Scheme::setDefaultResourceGroup("schemes");
  CEGUI::Font::setDefaultResourceGroup("fonts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
  CEGUI::WindowManager::setDefaultResourceGroup("layouts");
}

char keyToASCII(int key,bool shiftHeld)
{
  char c = key;
  if(shiftHeld)
  {
    switch(key)
    {
      case '`': c = '~'; break;
      case '1': c = '!'; break;
      case '2': c = '@'; break;
      case '3': c = '#'; break;
      case '4': c = '$'; break;
      case '5': c = '%'; break;
      case '6': c = '^'; break;
      case '7': c = '&'; break;
      case '8': c = '*'; break;
      case '9': c = '('; break;
      case '0': c = ')'; break;
      case '-': c = '_'; break;
      case '=': c = '+'; break;
      case '[': c = '{'; break;
      case ']': c = '}'; break;
      case '\\': c = '|'; break;
      case ';': c = ':'; break;
      case '\'': c = '\"'; break;
      case ',': c = '<'; break;
      case '/': c = '?'; break;
    }
  }
  else
  {
    switch(key)
    {
      case 65 ... 90: c += 32; break;
    }
  }
  return c;
}

void ChatConsole::addCharacterToChat(int key,bool shiftHeld)
{
  if(key == GLFW_KEY_BACKSPACE && !curMsg.empty())
  {
    curMsg.pop_back();
  }
  else
  {
    char c = keyToASCII(key,shiftHeld);
    curMsg.push_back(c);
  }
  consoleWindow->getChild("Editbox")->setText(curMsg);
}


void ChatConsole::addChatLine(std::string s,CEGUI::Colour colour)
{
  CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(consoleWindow->getChild("History"));
  CEGUI::ListboxTextItem* newItem = new CEGUI::ListboxTextItem(s);
  newItem->setTextColours(colour);
  outputWindow->addItem(newItem);
}

void ChatConsole::sendCurrentMessage()
{
  if(curMsg != "")
  {
    std::string line = '(' + userName + "): " + curMsg;
    consoleWindow->getChild("Editbox")->setText("");
  }
  curMsg = "";

}

void ChatConsole::createChatWindow()
{
  CEGUI::WindowManager *wManager = CEGUI::WindowManager::getSingletonPtr();
  consoleWindow = wManager->loadLayoutFromFile("chat.layout");
  if(consoleWindow != NULL)
  {
    CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(consoleWindow);
  }
  else
  {
    CEGUI::Logger::getSingleton().logEvent("Error loading chat window layout\n");
  }
}


void ChatConsole::open(bool visible)
{
  isOpen = visible;

  auto editBox = consoleWindow->getChild("Editbox");
  if(isOpen)
  {
    editBox->setVisible(isOpen);
    editBox->activate();
  }
  else
  {
    editBox->setVisible(isOpen);
    editBox->deactivate();
  }
}

void ChatConsole::update()
{
  while(!incomingMessages->empty())
  {
    std::string curLine = incomingMessages->front();
    incomingMessages->pop();
    addChatLine(curLine);
  }


}
