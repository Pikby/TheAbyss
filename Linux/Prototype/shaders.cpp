#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
  //Retrieving the shader code
  //Variables to hold the code
  std::string vertexCode;
  std::string fragmentCode;

  //Variables to retrieve the code from the file path
  std::ifstream vertexShaderFile;
  std::ifstream fragmentShaderFile;

  vertexShaderFile.open(vertexPath);
  fragmentShaderFile.open(fragmentPath);

  std::stringstream vertexShaderStream, fragmentShaderStream;
  vertexShaderStream << vertexShaderFile.rdbuf();
  fragmentShaderStream << fragmentShaderFile.rdbuf();

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
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexShaderCode, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
  glCompileShader(fragment);

  success = 0;
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

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

void Shader::setMat4(const char* name, glm::mat4 value)
{
  glUniformMatrix4fv(glGetUniformLocation(this->Program, name), 1, GL_FALSE, glm::value_ptr(value));
}
