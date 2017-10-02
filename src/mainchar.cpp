enum Type {STATIC,DYNAMIC,STREAM};

#include "headers/all.h"

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
  moveSpeed = 5.0f;
  curWorld = world;

  mainCam = Camera(glm::vec3(xpos,ypos,zpos));
}

void MainChar::update()
{
  //std::cout << xpos /10.0f<<":"<<ypos/10.0f<<":"<<zpos/10.0f<<"\n";
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


    deltax /= 5;
    deltay /= 5;
    deltaz /= 5;


  mainCam.setPosition(xpos,ypos+0.1,zpos);
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

void MainChar::moveDown()
{
  deltay = -5;
}

void MainChar::moveUp()
{
  deltay = 5;
}

void MainChar::jump()
{
  if(grounded) deltay = 5.0f;
}
