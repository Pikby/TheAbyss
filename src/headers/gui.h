#pragma once

#include "../headers/shaders.h"
struct Character
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};


class GUIRenderer
{
private:
  std::map<GLchar, Character> Characters;
  GLuint VAOgui, VBOgui;
  Shader guiShader;
public:
  GUIRenderer(int width, int height);
  GUIRenderer(){};
  void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(0,0,0));
  void drawRectangle(float x1, float y1, float x2, float y2,glm::vec3 color = glm::vec3(0,0,0));
  void drawRectangle(glm::vec2 a, glm::vec2 b,glm::vec3 color = glm::vec3(0,0,0));
};
