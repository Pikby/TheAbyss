#include "headers/SOIL.h"
#include <iostream>
#include <math.h>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "headers/shaders.h"
#include "headers/openGL.h"
#include "headers/block.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
  glfwInit();


  int winWidth = 800;
  int winHeight = 600;
  GLFWwindow* window = createWindow(winWidth,winHeight);

  glfwSetKeyCallback(window, key_callback);

  //Intialize glew and all related settings
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0,0,winWidth,winHeight);
  glEnable(GL_DEPTH_TEST);

  Block testBlock(0,0,"../assets/textures/tilesf1.jpg",STREAM);
  Block testBlock2(0.2,0.2,"../assets/textures/tilesf1.jpg",STATIC);
  Block* gameGrid;




  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //testBlock.xpos += 0.001;
    //testBlock.refresh();
    testBlock.draw();
    testBlock2.draw();

    glfwSwapBuffers(window);
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

}
