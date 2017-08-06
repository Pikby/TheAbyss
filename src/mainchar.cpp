enum Type {STATIC,DYNAMIC,STREAM};

#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "headers/camera.h"
#include "headers/shaders.h"
#include "headers/bsp.h"
#include "headers/mainchar.h"

#define PI 3.14159265

MainChar::MainChar(float x, float y, float z, World* world )
{
  mainCharShader = new Shader("../src/shaders/shaderBlocks.vs","../src/shaders/shaderBlocks.fs");
  xpos = x;
  ypos = y;
  zpos = z;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  moveSpeed = 0.1f;
  curWorld = world;

  mainCam = Camera(glm::vec3(xpos/10,ypos/10,zpos/10));
}

void MainChar::update()
{
  std::cout << xpos<<":"<<ypos<<":"<<zpos<<"\n";
  deltay -= 0.1;

  if(!curWorld->blockExists(floor(xpos+deltax),floor(ypos),floor(zpos)))
  {
    xpos += deltax;
  }

  if(!curWorld->blockExists(floor(xpos),floor(ypos),floor(zpos+deltaz)))
  {
    zpos += deltaz;
  }

  if(!curWorld->blockExists(floor(xpos),floor(ypos+deltay),floor(zpos)))
  {
    ypos += deltay;
    grounded = false;
  }
  else
  {
    deltay = 0;
    grounded = true;
  }

    deltax /= 5;
    deltay /= 5;
    deltaz /= 5;


  mainCam.setPosition(xpos/10,ypos/10+0.1,zpos/10);
}

void MainChar::moveRight()
{
  deltax = cos((mainCam.yaw+90)*PI/180.0)*moveSpeed;
  deltaz = sin((mainCam.yaw+90)*PI/180.0)*moveSpeed;
}

void MainChar::moveLeft()
{
  deltax = cos((mainCam.yaw-90)*PI/180.0)*moveSpeed;
  deltaz = sin((mainCam.yaw-90)*PI/180.0)*moveSpeed;
}

void MainChar::moveForward()
{
  deltax = cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz = sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::moveBackward()
{
  deltax = -cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz = -sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::jump()
{
  if(grounded) deltay = 5.0f;
}
