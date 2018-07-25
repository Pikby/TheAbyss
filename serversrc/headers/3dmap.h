#pragma once
#include <unordered_map>
#include <map>
#include <mutex>
#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <iostream>

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
    bool exists(int x, int y, int z);
    T get(int x, int y, int z);
    void add(int x, int y, int z, T data);
    void del(int x, int y, int z);
    void print();
    std::shared_ptr<std::list<T>> findAll(glm::ivec3 min, glm::ivec3 max);
    const std::list<T> &getFullList()
    {
      return fullList;
    }
};
template <class T>
void Map3D<T>::add(int x, int y,int z,T data)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  auto itr = addToList(data);
  Node tmp = {itr,data};
  map3D[x][y][z] = tmp;
  size++;
}

template <class T>
bool Map3D<T>::exists(int x, int y, int z)
{
  std::lock_guard<std::mutex> lock(mapMutex);
  if(map3D.count(x) == 1)
  {
    if(map3D[x].count(y) == 1)
    {
      if(map3D[x][y].count(z) == 1)
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
  if(map3D.count(x) == 1)
   {
     if(map3D[x].count(y) == 1)
     {
       if(map3D[x][y].count(z) == 1)
       {
         T tmp = map3D[x][y][z].data;
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
  fullList.erase(map3D[x][y][z].placeInList);
  map3D[x][y].erase(z);
  if(map3D[x][y].empty())
  {
    map3D[x].erase(y);
    if(map3D[x].empty())
    {
      map3D.erase(x);
    }
  }
  size--;
}

template <class T>
std::shared_ptr<std::list<T>> Map3D<T>::findAll(glm::ivec3 min,glm::ivec3 max)
{
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
