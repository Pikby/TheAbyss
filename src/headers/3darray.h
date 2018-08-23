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
    T &get(const glm::ivec3 &pos)
    {
      //if(x >= N || y >= N || z >= N) std::cout << "OUTOF ARRAY RANGE" << x << ":" << y << ":" << z << "\n";
      return array[pos.x*N*N+pos.z*N+pos.y];
    }
    void set(const glm::ivec3 &pos, T value)
    {
      //if(x >= N || y >= N || z >= N) std::cout << "OUTOF ARRAY RANGE" << x << ":" << y << ":" << z << "\n";
      array[pos.x*N*N+pos.z*N+pos.y] = value;
    }
    T &operator[](int i)
    {
      return array[i];
    }

};
