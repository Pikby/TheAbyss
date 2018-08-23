#pragma once
#include <unordered_map>
typedef const int cint;
template <class T, int N>
class BoundedMap3D
{
  private:
    std::unordered_map<T> map;
    int getIndex(cint x,cint y, cint z)
    {
      return x*N*N+y*N+z;
    }
  public:
    int getSize()
    {
      return map.size();
    }
    bool empty()
    {
      return map.empty();
    }
    bool exists(cint x,cint y,cint z)
    {
      return (map.count(getIndex(x,y,z)) == 1);
    }
    void add(cint x,cint y, cint z, const T& obj)
    {
      map[getIndex(x,y,z)] = obj;
    }
    void del(cint x,cint y,cint z)
    {
      map.erase(getIndex(x,y,z));
    }
    T& get(cint x, cint y, cint z)
    {
      return map[getIndex(x,y,z)];
    }
}
