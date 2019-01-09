#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef unsigned int uint;

class Shader
{
  private:
    unsigned int id;
    void compileShader(std::string path)
    {
      std::string extension = path.substr(path.rfind("."),path.size());
      std::string code;

      std::ifstream shaderFile;

      shaderFile.open(path);

      if(!shaderFile.is_open())
      {
        std::cout << "Shader path wrong: " << path << "\n";
        return;
      }

      std::stringstream shaderStream;
      shaderStream << shaderFile.rdbuf();

      shaderFile.close();

      code = shaderStream.str();

      const char* shaderCode = code.c_str();
      uint shader;
      int success;
      char infoLog[512];

      std::string shaderType;
      if(extension == ".vs")
      {
        shader = glCreateShader(GL_VERTEX_SHADER);
        shaderType = "Vertex";
      }
      else if(extension == ".fs")
      {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        shaderType = "Fragment";
      }
      else if(extension == ".gs")
      {
        shader = glCreateShader(GL_GEOMETRY_SHADER);
        shaderType = "Geometry";
      }
      else std::cout << "INVALID SHADER EXTENSION" << extension << "\n";
      glShaderSource(shader, 1, &shaderCode, NULL);
      glCompileShader(shader);


      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if(!success)
      {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::" + shaderType + "SHADER::COMPILATION_FAILED IN SHADER" +path+ "\n" << infoLog << std::endl;
        return;
      }

      //Create the shader program

      glAttachShader(id, shader);

      glDeleteShader(shader);
    }
  public:
    Shader(){};
    Shader(std::string shader1, std::string shader2 = "",std::string shader3 = "")
    {
      id = glCreateProgram();
      compileShader(shader1);
      if(shader2 != "") compileShader(shader2);
      if(shader3 != "") compileShader(shader3);
      glLinkProgram(id);
    }

    void use()
    {
      glUseProgram(id);
    }

    void setMat4(std::string name, const glm::mat4 &value)
    {
      glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setVec3(std::string name, const glm::vec3 &value)
    {
      glUniform3f(glGetUniformLocation(id, name.c_str()),value.x,value.y,value.z);
    }

    void setVec3(std::string name, float x, float y, float z)
    {
      glUniform3f(glGetUniformLocation(id, name.c_str()),x,y,z);
    }

    void setInt(std::string name, int x)
    {
      glUniform1i(glGetUniformLocation(id, name.c_str()),x);
    }
    void setBool(std::string name, bool val)
    {
      glUniform1i(glGetUniformLocation(id, name.c_str()),val);
    }
    void setFloat(std::string name, float val)
    {
      glUniform1f(glGetUniformLocation(id,name.c_str()),val);
    }
    void setUInt(std::string name, unsigned int x)
    {
      glUniform1ui(glGetUniformLocation(id,name.c_str()),x);
    }
    void setIVec2(std::string name, const glm::ivec2 &val)
    {
      glUniform2i(glGetUniformLocation(id,name.c_str()),val.x,val.y);
    }
};
