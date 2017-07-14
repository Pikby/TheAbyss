

class BSP
{
private:
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  std::map<float, std::map<float, std::map<float, int>>> vertMap;
  std::map<int, std::map<int, std::map<int, int>>> worldMap;

  GLuint VBO, EBO, VAO;
  GLuint glTexture;
  Shader* blockShader;
  const char* texture;
  long int totalVertices;
public:
  BSP();
  bool addBlock(int x, int y, int z,int id);
  int addVertex(float x, float y, float z,float texX, float texY);
  void addIndices(int index1, int index2, int index3, int index4);
  int removeBlock(int x, int y, int z);
  void render();
  void draw(glm::mat4 camera);
  bool existsAt(int x,int y,int z);
};
