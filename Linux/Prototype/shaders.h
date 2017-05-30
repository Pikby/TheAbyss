class Shader
{
public:
  //The actual shader program
  GLuint Program;

  //Create the shader with a path to the vertex and fragment shaders
  Shader(const GLchar* vertexPath, const GLchar* fragmentShader);

  //Command to use the program
  void Use();
};
