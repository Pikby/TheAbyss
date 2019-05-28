#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>



#include <memory>

class Shader;
class Object
{
protected:
  glm::vec3 pos,scale,curFacing = glm::vec3(0,1,0),nextFacing =  glm::vec3(0,1,0),color;
  double rotateTimeStart;
  double rotateTimeEnd;
  glm::quat curQuaternion = glm::quat(1,0,0,0);


  glm::mat4 rotationMat;


  glm::mat4 modelMat;
  bool isRendered = false;
  public:
  void updateModelMat()
  {
    /*
    modelMat = glm::translate(glm::mat4(1),pos);

    glm::vec3 norm = glm::normalize(glm::cross(curFacing,nextFacing));
    float angle = glm::angle(glm::normalize(curFacing),glm::normalize(nextFacing));
    glm::quat nextQuaternion = glm::angleAxis(angle,norm);

    glm::quat finalQuaternion;

    double curTime = glfwGetTime();

    if(curTime > rotateTimeEnd)
    {
      curFacing = nextFacing;
      finalQuaternion = nextQuaternion;
    }
    else if(rotateTimeEnd != rotateTimeStart)
    {
      float alpha = (rotateTimeEnd - glfwGetTime())/(rotateTimeEnd - rotateTimeStart);
      finalQuaternion = glm::slerp(curQuaternion,nextQuaternion,alpha);
    }
    glm::mat4 rotationMat = glm::toMat4(finalQuaternion);

    modelMat = rotationMat*modelMat;
    modelMat = glm::scale(modelMat,scale);
    */
  }

  Object(glm::vec3 p = glm::vec3(0,0,0),
                  glm::vec3 s = glm::vec3(1,1,1),
                  glm::vec3 r = glm::vec3(0,1,0))
                  : pos(p), scale(s) , curFacing(r)
  {
    updateModelMat();
    color = glm::vec3(1.0f,1.0f,1.0f);

  }
  void setFacing(const glm::vec3 &newFace, double timeInSeconds = 0)
  {
    curFacing = newFace;
    nextFacing = newFace;
    rotateTimeEnd = glfwGetTime() + timeInSeconds;
    rotateTimeStart = glfwGetTime();
  }
  void setModelMatrix(const glm::mat4 &newModel)
  {
    modelMat = newModel;
  }
  void setPosition(const glm::vec3 &newPos)
  {
    pos = newPos;
  }
  virtual void draw(Shader* shader,const glm::vec3 &viewPos)
  {
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

  void draw(Shader* shader,const glm::vec3 &viewPos);
  void render();
  bool isCollided(Cube* cube);
};

class Player : public Object
{
private:
  static unsigned int VAO, VBO, EBO;
public:
  using Object::Object;

  void draw(Shader* shader,const glm::vec3 &viewPos);
  void render();
  bool isCollided(Cube* cube);
};
