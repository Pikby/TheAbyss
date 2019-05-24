#include "include/objects.h"
#include "../headers/camera.h"
#include "../TextureLoading/textureloading.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtx/string_cast.hpp>

unsigned int Cube::VAO, Cube::VBO, Cube::EBO;
unsigned int Player::VAO, Player::VBO, Player::EBO;
float skyboxVertices[] =
{
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};



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


void Cube::draw(Shader* shader, const glm::vec3 &viewPos)
{
  glm::mat4 model = glm::translate(modelMat,-viewPos);
  shader->use();
  shader->setMat4("modelMat",model);
  shader->setVec3("objectColor",glm::vec3(0.5,0.5f,0.5f));
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

void Player::draw(Shader* shader,const glm::vec3 &viewPos)
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

  shader->use();
  shader->setMat4("modelMat",model);
  shader->setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

  glm::mat4 faceModel = glm::translate(model,glm::vec3(0,1.5,0));
  faceModel = glm::scale(faceModel,glm::vec3(0.5));
  shader->setMat4("modelMat",faceModel);
  shader->setVec3("objectColor",glm::vec3(0.5f,0.5f,0.5f));
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

}

SkyBox::SkyBox(const std::string &filePath)
{
  skyboxShader = Shader("skyShader.vs","skyShader.fs");
  std::vector<std::string> faces
  {
    filePath + "_ft.tga",
    filePath + "_bk.tga",
    filePath + "_up.tga",
    filePath + "_dn.tga",
    filePath + "_rt.tga",
    filePath + "_lf.tga"
  };

  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texID);


  int width, height, nrChannels;
  for(int i =0; i < 6;i++)
  {
    unsigned char *data = loadTexture(faces[i].c_str(), &width,&height,&nrChannels);
    if(data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                  0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      freeTexture(data);
    }
  }


  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),skyboxVertices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  projectMat = glm::perspective(glm::radians(90.0f),
                  (float)1920/ (float)1080, 0.1f, 10.0f);

}

void SkyBox::draw(glm::mat4* view)
{

  skyboxShader.use();
  skyboxShader.setMat4("projection",projectMat);
  skyboxShader.setMat4("view", glm::mat4(glm::mat3(*view)));
  glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}
