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

#include "headers/shaders.h"
#include "headers/camera.h"
#include "headers/gui.h"



GUIRenderer::GUIRenderer(int width, int height)
{
  guiShader = Shader("../src/shaders/shaderText.vs","../src/shaders/shaderText.fs");
  glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
  guiShader.use();
  guiShader.setMat4("projection",projection);

  FT_Library ft;
  if (FT_Init_FreeType(&ft))
      std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
  FT_Face face;
  if (FT_New_Face(ft, "../assets/fonts/LDFComicSans.ttf", 0, &face))
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  FT_Set_Pixel_Sizes(face,0,48);

  glPixelStorei(GL_UNPACK_ALIGNMENT,1 );



  for(GLubyte c = 0;c<128;c++)
  {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character =
    {
      texture,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      (unsigned int)face->glyph->advance.x
    };
    Characters.insert(std::pair<GLchar, Character>(c, character));


  }
  glBindTexture(GL_TEXTURE_2D, 0);

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &VAOgui);
  glGenBuffers(1, &VBOgui);
  glBindVertexArray(VAOgui);
  glBindBuffer(GL_ARRAY_BUFFER, VBOgui);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
}

void GUIRenderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
  guiShader.use();
  guiShader.setVec3("textColor",color);
  guiShader.setBool("isText",true);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAOgui);



  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++)
  {
      Character ch = Characters[*c];

      GLfloat xpos = x + ch.bearing.x * scale;
      GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

      GLfloat w = ch.size.x * scale;
      GLfloat h = ch.size.y * scale;
      // Update VBOText for each character
      GLfloat vertices[6][4] = {
          { xpos,     ypos + h,   0.0, 0.0 },
          { xpos,     ypos,       0.0, 1.0 },
          { xpos + w, ypos,       1.0, 1.0 },

          { xpos,     ypos + h,   0.0, 0.0 },
          { xpos + w, ypos,       1.0, 1.0 },
          { xpos + w, ypos + h,   1.0, 0.0 }
      };
      // Render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.textureID);
      // Update content of VBOText memory
      glBindBuffer(GL_ARRAY_BUFFER, VBOgui);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // Render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GUIRenderer::drawRectangle(float x1, float y1, float x2, float y2, glm::vec3 color)
{

  guiShader.use();
  guiShader.setVec3("textColor",color);
  guiShader.setBool("isText",false);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAOgui);
  Character c = Characters[65];


  GLfloat vertices[6][4] =
  {
    {x1,y1,0.0,1.0},
    {x2,y1,1.0,1.0},
    {x1,y2,0.0,0.0},


    {x1,y2,0.0,0.0},
    {x2,y1,1.0,1.0},
    {x2,y2,1.0,0.0}

  };
  glBindTexture(GL_TEXTURE_2D, c.textureID);
  glBindBuffer(GL_ARRAY_BUFFER, VBOgui);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDrawArrays(GL_TRIANGLES,0,6);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GUIRenderer::drawRectangle(glm::vec2 a,glm::vec2 b,glm::vec3 color)
{
    drawRectangle(a.x,a.y,b.x,b.y,color);
}
