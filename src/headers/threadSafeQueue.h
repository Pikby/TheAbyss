#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class TSafeQueue
{
private:
  std::deque<T> queue;
  std::mutex queueMutex;
  std::condition_variable cv;

public:
  bool empty()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.empty();
  }
  int size()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.size();
  }
  T front()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.front();
  }
  T back()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.back();
  }
  void push(const T &item)
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push_back(item);
    cv.notify_one();
  }
  void push_front(const T &item)
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push_front(item);
    cv.notify_one();
  }
  void notify_one()
  {
    cv.notify_one();
  }
  void pop()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.pop_front();
  }
  T getAndPop()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    T temp = queue.front();
    queue.pop_front();
    return temp;
  }
  void waitForData()
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    cv.wait(lock);
  }

};
