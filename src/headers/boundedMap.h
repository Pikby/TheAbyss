#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
template <class T, int N>
class BoundedMap3D
{
  private:
    std::unordered_map<int,T> map;
    int getIndex(const glm::ivec3 &pos)
    {
      return pos.x*N*N+pos.y*N+pos.z;
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
    bool exists(const glm::ivec3 &pos)
    {
      return (map.count(getIndex(pos)) == 1);
    }
    void add(const glm::ivec3 &pos, const T& obj)
    {
      map[getIndex(pos)] = obj;
    }
    void del(const glm::ivec3 &pos)
    {
      map.erase(getIndex(pos));
    }
    T& get(const glm::ivec3 &pos)
    {
      return map[getIndex(pos)];
    }
};
