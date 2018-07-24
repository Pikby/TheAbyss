#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "../MainEngine/include/world.h"
#include "../headers/shaders.h"
#include "include/mainchar.h"

#define PI 3.14159265

template <typename T> int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}

float max(float a, float b, float c)
{
  float m = a;
  if(m<b) m = b;
  if(m<c) m = c;
  return m;

}

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
unsigned int oVAO, oVBO;

MainChar::MainChar(float x, float y, float z, World* world )
{
  mainCharShader = Shader("../src/Shaders/entShader.vs","../src/Shaders/entShader.fs");
  xpos = x;
  ypos = y;
  zpos = z;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  moveSpeed = 0.1f;
  curWorld = world;
  mainCam = Camera(glm::vec3(xpos,ypos,zpos));
  calculateHud();
  screenWidth = curWorld->drawer.screenWidth;
  screenHeight = curWorld->drawer.screenHeight;
  gui = GUIRenderer(screenWidth,screenHeight);
  std::cout << screenWidth << "\n" << screenHeight <<"\n";

  glGenVertexArrays(1, &oVAO);
  glGenBuffers(1, &oVBO);
  glBindVertexArray(oVAO);

  glBindBuffer(GL_ARRAY_BUFFER,oVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


void MainChar::update()
{
  //std::cout << xpos /10.0f<<":"<<ypos/10.0f<<":"<<zpos/10.0f<<"\n";
  if(!curWorld->blockExists(floor(xpos+deltax),floor(ypos),floor(zpos)))
  {
    xpos = xpos + deltax;
  }

  if(!curWorld->blockExists(floor(xpos),floor(ypos),floor(zpos+deltaz)))
  {
    zpos = zpos + deltaz;
  }

  if(!curWorld->blockExists(floor(xpos),floor(ypos+deltay),floor(zpos)))
  {
    ypos = ypos + deltay;
    grounded = false;
  }


    deltax /= 5;
    deltay /= 5;
    deltaz /= 5;


  mainCam.setPosition(xpos,ypos,zpos);


}

void MainChar::moveRight()
{
  deltax += cos((mainCam.yaw+90)*PI/180.0)*moveSpeed;
  deltaz += sin((mainCam.yaw+90)*PI/180.0)*moveSpeed;
}

void MainChar::moveLeft()
{
  deltax += cos((mainCam.yaw-90)*PI/180.0)*moveSpeed;
  deltaz += sin((mainCam.yaw-90)*PI/180.0)*moveSpeed;
}

void MainChar::moveForward()
{
  deltax += cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz += sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::moveBackward()
{
  deltax += -cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz += -sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::setPosition(float x, float y, float z)
{
  return;
  if(sqrt(pow(x-xpos,2)+pow(y-ypos,2)+pow(z-zpos,2) < 20)) return;
  xpos = x;
  ypos = y;
  zpos = z;
}

void MainChar::moveDown()
{
  deltay += -moveSpeed;
}

void MainChar::moveUp()
{
  deltay += moveSpeed;
}

void MainChar::jump()
{
  if(grounded) deltay = 5.0f;
}
//Destroys the block ur looking at
void MainChar::destroyBlock()
{
  glm::vec4 block = curWorld->rayCast(mainCam.position,mainCam.front,reach);

  if(block.w == NOTHING) return;
  std::cout << "Destroying blocks\n";
  curWorld->messenger.createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));

}
void MainChar::addBlock(int id)
{
    glm::vec4 block = curWorld->rayCast(mainCam.position,mainCam.front,reach);
    if(block.w == NOTHING) return;

    glm::vec3 p1 = glm::vec3(block);
    glm::vec3 p2 = p1 - mainCam.front/10.0f;

    int x = floor(p1.x)-floor(p2.x);
    int y = floor(p1.y)-floor(p2.y);
    int z = floor(p1.z)-floor(p2.z);

    if(x != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x)-sign(x),floor(p1.y),floor(p1.z),id);
    }
    else if(y != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x),floor(p1.y)-sign(y),floor(p1.z),id);
    }
    else if(z != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x),floor(p2.y),floor(p1.z)-sign(z),id);
    }

}

void MainChar::calculateHud()
{
  actionMain.topLeft = glm::vec2(screenWidth*0.05,screenHeight*0.05);
  actionMain.bottomRight = glm::vec2(screenWidth*0.95,screenHeight*0.15);
  actionMain.selected = 0;
  actionMain.width = 2;
}

void MainChar::drawHud()
{
  int topLeftx = actionMain.topLeft.x;
  int topLefty = actionMain.topLeft.y;
  int bottomRightx = actionMain.bottomRight.x;
  int bottomRighty = actionMain.bottomRight.y;
  int width = actionMain.width;

  gui.drawRectangle(topLeftx,topLefty,topLeftx+width,bottomRighty);
  gui.drawRectangle(topLeftx+width,topLefty,bottomRightx,topLefty+width);

  gui.drawRectangle(screenWidth/2+3,screenHeight/2+3,screenWidth/2-3,screenHeight/2-3);
  showFPS();
}

void MainChar::draw()
{
  mainCharShader.use();
  glViewport(0,0,screenWidth,screenHeight);
  glm::mat4 camProjection = glm::perspective(glm::radians(45.0f),
                            (float)1920/ (float)1080, 0.1f,
                            (float)7*CHUNKSIZE*4);
  glm::mat4 model;
  model = glm::translate(model,glm::vec3(xpos,ypos,zpos-3));
  mainCharShader.setMat4("projection",camProjection);
  mainCharShader.setMat4("view", mainCam.getViewMatrix());
  mainCharShader.setMat4("model", model);

  glBindVertexArray(oVAO);
  glDrawArrays(GL_TRIANGLES,0,36);
  glBindVertexArray(0);
}

void MainChar::showFPS()
{
  static double lastFrameTime = 0;
  static double curFrameTime  = 0;
  static double curFPS = 0;

  curFrameTime = glfwGetTime();
  double smooth = 0.9f;

  double deltaTime = 1.0f/(curFrameTime - lastFrameTime);

  curFPS = curFPS*smooth + deltaTime*(1-smooth);
  std::string fps = "FPS: " + std::to_string((int)round(curFPS));

  gui.renderText(fps,0,screenHeight*.9,1);
  lastFrameTime = glfwGetTime();
}
