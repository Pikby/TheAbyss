#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class TSafeQueue
{
private:
  std::queue<T> queue;
  std::mutex queueMutex;
public:
  std::condition_variable cv;
  void emptyQueue()
  {

    std::lock_guard<std::mutex> lock(queueMutex);
    std::queue<T> empty;
    std::swap(empty,queue);
    cv.notify_one();

  }
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
    queue.push(item);
    cv.notify_one();
  }
  void pop()
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.pop();
  }
  void waitForData()
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    cv.wait(lock);
  }

};
