#ifndef GUILIBRARY
#define GUILIBRARY



#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <list>
#include <GLFW/glfw3.h>
#include <memory>
//#include "../Widgets/textrenderer.h"
//#include "../Widgets/widgets.h"
#include "../Widgets/texttypes.h"

class Shader;
class Widget;

const glm::vec3 COLORPALETTE[5] =
{
  glm::vec3(85,192,127)/255.0f,
  glm::vec3(100,162,95)/255.0f,
  glm::vec3(94,106,10)/255.0f,
  glm::vec3(151,147,17)/255.0f,
  glm::vec3(246,255,123)/255.0f,
};


class ChatBox;
class InGame;
class TextRenderer;
class Menu;
class GUI
{
private:

  static glm::vec2 mousePos;
  static Menu* currentMenu;
  static Shader GUIShader2D,GUIShaderImage;
  static uint quadVAO,quadVBO;

public:
  static std::unique_ptr<Menu> debugMenu,gameMenu,mainMenu,optionsMenu,pauseMenu;
  static std::unique_ptr<ChatBox> chatBox;
  static std::unique_ptr<InGame> inGame;
  static std::string username;

  static std::unique_ptr<TextRenderer> textRenderer;
  static glm::ivec2 dimensions;


  static void setMenu(Menu* newMenu);
  static glm::vec2 getMousePos(){return mousePos;}
  static void renderText(std::string text, glm::vec2 pos, float scale=1, glm::vec4 color=glm::vec4(1),glm::mat3 rot = glm::mat3(1),TextAlignment alignment = TEXTALILEFT,int cursorPosition = -1);
  static glm::vec3 calculateStringDimensions(const std::string &line, double scale);

  static std::vector<std::string> splitString(const std::string& string,double scale, int viewLength);
  static void initGUI(const glm::ivec2 Dimensions, const std::string& name);
  static void freeGUI();
  static void initQuadVAO();

  static void setQuadDepth(float newDepth);
  static void initGameJoinMenu();
  static glm::mat3 calculateQuadModel(const glm::vec2& botLeft, const glm::vec2& topRight);
  static void drawImage(const glm::vec2& botLeft, const glm::vec2& topRight, const uint id);
  static void drawImage(const glm::mat3& model, const uint id);
  static void drawInstancedQuads(int count);
  static void drawQuad(const glm::mat3&  model,const glm::vec4& color,QuadDrawType type = DEFAULTQUAD, Shader* shader = &GUIShader2D);
  static void drawQuad(const glm::vec2& botLeft,const glm::vec2& topRight,const glm::vec4& color =glm::vec4(1) ,QuadDrawType type = DEFAULTQUAD, Shader* shader = &GUIShader2D);
  static void drawQuad(const glm::vec2& p1,const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4,QuadDrawType type = DEFAULTQUAD,Shader* shader = &GUIShader2D);
  static void drawTriangle(const glm::vec2& p1,const glm::vec2& p2,const glm::vec2& p3, Shader* shader = &GUIShader2D);
  static void drawGUI();
  static void GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
  static void GLFWCharCallBack(GLFWwindow* window, uint character);
  static void GLFWCursorCallback(GLFWwindow* window, double xpos, double ypos);
  static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void handleMouseHover();
  static Widget* findWidgetAtMouse();

  static void initDebugMenu();
  static void initMainMenu();
  static void initOptionsMenu();
  static void initGameMenus();
};
#ifdef GUILIBRARYIMPLEMENTATION
std::unique_ptr<InGame> GUI::inGame;
std::unique_ptr<ChatBox> GUI::chatBox;
std::unique_ptr<Menu> GUI::debugMenu, GUI::gameMenu,GUI::mainMenu,GUI::optionsMenu,GUI::pauseMenu;
glm::vec2 GUI::mousePos;
glm::ivec2 GUI::dimensions;
Menu* GUI::currentMenu = NULL;
Shader GUI::GUIShader2D,GUI::GUIShaderImage;
std::unique_ptr<TextRenderer> GUI::textRenderer;
uint GUI::quadVAO,GUI::quadVBO;
std::string GUI::username;

#endif



#endif
