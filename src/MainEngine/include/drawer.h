#include "bsp.h"
#include "../../Objects/include/objects.h"
struct PointLight
{
  glm::vec3 position;

  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 diffuse;

  float constant;
  float linear;


  float quadratic;

  glm::mat4 shadowTransform[6];
  unsigned int depthMapFBO,depthCubemap;
};
struct DirLight
{
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  glm::mat4 lightSpaceMat;
  unsigned int depthMapFBO,depthMap;

};

class Drawer
{
private:
  uint glTexture;
  const char* texture;

  int vertRenderDistance, horzRenderDistance,renderBuffer;


  std::vector<std::shared_ptr<Object>> objList;
  std::vector<PointLight> lightList;

  DirLight dirLight;
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),16.0f/9.0f,1.0f,25.0f);
  glm::mat4 viewProj;
  glm::mat4 hsrMat;
  glm::mat4 viewMat;
  glm::vec3 viewPos;

  Shader objShader,dirDepthShader,pointDepthShader,blockShader;
  Shader debugDepthQuad;
public:
  int screenWidth,screenHeight;
  std::shared_ptr<BSPNode> frontNode;
  void setupShadersAndTextures(int screenWidth, int screenHeight);
  void setRenderDistances(int vert,int horz,int buffer);
  void renderDirectionalDepthMap();
  void renderPointDepthMap(int id);
  void startPointShadowDraw(Shader* shader, int id);
  void renderDirectionalShadows();
  void bindDirectionalShadows(Shader* shader);
  void renderPointShadows();
  void bindPointShadows();
  void setLights(Shader* shader);
  void setupSockets(std::string ipAddress);
  void addCube(Cube newCube);
  std::shared_ptr<Object> getObject(int id) {return objList[id];}
  void drawObjects();
  void drawFinal();
  void updateViewMatrix(glm::mat4 viewMatrix,glm::mat4 hsrMatrix,glm::vec3 viewPosition)
  {
    viewMat = viewMatrix;
    hsrMat = hsrMatrix;
    viewPos = viewPosition;
  }
  void addLight(glm::vec3 pos,
           glm::vec3 amb = glm::vec3(0.2f,0.2f,0.2f),
           glm::vec3 spe = glm::vec3(1.0f,1.0f,1.0f),
           glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
           float cons = 1.0f, float lin = 0.14f, float quad = 0.07f)
  {
    lightList.push_back(PointLight{pos,amb,spe,dif,cons,lin,quad});
    renderPointDepthMap(lightList.size()-1);
  }

  void createDirectionalLight(glm::vec3 dir = glm::vec3(-1.0f,-1.0f,-1.0f),
                              glm::vec3 amb = glm::vec3(0.8f,0.8f,0.8f),
                              glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                              glm::vec3 spec = glm::vec3(1.0f,1.0f,1.0f))
  {
    dirLight = {dir,amb,dif,spec};
    renderDirectionalDepthMap();
  }
  void drawTerrain(Shader* shader = NULL, bool useHSR = false);
  void drawPlayers(Shader* shader);



};
