
enum Type {STATIC,DYNAMIC,STREAM};

#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "headers/shaders.h"
#include "headers/block.h"


Block::Block(const char* newTexture, int newType)
{
  blockShader = new Shader("../src/shaders/shaderBlocks.vs","../src/shaders/shaderBlocks.fs");
  texture = newTexture;
  type = newType;
  refresh();
}

void Block::refresh()
{
  GLfloat vertices[] =
  {
    0.0f, 0.0f,   0.0f,0.0f,0.0f,
    0.1f, 0.0f,   0.0f,0.0f,1.0f,
    0.0f, 0.1f,   0.0f,1.0f,0.0f,
    0.1f, 0.1f,   0.0f,1.0f,1.0f,
    0.0f, 0.0f,   0.1f,1.0f,1.0f,
    0.1f, 0.0f,   0.1f,1.0f,0.0f,
    0.0f, 0.1f,   0.1f,0.0f,1.0f,
    0.1f, 0.1f,   0.1f,0.0f,0.0f
  };

  GLuint indices[] =
  {
    //front
    0,1,2,
    1,2,3,

    //top
    2,6,7,
    2,3,7,

    //bottom
    0,4,5,
    0,1,5,

    //lside
    0,2,6,
    0,4,6,

    //rside
    1,3,7,
    1,5,7,

    //back
    4,5,6,
    5,6,7
  };

  //Generate and bind the buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  //Draws the object for which ever type it is
  switch(type)
  {
    case STATIC:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices, GL_STATIC_DRAW);
    break;
    case DYNAMIC:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices, GL_DYNAMIC_DRAW);
    break;
    case STREAM:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices, GL_STREAM_DRAW);
    break;
    default :
    std::cout << "INVALID DRAW TYPE  \n STOPPING REFRESH\n";
    return;
  }

  //Position
  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  //Texture
  glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  //Set the texture properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //Load and bind the texture from the class
  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,0,GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
}

WorldBlk::WorldBlk(int x, int y ,int z, Block* newId)
{
  xpos = (float)x/(float)10;
  ypos = (float)y/(float)10;
  zpos = (float)z/(float)10;
  id = newId;
}

void WorldBlk::draw(glm::mat4 camera)
{
  glBindTexture(GL_TEXTURE_2D, id->glTexture);
  id->blockShader->Use();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/ (float)600, 0.1f, 100.0f);
  id->blockShader->setMat4("projection", projection);

  glm::mat4 view = camera;
  id->blockShader->setMat4("view", view);

  glm::mat4 model;
  glm::vec3 newPos;
  newPos.x = xpos;
  newPos.y = ypos;
  newPos.z = zpos;
  std::cout << xpos << ypos << zpos << "\n";
  model = glm::translate(model, newPos);
  id->blockShader->setMat4("model", model);

  glBindVertexArray(id->VAO);
  glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT,0);
  glBindVertexArray(0);


}
