
union IntOrFloat
{
  int i;
  float f;
};
struct InMessage
{
  uint8_t opcode;
  uint8_t ext1;
  uint8_t ext2;
  uint8_t ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  int length;
  InMessage(){};
  InMessage(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int xpos, int ypos, int zpos,int len)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.i = xpos;
    y.i = ypos;
    z.i = zpos;
    length = len;
  }
  InMessage(uint8_t a, uint8_t b, uint8_t c, uint8_t d, float xpos, float ypos, float zpos,int len)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.f = xpos;
    y.f = ypos;
    z.f = zpos;
    length = len;
  }
};
struct OutMessage
{
  uint8_t opcode;
  uint8_t ext1;
  uint8_t ext2;
  uint8_t ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  std::shared_ptr<std::string> data;
  OutMessage();
  OutMessage(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int xpos, int ypos, int zpos,std::shared_ptr<std::string> newData)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.i = xpos;
    y.i = ypos;
    z.i = zpos;
    data = newData;
  }
  OutMessage(uint8_t a, uint8_t b, uint8_t c, uint8_t d, float xpos,float ypos, float zpos,std::shared_ptr<std::string> newData)
  {
    opcode = a;
    ext1 = b;
    ext2 = c;
    ext3 = d;
    x.f = xpos;
    y.f = ypos;
    z.f = zpos;
    data = newData;
  }
};
