


class Block
{
public:
  int id;
  int texArray[12];
  int width;
  int height;
  int atlasWidth;
  int atlasHeight;

  void getTop(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[0]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[1]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[0]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[1]+1);
  };


  void getBottom(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[2]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[3]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[2]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[3]+1);
  };

  void getLeft(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[4]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[5]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[4]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[5]+1);
  };

  void getRight(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[6]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[7]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[6]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[7]+1);
  };

  void getFront(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[8]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[9]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[8]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[9]+1);
  };

  void getBack(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[10]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[11]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[10]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[11]+1);
  };

};
