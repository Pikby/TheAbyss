#pragma once
#include <unordered_map>
#include <map>
#include <mutex>
#include <glm/glm.hpp>
#include <list>
#include <iostream>

using namespace std;
//Take from https://stackoverflow.com/questions/27860685/how-to-make-a-multiple-read-single-write-lock-from-more-basic-synchronization-pr
//Plz no hate this looks great

#include <queue>
#include <mutex>
#include <condition_variable>
class RWLock {
private:
    std::mutex shared;
    std::condition_variable readerQ,writerQ;
    int active_readers, waiting_writers,active_writers;
public:
    RWLock() : shared(), readerQ(), writerQ(),
               active_readers(0), waiting_writers(0), active_writers(0)
    {}

    void readLock()
    {
      std::unique_lock<std::mutex> lk(shared);
      while( waiting_writers != 0 ) readerQ.wait(lk);
      ++active_readers;
      lk.unlock();
    }

    void readUnlock()
    {
      std::unique_lock<std::mutex> lk(shared);
      --active_readers;
      lk.unlock();
      writerQ.notify_one();
    }

    void writeLock() {
      std::unique_lock<std::mutex> lk(shared);
      ++waiting_writers;
      while( active_readers != 0 || active_writers != 0 )  writerQ.wait(lk);
      ++active_writers;
      lk.unlock();
    }

    void writeUnlock() {
      std::unique_lock<std::mutex> lk(shared);
      --waiting_writers;
      --active_writers;
      if(waiting_writers > 0) writerQ.notify_one();
      else readerQ.notify_all();
      lk.unlock();
    }

};


template <class T>
class Map3D
{
  struct Node
  {
    typename std::list<T>::iterator placeInList;
    T data;
  };
  private:
    RWLock lock;
    std::list<T> fullList;
    typename std::list<T>::iterator addToList(const T& data)
    {
      fullList.push_back(data);
      return --fullList.end();
    }
    map<int, map<int, map<int, Node>>> map3D;
    unsigned int size;
  public:
    Map3D()
    {
      size = 0;
    }
    void deleteAll()
    {
      lock.writeLock();
      map3D = map<int, map<int, map<int, Node>>>();
      fullList = std::list<T>();
      lock.writeUnlock();
    }
    unsigned int getSize(){return size;}
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
  lock.writeLock();
  auto itr = addToList(data);
  Node tmp = {itr,data};
  map3D[pos.x][pos.y][pos.z] = tmp;
  size++;
  lock.writeUnlock();
}

template <class T>
bool Map3D<T>::exists(const glm::ivec3 &pos)
{
  lock.readLock();
  if(map3D.count(pos.x) == 1)
  {
    auto &xMaps = map3D[pos.x];
    if(xMaps.count(pos.y) == 1)
    {
      if(xMaps[pos.y].count(pos.z) == 1)
      {
        lock.readUnlock();
        return true;
      }
    }
  }
  lock.readUnlock();
  return false;
}

template <class T>
T Map3D<T>::get(const glm::ivec3 &pos)
{
  lock.readLock();
  if(map3D.count(pos.x) == 1)
   {
     auto &xMaps = map3D[pos.x];
     if(xMaps.count(pos.y) == 1)
     {
       auto &yMaps = xMaps[pos.y];
       if(yMaps.count(pos.z) == 1)
       {
         T tmp = yMaps[pos.z].data;
         lock.readUnlock();
         return tmp;
       }
     }
   }
  lock.readUnlock();
  return NULL;
}

template <class T>
void Map3D<T>::del(const glm::ivec3 &pos)
{
  lock.writeLock();
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

  lock.writeUnlock();
}

template <class T>
std::shared_ptr<std::list<T>> Map3D<T>::findAll(const glm::ivec3 &min,const glm::ivec3 &max)
{
  lock.readLock();
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
  lock.readUnlock();
  return partList;
}

template <class T>
void Map3D<T>::print()
{
  lock.readLock();
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
  lock.readUnlock();
}
