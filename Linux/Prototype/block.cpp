
#include <string>
#include "SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "shaders.h"
#include "block.h"

block::block(float x, float y, const char* newTexture)
{
  blockShader = new Shader("shaders/shader.vs","shaders/shader.fs");
  xpos = x;
  ypos = y;
  texture = newTexture;
  std::cout << "texture\n";
  init();
  std::cout << "texture\n";
}

void block::init()
{
  float scrPosX = xpos;
  float scrPosY = ypos;


  GLfloat vertices[] =
  {
    scrPosX,            scrPosY,    0.0f,1.0f,1.0f,
    scrPosX+0.125f,     scrPosY,    0.0f,1.0f,0.0f,
    scrPosX,       scrPosY+0.167f,  0.0f,1.0f,0.0f,
    scrPosX+0.125f,  scrPosY+0.167f,0.0f,0.0f,1.0f
  };

  GLuint indices[] =
  {
    0,1,2,
    1,2,3
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,0,GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

}

void block::draw()
{
  glBindTexture(GL_TEXTURE_2D, glTexture);

  blockShader->Use();

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
  glBindVertexArray(0);


}
