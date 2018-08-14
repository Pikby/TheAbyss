
union IntOrFloat
{
  int i;
  float f;
};
struct InMessage
{
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  int length;
  InMessage(){};
  InMessage(uchar a, uchar b, uchar c, uchar d, int xpos, int ypos, int zpos,int len)
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
  InMessage(uchar a, uchar b, uchar c, uchar d, float xpos, float ypos, float zpos,int len)
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
  uchar opcode;
  uchar ext1;
  uchar ext2;
  uchar ext3;
  IntOrFloat x;
  IntOrFloat y;
  IntOrFloat z;
  std::shared_ptr<std::string> data;
  OutMessage();
  OutMessage(uchar a, uchar b, uchar c, uchar d, int xpos, int ypos, int zpos,std::shared_ptr<std::string> newData)
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
  OutMessage(uchar a, uchar b, uchar c, uchar d, float xpos,float ypos, float zpos,std::shared_ptr<std::string> newData)
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
