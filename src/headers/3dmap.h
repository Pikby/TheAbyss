
using namespace std;
template <class T>
class Map3D
{
  private:
    std::mutex mapMutex;
    unordered_map<int, unordered_map<int, unordered_map<int, T>>> map;
  public:
    bool exists(int x, int y, int z);
    T get(int x, int y, int z);
    void add(int x, int y, int z, T data);
    void del(int x, int y, int z);
};
template <class T>
void Map3D<T>::add(int x, int y,int z,T data)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  map[x][y][z] = data;
}

template <class T>
bool Map3D<T>::exists(int x, int y, int z)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  if(map.count(x) == 1)
  {
    if(map[x].count(y) == 1)
    {
      if(map[x][y].count(z) == 1)
      {
        return true;
      }
    }
  }
  return false;
}

template <class T>
T Map3D<T>::get(int x, int y, int z)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  if(map.count(x) == 1)
   {
     if(map[x].count(y) == 1)
     {
       if(map[x][y].count(z) == 1)
       {
         T tmp = map[x][y][z];
         return tmp;
       }
     }
   }
  return NULL;
}

template <class T>
void Map3D<T>::del(int x, int y, int z)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  map[x][y].erase(z);
  if(map[x][y].empty())
  {
    map[x].erase(y);
    if(map[x].empty())
    {
      map.erase(x);
    }
  }
}
