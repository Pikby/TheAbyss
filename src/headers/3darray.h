template <typename T, int N>
class Array3D
{
  private:
    T* array;
    //std::mutex arrayMutex;
  public:
    Array3D()
    {
      array = new T[N*N*N];
    }

    ~Array3D()
    {
      delete[] array;
    }


    T &get(const int x,const int y,const int z)
    {
        return get(glm::ivec3(x,y,z));
    }

    T &get(const glm::ivec3 &pos)
    {
      //if(x >= N || y >= N || z >= N) std::cout << "OUTOF ARRAY RANGE" << x << ":" << y << ":" << z << "\n";
      return array[pos.x*N*N+pos.z*N+pos.y];
    }

    void set(const int x, const int y ,const int z)
    {
        return set(glm::ivec3(x,y,z));
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
