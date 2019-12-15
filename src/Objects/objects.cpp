
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "../headers/camera.h"
#include "../headers/shaders.h"

#include "include/objects.h"
#include "../TextureLoading/textureloading.h"

Shader Object::shader;
unsigned int Cube::VAO, Cube::VBO, Cube::EBO;
unsigned int Player::VAO, Player::VBO, Player::EBO;
unsigned int WireframeCube::VAO,WireframeCube::VBO,WireframeCube::EBO;
void Object::initObjects()
{
  Cube::render();
  WireframeCube::render();
  Player::render();
}

void Object::initializeObjectShader()
{
  shader = Shader("EntityShaders/entShader.vs","EntityShaders/entShader.fs");
  shader.use();
  shader.setInt("curTexture",0);
  shader.setInt("dirLight.shadow",4);
  shader.setInt("pointLights[0].shadow",5);
  shader.setInt("pointLights[1].shadow",6);
  shader.setFloat("far_plane",25.0f);
}

void Object::updateMatrices(Camera& camera)
{
  shader.use();
  shader.setMat4("projection",camera.getProjectionMatrix());
  shader.setMat4("view",camera.getViewMatrix());
}

  float cubeVertices[] =
  {
      // Back face
      -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
       1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
       1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
      -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
      // Front face
      -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
       1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
      // Left face
      -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
      -1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
      -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
      // Right face
       1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
       1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
       1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
       1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
      // Bottom face
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
       1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
       1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
      -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
      // Top face
      -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
       1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
  };


unsigned cubeIndices[] =
{
  0,1,2,
  3,1,0,

  4,5,6,
  6,7,4,

  8,9,10,
  10,11,8,

  12,13,14,
  13,12,15,

  16,17,18,
  18,19,16,

  20,21,22,
  21,20,23,
};


void Cube::render()
{


  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void WireframeCube::render()
{

  float cubeVertices[] =
  {
      // Back face
      -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
       1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
      // Front face
      -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
       1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left

  };

  unsigned cubeIndices[] =
  {
    0,1,1,2,
    2,3,3,0,

    4,5,5,6,
    6,7,7,4,

    0,4,1,5,
    2,6,3,7


  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);


  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}

void WireframeCube::draw(const glm::vec3 &viewPos)
{
  //return;
  glm::mat4 model = glm::translate(glm::mat4(1),pos-viewPos);
  model = glm::scale(model,glm::vec3(scale));
  shader.use();
  shader.setMat4("modelMat",model);
  shader.setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));

  glDisable(GL_CULL_FACE);
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES,24,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
  glEnable(GL_CULL_FACE);
}


void Cube::draw(const glm::vec3 &viewPos)
{
  glm::mat4 model = glm::translate(glm::mat4(1),pos-viewPos);
  model = glm::scale(model,glm::vec3(scale));
  shader.use();
  shader.setMat4("modelMat",model);
  shader.setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

}

void Player::render()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Player::draw(const glm::vec3 &viewPos)
{

  glm::mat4 model = glm::translate(glm::mat4(1),pos-viewPos);
  glm::vec3 defaultFace = glm::vec3(0,1,0);
  glm::mat4 rot1 =  glm::mat4(1);
  if(defaultFace != curFacing)
  {
    glm::vec3 norm = glm::normalize(glm::cross(defaultFace,curFacing));
    float angle = glm::angle(glm::normalize(curFacing),glm::normalize(defaultFace));
    //std::cout << glm::to_string(norm) << angle << "\n";
    rot1 = glm::rotate(angle,norm);
  }



  glm::mat4 rot2 = glm::mat4(1);
  if(curFacing != nextFacing)
  {
    glm::vec3 norm = glm::normalize(glm::cross(curFacing,nextFacing));
    float angle = glm::angle(glm::normalize(curFacing),glm::normalize(nextFacing));

    double curTime = glfwGetTime();
    float alpha = (rotateTimeEnd - glfwGetTime())/(rotateTimeEnd - rotateTimeStart);
  }


  glm::mat4 rotationMat = rot1 * rot2;

  model  = model*rotationMat;

  shader.use();
  shader.setMat4("modelMat",model);
  shader.setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

  glm::mat4 faceModel = glm::translate(model,glm::vec3(0,1.5,0));
  faceModel = glm::scale(faceModel,glm::vec3(0.5));
  shader.setMat4("modelMat",faceModel);
  shader.setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

}
