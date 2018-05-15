

class Entity
{
protected:

  int hp;
  std::string name;
  std::atomic<float> xpos;
  std::atomic<float> ypos;
  std::atomic<float> zpos;
  unsigned int VAO;
  unsigned int VBO;
  bool isRendered;
  Shader entShader;
public:
  virtual void draw()
  {
    std::cout << "UHHHH\n";
  }
  void setPostion(float x,float y,float z)
  {
    xpos = x;
    ypos = y;
    zpos = z;
  }
};

class Player : public Entity
{
public:
  Player(){};
  Player(glm::vec3 newPos);
  void render();
  void draw(glm::mat4* view);


};
