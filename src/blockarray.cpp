#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "headers/shaders.h"
#include "headers/block.h"
#include "headers/blockarray.h"


BlockArray::BlockArray(const char* newTexture, int blockType)
{
  type = new Block(newTexture,blockType);
  numb = 0;
}

glm::vec3 BlockArray::getPos(int index)
{
  if(index>numb) return glm::vec3(0,0,0);
  else return glm::vec3(coords.at(index*3),coords.at(index*3+1),coords.at(index*3+2));
}

void BlockArray::addBlock(float xPos, float yPos, float zPos)
{
  coords.push_back(xPos);
  coords.push_back(yPos);
  coords.push_back(zPos);
  numb++;
}

void BlockArray::draw(glm::mat4 camera)
{
  glBindTexture(GL_TEXTURE_2D, type->glTexture);
  type->blockShader->Use();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/ (float)600, 0.1f, 100.0f);
  type->blockShader->setMat4("projection", projection);

  glm::mat4 view = camera;
  type->blockShader->setMat4("view", view);


  glBindVertexArray(type->VAO);
  for(int x=0;x<numb;x+=3)
  {
    glm::mat4 model;
    model = glm::translate(model, getPos(x));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    //std::cout << coords.at(x) << coords.at(x+1) << coords.at(x+2);
    type->blockShader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
  }
}
