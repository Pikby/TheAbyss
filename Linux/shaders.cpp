#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#include "shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
  //Retrieving the shader code
  //Variables to hold the code
  std::string vertexCode;
  std::string fragmentCode;

  //Variables to retrieve the code from the file path
  std::ifStream vertexShaderFile;
  std::ifStream fragmentShaderFile;

  vertexShaderFile.open(vertexPath);
  fragmentShaderFile.open(fragmentPath);

  std::stringstream vertexShaderStream, fragmentShaderStream;
  vertexShaderStream << vertexShaderFile.rdbuf();
  fragmentShaderStream << fragmentShaderFile.redbuf();

  //Close the files
  vertexShaderFile.close();
  fragmentShaderFile.close();

  //Extrace the code from the stream
  vertexCode = vertexShaderStream.str();
  fragmentCode = fragmentShaderStream.str();

  //Translate the code into openGL terms
  const GLchar* vertexShaderCode = vertexCode.c_str();
  const GLchar* fragmentShaderCode = fragmentCode.c_str();


  //Compile the shaders
  GLuint vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexShaderCode, NULL);
  glCompileShader(vertex);

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertex, 1, &fragmentShaderCode, NULL);
  glCompileShader(vertex);

  //Create the shader program
  this->Program = glCreateProgram();
  glAttachShader(this->Program, vertex);
  glAttachShader(this->Program, fragment);
  glLinkProgram(this->Program);

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Use()
{
  glUseProgram(this->Program);
}
