#pragma once
#include <unordered_map>
#include <map>
#include <mutex>
#include <glm/glm.hpp>
#include <list>

using namespace std;

template <class T>
class Map3D
{
  struct Node
  {
    typename std::list<T>::iterator placeInList;
    T data;
  };
  private:
    std::mutex mapMutex;
    std::list<T> fullList;
    typename std::list<T>::iterator addToList(const T& data)
    {
      fullList.push_back(data);
      return --fullList.end();
    }
    map<int, map<int, map<int, Node>>> map3D;
  public:
    Map3D()
    {
      size = 0;
    }
    unsigned int size;
    bool exists(const glm::ivec3 &pos);
    T get(const glm::ivec3 &pos);
    void add(const glm::ivec3 &pos, T data);
    void del(const glm::ivec3 &pos);
    void print();
    std::shared_ptr<std::list<T>> findAll(const glm::ivec3 &min, const glm::ivec3 &max);
    const std::list<T> &getFullList()
    {
      return fullList;
    }
};
template <class T>
void Map3D<T>::add(const glm::ivec3 &pos,T data)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  auto itr = addToList(data);
  Node tmp = {itr,data};
  map3D[pos.x][pos.y][pos.z] = tmp;
  size++;
}

template <class T>
bool Map3D<T>::exists(const glm::ivec3 &pos)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  if(map3D.count(pos.x) == 1)
  {
    auto &xMaps = map3D[pos.x];
    if(xMaps.count(pos.y) == 1)
    {
      if(xMaps[pos.y].count(pos.z) == 1)
      {
        return true;
      }
    }
  }
  return false;
}

template <class T>
T Map3D<T>::get(const glm::ivec3 &pos)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  if(map3D.count(pos.x) == 1)
   {
     auto &xMaps = map3D[pos.x];
     if(xMaps.count(pos.y) == 1)
     {
       auto &yMaps = xMaps[pos.y];
       if(yMaps.count(pos.z) == 1)
       {
         T tmp = yMaps[pos.z].data;
         return tmp;
       }
     }
   }
  return NULL;
}

template <class T>
void Map3D<T>::del(const glm::ivec3 &pos)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  auto &xMaps = map3D[pos.x];
  auto &yMaps = xMaps[pos.y];
  auto &node  = yMaps[pos.z];

  fullList.erase(node.placeInList);
  yMaps.erase(pos.z);
  if(yMaps.empty())
  {
    xMaps.erase(pos.y);
    if(xMaps.empty())
    {
      map3D.erase(pos.x);
    }
  }
  size--;
}

template <class T>
std::shared_ptr<std::list<T>> Map3D<T>::findAll(const glm::ivec3 &min,const glm::ivec3 &max)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  std::shared_ptr<std::list<T>> partList(new std::list<T>);
  for(auto itx = map3D.lower_bound(min.x); itx->first <= max.x && itx !=map3D.end();++itx)
  {
    for(auto ity = itx->second.lower_bound(min.y);ity->first <= max.y && ity != itx->second.end();++ity)
    {
      for(auto itz = ity->second.lower_bound(min.z); itz->first <= max.z && itz != ity->second.end();++itz)
      {
        partList->push_back(itz->second.data);
      }
    }
  }
  return partList;
}

template <class T>
void Map3D<T>::print()
{
  for(auto itx = map3D.begin();itx != map3D.end();++itx)
  {
    std::cout << itx->first << ":\n";
    for(auto ity = itx->second.begin();ity != itx->second.end();++ity)
    {
      std::cout << "  " << ity->first << ":\n";
        for(auto itz = ity->second.begin();itz != ity->second.end();++itz)
        {
          std::cout << "    " << itz->first << "\n";
        }
    }
  }
}
