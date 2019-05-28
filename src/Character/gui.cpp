
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>


#include "../headers/shaders.h"
#define GUILIBRARYIMPLEMENTATION
#include "include/gui.h"
#include "include/menus.h"

#include "Widgets/widgets.h"
#include "Widgets/textrenderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>





void GUI::initGUI(const glm::ivec2 Dimensions,const std::string &name)
{
  chatBox = std::make_unique<ChatBox>(glm::vec2(0.2,0.2),glm::vec2(0.2,0.2),12.0/64.0);
  inGame = std::make_unique<InGame>();
  textRenderer = std::make_unique<TextRenderer>();
  debugMenu = std::make_unique<Menu>();
  gameMenu = std::make_unique<Menu>();
  mainMenu = std::make_unique<Menu>();

  username = name;

  glm::mat4 projection = glm::ortho(0.0f,1.0f,0.0f,1.0f);
  dimensions = Dimensions;


  GUIShader2D = Shader("GUIShaders/guiShader2D.vs","GUIShaders/guiShader2D.fs");
  GUIShaderImage = Shader("GUIShaders/guiImageShader.vs","GUIShaders/guiImageShader.fs");

  GUIShaderImage.use();
  GUIShaderImage.setMat4("projection",projection);
  GUIShaderImage.setInt("text",0);

  GUIShader2D.use();
  GUIShader2D.setMat4("projection",projection);
  GUIShader2D.setFloat("depth",0);

  std::cout << "initing GUI\n";



  initQuadVAO();
  textRenderer->init();
  GUI::textRenderer->GUIShaderText->use();

  chatBox->setOrigin(glm::vec2(0.2,0.2));
  chatBox->setColor(glm::vec4(0,0,0,1));
  initDebugMenu();
  initMainMenu();
}

void GUI::drawGUI()
{
  if(currentMenu == NULL) return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  currentMenu->drawGUI();
  glEnable(GL_DEPTH_TEST);
  textRenderer->drawAllText();
}

void GUI::freeGUI()
{
}


glm::mat3 GUI::calculateQuadModel(const glm::vec2& botLeft, const glm::vec2& topRight)
{
  float x2 = botLeft.x;
  float x1 = topRight.x;
  float y2 = botLeft.y;
  float y1 = topRight.y;

  float a = (x2-x1)/-2.0f;
  float b = x2+a;
  float c = (y2-y1)/-2.0f;
  float d = y2+c;

  glm::mat3 model =
  {
    a,0.0f,0.0f,
    0.0f,c,0.0f,
    b,d,1.0f,
  };
  //std::cout << a << ":" << b<< ":"<<c << ":" << d << "\n";

  return model;
}

void GUI::drawInstancedQuads(int count)
{
  glBindVertexArray(quadVAO);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4,count);
  glBindVertexArray(0);
}

void GUI::initQuadVAO()
{
  float quadVertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
  };
  // setup plane VAO
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}



void GUI::drawQuad(const glm::mat3& model,const glm::vec4 &color, QuadDrawType type, Shader* shader)
{
  shader->use();
  shader->setMat3("model",model);
  shader->setVec4("color",color);
  shader->setInt("quadStyle",type);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void GUI::setQuadDepth(float newDepth)
{
  GUIShader2D.setFloat("depth",newDepth);
}


void GUI::drawQuad(const glm::vec2 &botLeft,const glm::vec2 &topRight,const glm::vec4& color, QuadDrawType type, Shader* shader)
{
  glm::mat3 model = calculateQuadModel(botLeft,topRight);
  shader->use();
  shader->setMat3("model",model);
  shader->setVec4("color",color);
  shader->setInt("quadStyle",type);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void GUI::drawQuad(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3 ,const glm::vec2 &p4, QuadDrawType type, Shader* shader)
{
  shader->setInt("quadStyle",type);
  drawTriangle(p1,p2,p3,shader);
  drawTriangle(p1,p3,p4,shader);
}
void GUI::renderText(std::string text, glm::vec2 pos, float scale, glm::vec4 color,glm::mat3 rot,TextAlignment alignment,int cursorPosition)
{
  textRenderer->renderText(text,pos,scale,color,rot,alignment,cursorPosition);
}

glm::vec3 GUI::calculateStringDimensions(const std::string &line, double scale)
{
  return textRenderer->calculateStringDimensions(line,scale);
}

std::vector<std::string> GUI::splitString(const std::string& string,double scale, int viewLength)
{
  return textRenderer->splitString(string,scale,viewLength);
}

void GUI::drawTriangle(const glm::vec2& p1,const glm::vec2& p2, const glm::vec2& p3,Shader* shader)
{
  static uint triVAO,triVBO;
  const glm::vec2 src1(-1,-1);
  const glm::vec2 src2(0,1);
  const glm::vec2 src3(1.0f,-1.0f);
  if (triVAO == 0)
  {
      float triVertices[] = {
          // positions        // texture Coords
           src1.x, src1.y, 0.0f, 0.0f,
           src2.x, src2.y, 0.5f, 1.0f,
           src3.x, src3.y, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &triVAO);
      glGenBuffers(1, &triVBO);
      glBindVertexArray(triVAO);
      glBindBuffer(GL_ARRAY_BUFFER, triVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), &triVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  }
  glm::mat3 target =
  {
    p1.x,p1.y,1,
    p2.x,p2.y,1,
    p3.x,p3.y,1
  };
  const glm::mat3 src =
  {
    src1.x,src1.y,1,
    src2.x,src2.y,1,
    src3.x,src3.y,1
  };
  const glm::mat3 invSrc = glm::inverse(src);

  glm::mat3 model = target*invSrc;
  GUIShader2D.use();
  GUIShader2D.setMat3("model",model);

  glViewport(0,0,dimensions.x,dimensions.y);
  glBindVertexArray(triVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
  glBindVertexArray(0);
}
void GUI::drawImage(const glm::vec2& botLeft, const glm::vec2& topRight, const uint id)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,id);
  drawQuad(botLeft,topRight,glm::vec4(1),DEFAULTQUAD,&GUIShaderImage);
}

void GUI::drawImage(const glm::mat3& model, const uint id)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,id);
  drawQuad(model,glm::vec4(1),DEFAULTQUAD,&GUIShaderImage);
}


void GUI::GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if(currentMenu == NULL) return;
  currentMenu->GLFWKeyCallBack(window,key,scancode,action,mode);
}

void GUI::GLFWCharCallBack(GLFWwindow* window, uint character)
{
  if(currentMenu == NULL) return;
  currentMenu->GLFWCharCallBack(window,character);
}

void GUI::GLFWCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
  mousePos = glm::vec2(xpos/dimensions.x,1-ypos/dimensions.y);
  if(currentMenu == NULL) return;
  currentMenu->GLFWCursorCallback(window,xpos,ypos);
}
void GUI::handleMouseHover()
{
  if(currentMenu == NULL) return;
  currentMenu->handleMouseHover(mousePos);
}

void GUI::GLFWMouseButtonCallback(GLFWwindow* window, int button,int action, int mods)
{
  if(currentMenu == NULL) return;
  currentMenu->GLFWMouseButtonCallback(window,button,action,mods);
}

void GUI::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(currentMenu == NULL) return;
  currentMenu->GLFWScrollCallback(window,xoffset,yoffset);
}
