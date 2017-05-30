#include "SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "shaders.h"
#include "openGL.h"
#include "block.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
  glfwInit();


  int winWidth = 800;
  int winHeight = 600;
  GLFWwindow* window = createWindow(winWidth,winHeight);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0,0,winWidth,winHeight);
  glEnable(GL_DEPTH_TEST);

  std::cout << "flag one\n";
  block testBlock(0,0,"textures/tilesf1.jpg");
  block testBlock2(0.2,0.2,"textures/tilesf1.jpg");
  std::cout << "flag two\n";
  block* gameGrid;
  /*
  std::cout << "How many blocks do you want to lay?\n";
  int answer;
  std::cin << answer;

  gameGrid = new block[answer];
  int x;
  for(int x = 0; x<answer, x++)
  {
    int xCord, yCord
    std::cin >> xCord;
    std::cin >> yCord;
    gameGrid[x] = new block
  }

*/


  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    testBlock.draw();
    testBlock2.draw();
    glfwSwapBuffers(window);
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

}
