#include <glm/glm.hpp>
#include <memory>

class Shader;
class SkyBox
{
private:
  unsigned int VAO, VBO,texID;
  glm::mat4 projectMat;
  std::unique_ptr<Shader> skyboxShader;
  public:
  SkyBox(const std::string &filePath);
  SkyBox(){};
  void draw(const glm::mat4 &view);

};
