
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include <vector>

#include "../TextureLoading/textureloading.h"
#include "../headers/shaders.h"
#include "include/skybox.h"

static const float skyboxVertices[] =
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


SkyBox::SkyBox(const std::string &filePath)
{
  skyboxShader = std::make_unique<Shader>(Shader("skyShader.vs","skyShader.fs"));
  std::vector<std::string> faces
  {
    filePath + "_bk.tga",
    filePath + "_ft.tga",
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
    std::cout << faces[i] << "\n";
    unsigned char *data = loadTexture(faces[i].c_str(), &width,&height,&nrChannels);
    if(data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                  0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      freeTexture(data);
    }
    else
    {
      std::cout << "Cant find texture "  << faces[i].c_str() << "\n";
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

void SkyBox::draw(const glm::mat4 &view)
{

  skyboxShader->use();
  skyboxShader->setMat4("projection",projectMat);
  skyboxShader->setMat4("view", glm::mat4(glm::mat3(view)));
  glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}
