#include <iostream>
#include <queue>
#include <mutex>
#include <thread>


template <typename T>
class DynamicQueue {
 private:
  std::queue<T> queue;
  std::mutex mutex;

 public:
  void push(T val);
  bool pop(T& val);
};

template <typename T>
void DynamicQueue<T>::push(T val) {
  std::lock_guard<std::mutex> lock(mutex);

  queue.push(val);
}

template <typename T>
bool DynamicQueue<T>::pop(T& val) {
  mutex.lock();
  if (queue.empty()) {
    mutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mutex.lock();
    if (queue.empty()) {
      mutex.unlock();
      return false;
    }
  }

  val = queue.front();
  queue.pop();
  mutex.unlock();
  return true;
}




