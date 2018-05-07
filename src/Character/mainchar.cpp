
#include "../headers/all.h"

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

MainChar::MainChar(float x, float y, float z, World* world )
{
  mainCharShader = Shader("../src/Shader/shaders/shaderBlocks.vs","../src/Shader/shaders/shaderBlocks.fs");
  xpos = x;
  ypos = y;
  zpos = z;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  moveSpeed = 0.5f;
  curWorld = world;
  mainCam = Camera(glm::vec3(xpos,ypos,zpos));
  calculateHud();
  gui = GUIRenderer(screenWidth,screenHeight);
  std::cout << screenWidth << "\n" << screenHeight <<"\n";


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
  glm::vec4 block = rayCast(mainCam.position,mainCam.front,reach);

  if(block.w == NOTHING) return;
  curWorld->createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));

}
void MainChar::addBlock(int id)
{
    glm::vec4 block = rayCast(mainCam.position,mainCam.front,reach);
    if(block.w == NOTHING) return;

    glm::vec3 p1 = glm::vec3(block);
    glm::vec3 p2 = p1 - mainCam.front/10.0f;

    int x = floor(p1.x)-floor(p2.x);
    int y = floor(p1.y)-floor(p2.y);
    int z = floor(p1.z)-floor(p2.z);

    if(x != 0)
    {
      curWorld->addBlock(floor(p1.x)-sign(x),floor(p1.y),floor(p1.z),id);
    }
    else if(y != 0)
    {
      curWorld->addBlock(floor(p1.x),floor(p1.y)-sign(y),floor(p1.z),id);
    }
    else if(z != 0)
    {
      curWorld->addBlock(floor(p1.x),floor(p2.y),floor(p1.z)-sign(z),id);
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
