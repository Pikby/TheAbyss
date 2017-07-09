
struct Character
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};


class CharRenderer
{
private:
  std::map<GLchar, Character> Characters;
  GLuint VAOText, VBOText;
  Shader* textShader;
public:
  CharRenderer();
  void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

};
