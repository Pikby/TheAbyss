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

MainChar::MainChar(float x, float y, float z, World* curWorld )
{
  mainCharShader = new Shader("../src/shaders/shaderBlocks.vs","../src/shaders/shaderBlocks.fs");
  xpos = x/10;
  ypos = y/10;
  zpos = z/10;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  moveSpeed = 0.1f;
  zaWarudo = curWorld;

  mainCam = Camera(glm::vec3(xpos/10,ypos/10,zpos/10));
}

void MainChar::update()
{
    xpos += deltax;
    zpos += deltaz;

    deltax /= 5;
    deltay /= 5;
    deltaz /= 5;
  std::cout << xpos*10 << "," << ypos*10 << "," << zpos*10 << "\n";

  mainCam.setPosition(xpos,ypos,zpos);
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
  if(grounded) deltay = 0.05f;
}
