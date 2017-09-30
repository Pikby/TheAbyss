class Shader
{
public:
  //The actual shader program
  GLuint Program;

  //Create the shader with a path to the vertex and fragment shaders
  Shader(const GLchar* vertexPath, const GLchar* fragmentShader);

  //Command to use the program
  void use();

  void setMat4(const char* name, glm::mat4 value);
  void setVec3(const char* name, glm::vec3 value);
  void setVec3(const char* name, float x, float y, float z);
  void setInt(const char* name, int x);
};
