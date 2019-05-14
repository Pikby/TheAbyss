#ifndef GUILIBRARY
#define GUILIBRARY

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <list>
#include <GLFW/glfw3.h>
#include "../Widgets/textrenderer.h"
class Shader;
class Widget;
class Menu;
enum QuadDrawType {DEFAULTQUAD = 0,ROUNDEDQUAD = 1,INVERTEDQUAD = 2, CIRCLEQUAD = 3, OUTLINEQUAD =4};

class GUI
{
private:
  static glm::vec2 mousePos;
  static Menu* currentMenu;
  static Shader GUIShader2D,GUIShaderImage;
  static uint quadVAO,quadVBO;
public:
  static void setMenu(Menu* newMenu){currentMenu = newMenu;}
  static glm::vec2 getMousePos(){return mousePos;}
  static TextRenderer textRenderer;
  static glm::ivec2 dimensions;
  static void renderText(std::string text, glm::vec2 pos, float scale=1, glm::vec4 color=glm::vec4(1),glm::mat3 rot = glm::mat3(1),TextAlignment alignment = TEXTALILEFT){textRenderer.renderText(text,pos,scale,color,rot,alignment);}
  static glm::vec3 calculateStringDimensions(const std::string &line, double scale){return textRenderer.calculateStringDimensions(line,scale);}
  static void initGUI(const glm::ivec2 Dimensions);
  static void freeGUI();
  static void initQuadVAO();

  static void setQuadDepth(float newDepth);

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
};
#ifdef GUILIBRARYIMPLEMENTATION
glm::vec2 GUI::mousePos;
glm::ivec2 GUI::dimensions;
Menu* GUI::currentMenu = NULL;
Shader GUI::GUIShader2D,GUI::GUIShaderImage;
TextRenderer GUI::textRenderer;
uint GUI::quadVAO,GUI::quadVBO;

#endif



#endif
