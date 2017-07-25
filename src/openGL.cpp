#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/shaders.h"
#include "headers/bsp.h"
#include "headers/openGL.h"




GLFWwindow* createWindow(int width, int height)
{
  //Initial windows configs
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  //Create the window
  GLFWwindow* window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  return window;
}


void generateWhiteNoise(int width, int height, float** array)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y<width; y++)
        {
            array[x][y] = (float)(rand()%1000)/(float)1000;
        }
    }
}

void generateSmoothNoise(int width, int height, float** array, int octave)
{
  int period = pow(2,octave);
  float freq = 1.0f/period;

  float** newArray;
  newArray = new float *[width];
  for(int i = 0; i <height; i++)
    newArray[i] = new float[16];

  for(int x = 0;x<width;x++)
  {
    int x0 = (x/period)*period;
    int x1 = (x0+period) % width;
    float horzBlend = (x-x0) * freq;

    for(int y = 0; y<height;y++)
    {
      int y0 = (y/period)*period;
      int y1 = (y0+period) % height;
      float vertBlend = (y-y0) * freq;

      float top = interpolate(array[x0][y0],array[x1][y0],horzBlend);
      float bottom = interpolate(array[x0][y1],array[x1][y1],horzBlend);

      newArray[x][y] = interpolate(top,bottom,vertBlend);
      std::cout << newArray[x][y] << "\n";
    }
  }

  for(int x=0;x<width;x++)
    for(int y=0;y<height;y++)
    {
      array[x][y] = newArray[x][y];
    }

}

void generateLand(int width, int height, float** array, int octCount)
{
  generateWhiteNoise(width,height,array);
  for(int i = 0; i<octCount;i++)
  {
    generateSmoothNoise(width,height,array,i);
  }
}

float interpolate(float x0,float x1, float alpha)
{
  std::cout << "x0 is: " << x0 << "x1: " <<x1 << "alpha: " << alpha << "\n";
  return x0*(1-alpha) + alpha*x1;
}
