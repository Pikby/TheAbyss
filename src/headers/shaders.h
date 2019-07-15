#ifndef GLSLSHADERS
#define GLSLSHADERS


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
protected:
    static std::string filePath;
    unsigned int id;

    std::string preProcess(std::string code,std::string path)
    {
      std::size_t place = code.find("#include");
      while(place != std::string::npos)
      {
        std::size_t endl = code.find('\n',place);
        std::string includePath = code.substr(place,endl-place);
        code.erase(place,endl-place);


        std::size_t end = path.rfind('/');
        std::string dir = path.substr(0,path.rfind('/'));



        std::size_t first = includePath.find('\"')+1;
        std::size_t last = includePath.rfind('\"');
        std::string fullPath = dir + '/' + includePath.substr(first,last-first);

        std::ifstream includeFile;
        includeFile.open(fullPath);
        if(!includeFile.is_open())
        {
          std::cout << "include path wrong: " << fullPath << "\n";
          return "";
          throw -1;
        }

        std::stringstream includeStream;
        includeStream << includeFile.rdbuf();
        std::string preProcessStr = preProcess(includeStream.str(),fullPath);
        code.insert(place,preProcessStr);

        place = code.find("#include");
      }
      return code;
    }



    void compileShader(std::string path)
    {
      std::string extension = path.substr(path.rfind("."),path.size());

      std::ifstream shaderFile;

      shaderFile.open(path);

      if(!shaderFile.is_open())
      {
        std::cout << "Shader path wrong: " << path << "\n";
        return;
        throw -1;
      }

      std::stringstream shaderStream;
      shaderStream << shaderFile.rdbuf();

      shaderFile.close();

      std::string code = shaderStream.str();
      std::string preProcessCode = preProcess(code,path);
      const char* shaderCode = preProcessCode.c_str();
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

    static void setShaderDirectory(const std::string& FilePath)
    {
      filePath = FilePath;
    }
    Shader(){};
    Shader(const std::string &shader1,const std::string &shader2 = "",const std::string &shader3 = "")
    {
      id = glCreateProgram();
      compileShader(filePath+ shader1);
      if(shader2 != "") compileShader(filePath+ shader2);
      if(shader3 != "") compileShader(filePath+ shader3);
      glLinkProgram(id);

      int success;
      char infoLog[512];
      glGetProgramiv(id, GL_LINK_STATUS, &success);
           if(!success)
           {
               glGetProgramInfoLog(id, 1024, NULL, infoLog);
               std::cout << shader1 << ":  ERROR::PROGRAM_LINKING_ERROR"<< "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

               throw -1;
           }

    }


    void use()
    {
      glUseProgram(id);
    }

    void setMat4(const std::string &name, const glm::mat4 &value)
    {
      glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setMat3(const std::string &name, const glm::mat3 &value)
    {
      glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setVec3(const std::string &name, const glm::vec3 &value)
    {
      glUniform3f(glGetUniformLocation(id, name.c_str()),value.x,value.y,value.z);
    }

    void setVec3(const std::string &name, float x, float y, float z)
    {
      glUniform3f(glGetUniformLocation(id, name.c_str()),x,y,z);
    }

    void setInt(const std::string &name, int x)
    {
      glUniform1i(glGetUniformLocation(id, name.c_str()),x);
    }
    void setBool(const std::string &name, bool val)
    {
      glUniform1i(glGetUniformLocation(id, name.c_str()),val);
    }
    void setFloat(const std::string &name, float val)
    {
      glUniform1f(glGetUniformLocation(id,name.c_str()),val);
    }
    void setUInt(const std::string &name, unsigned int x)
    {
      glUniform1ui(glGetUniformLocation(id,name.c_str()),x);
    }
    void setIntArray(const std::string &name,const int* arr,int size)
    {
      glUniform1iv(glGetUniformLocation(id,name.c_str()),size,arr);
    }
    void setVec2(const std::string &name, const glm::vec2 & val)
    {
      glUniform2f(glGetUniformLocation(id,name.c_str()),val.x,val.y);
    }
    void setIVec2(const std::string &name, const glm::ivec2 &val)
    {
      glUniform2i(glGetUniformLocation(id,name.c_str()),val.x,val.y);
    }
    void setVec4(const std::string &name, const glm::vec4 &val)
    {
      glUniform4f(glGetUniformLocation(id,name.c_str()),val.x,val.y,val.z,val.w);
    }
};

class ShaderFeedback : public Shader
{
public:
  ShaderFeedback(){};
  ShaderFeedback(const std::string &shader1,const std::string &shader2 = "",const std::string &shader3 = "")
  {
    id = glCreateProgram();
    compileShader(filePath+ shader1);
    if(shader2 != "") compileShader(filePath+ shader2);
    if(shader3 != "") compileShader(filePath+ shader3);

    const char* feedback[] = { "outValue" };
    glTransformFeedbackVaryings(id,1,feedback,GL_INTERLEAVED_ATTRIBS);
    glLinkProgram(id);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
         if(!success)
         {
             glGetProgramInfoLog(id, 1024, NULL, infoLog);
             std::cout << shader1 << "ERROR::PROGRAM_LINKING_ERROR"<< "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

             throw -1;
         }

  }

  int getProgram()
  {
    return id;
  }
};

#ifdef GLSLSHADERSIMPLEMNTATION
std::string Shader::filePath;
#endif
#endif
