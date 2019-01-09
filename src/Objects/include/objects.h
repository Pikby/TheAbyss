#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../headers/shaders.h"
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/ext.hpp>
//#include <glm/gtx/string_cast.hpp>
#include <memory>

class Camera;
class Object
{
protected:
  glm::vec3 pos,scale,rotAxis,color;
  float angInRads;
  glm::mat4 modelMat;
  bool isRendered = false;
  public:
  void updateModelMat()
  {
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat,pos);
    modelMat = glm::rotate(modelMat,angInRads,rotAxis);
    modelMat = glm::scale(modelMat,scale);
  }

  Object(glm::vec3 p = glm::vec3(0,0,0),
                  glm::vec3 s = glm::vec3(1,1,1),
                  glm::vec3 r = glm::vec3(0,1,0),
                  float angInDegrees = 0)
  {
    pos = p;
    scale = s;
    rotAxis = r;
    angInRads = glm::radians(angInDegrees);
    updateModelMat();
    color = glm::vec3(1.0f,1.0f,1.0f);

  }
  void setRotation(const glm::vec3 &rot,const double &newAngInRads)
  {
    rotAxis = rot;
    angInRads = newAngInRads;
  }
  void setModelMatrix(const glm::mat4 &newModel)
  {
    modelMat = newModel;
    //updateModelMat();
  }
  void setPosition(const glm::vec3 &newPos)
  {
    pos = newPos;
  }
  virtual void draw(Shader* shader){
    std::cout << "ERROR DRAWING DEFAULT OBJECT\n";
  }
  virtual void render(){
    std::cout << "ERROR RENDERING DEFAULT OBJECT\n";
  }
  void setModel()
  {
  }
  void setColor(glm::vec3 newColor){color = newColor;}
};

typedef std::shared_ptr<Object> objPointer;



class Cube : public Object
{
private:
  static unsigned int VAO, VBO, EBO;
public:
  using Object::Object;

  void draw(Shader* shader);
  void render();
  bool isCollided(Cube* cube);
};

class Player : public Object
{
private:
  static unsigned int VAO, VBO, EBO;
public:
  using Object::Object;

  void draw(Shader* shader);
  void render();
  bool isCollided(Cube* cube);
};

class SkyBox
{
  private:
  unsigned int VAO, VBO,texID;
  glm::mat4 projectMat;
  Shader skyboxShader;
  public:
  SkyBox(const std::string &filePath);
  SkyBox(){};
  void draw(glm::mat4* view);

};
