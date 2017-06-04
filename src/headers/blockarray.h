

class BlockArray
{
private:
  Block* type;
  int numb;
  std::vector<float> coords;
public:
  BlockArray(const char* newTexture, int blockType);
  glm::vec3 getPos(int index);
  void addBlock(float xPos,float yPos, float zPos);
  void draw(glm::mat4 camera);
};
