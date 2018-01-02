
class Shader
{
public:
  //The actual shader program
  GLuint id;

  //Create the shader with a path to the vertex and fragment shaders
  Shader(const GLchar* vertexPath, const GLchar* fragmentShader);
  Shader(){};
  //Command to use the program
  void use();

  void setMat4(const char* name, glm::mat4 value);
  void setVec3(const char* name, glm::vec3 value);
  void setVec3(const char* name, float x, float y, float z);
  void setInt(const char* name, int x);
  void setBool(const char* name, bool val);
  void setFloat(const char* name, float val);
};
