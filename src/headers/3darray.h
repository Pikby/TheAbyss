template <typename T, int N>
class Array3D
{
  private:
    T array[N*N*N];
  public:
    Array3D()
    {
      memset(array,0,sizeof(array));
    }
    T get(int x,int y, int z)
    {
      return array[x+y*N+z*N*N];
    }
    void set(int x,int y, int z, T value)
    {
      array[x+y*N+z*N*N] = value;
    }
    T &operator[](int i)
    {
      return array[i];
    }

};
