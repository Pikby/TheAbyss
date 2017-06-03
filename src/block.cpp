
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


glm::vec3 cameraPos   = glm::vec3(0.0f,0.0f,3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f,1.0f,0.0f);


Block::Block(float x, float y, const char* newTexture, int newType)
{
  blockShader = new Shader("../src/shaders/shader.vs","../src/shaders/shader.fs");
  xpos = x;
  ypos = y;
  texture = newTexture;
  type = newType;
  refresh();
}

void Block::refresh()
{
  float scrPosX = xpos;
  float scrPosY = ypos;


  GLfloat vertices[] =
  {
    scrPosX,            scrPosY,     0.0f,  0.0f,0.0f,
    scrPosX+0.125f,     scrPosY,     0.0f,  0.0f,1.0f,
    scrPosX,       scrPosY+0.125f,   0.0f,  1.0f,0.0f,
    scrPosX+0.125f,  scrPosY+0.125f, 0.0f,  1.0f,1.0f,
    scrPosX,            scrPosY,     0.125f,1.0f,1.0f,
    scrPosX+0.125f,     scrPosY,     0.125f,1.0f,0.0f,
    scrPosX,       scrPosY+0.125f,   0.125f,0.0f,1.0f,
    scrPosX+0.125f,  scrPosY+0.125f, 0.125f,0.0f,0.0f
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
    1,4,5,

    //lside
    0,2,6,
    0,4,6,

    //rside
    1,3,7,
    1,5,6,

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

void Block::draw(glm::mat4 camera)
{
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/ (float)600, 0.1f, 100.0f);
  blockShader->setMat4("projection", projection);

  glm::mat4 view = camera;
  blockShader->setMat4("view", view);

  glBindTexture(GL_TEXTURE_2D, glTexture);
  blockShader->Use();

  glm::mat4 model;
  model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
  blockShader->setMat4("model", model);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
  glBindVertexArray(0);


}
