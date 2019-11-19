#include <iostream>
#include <vector>
#include <array>
#include <mutex>
#include <condition_variable>


template <typename T>
class FixedMutexQueue {
 private:
  std::vector<T> array;
  std::mutex mutex_pop, mutex_push, mutex_common;
  std::condition_variable cond_var_push, cond_var_pop;
  size_t head, tail, size;

 public:
  FixedMutexQueue(size_t size) : head(0), tail(0), size(0) {
    array.resize(size);
  }
  void push(T val);
  bool pop(T& val);
};

template <typename T>
void FixedMutexQueue<T>::push(T val) {
  std::unique_lock<std::mutex> lock(mutex_push);
  cond_var_push.wait(lock, [&] { return size < array.size(); });
  array[tail % array.size()] = val;
  tail++;
  // shared resources, had trouble without this block
  {
    std::lock_guard<std::mutex> lck(mutex_common);
    size++;
  }
  cond_var_pop.notify_one();
}

template <typename T>
bool FixedMutexQueue<T>::pop(T &val) {
  std::unique_lock<std::mutex> lock(mutex_pop);
  cond_var_push.notify_all();
  // wait_for allow don't check how many elements was poped
  // outside of this function we can not use this function
  if (cond_var_pop.wait_for(lock, std::chrono::milliseconds(1), [&]{return size > 0;})) {
    val = array[head % array.size()];
    head++;
    {
      std::lock_guard<std::mutex> lck(mutex_common);
      size--;
    }
    cond_var_push.notify_one();
    return true;
  } else {
    return false;
  }
}


// one mutex FixedMutexQueue

template <typename T>
class FixedOneMutexQueue {
 private:
  std::vector<T> array;
  std::mutex mutex;
  std::condition_variable cond_var_push, cond_var_pop;
  size_t head, tail, size;

 public:
  FixedOneMutexQueue(size_t size) : head(0), tail(0), size(0) {
    array.resize(size);
  }
  void push(T val);
  bool pop(T& val);
};

template <typename T>
void FixedOneMutexQueue<T>::push(T val) {
  std::unique_lock<std::mutex> lock(mutex);
  cond_var_push.wait(lock, [&] { return size < array.size(); });
  array[tail % array.size()] = val;
  tail++;
  size++;
  cond_var_pop.notify_one();
}

template <typename T>
bool FixedOneMutexQueue<T>::pop(T &val) {
  std::unique_lock<std::mutex> lock(mutex);
  // wait_for allow don't check how many elements was poped
  // outside of this function we can not use this function
  cond_var_push.notify_all();
  if (cond_var_pop.wait_for(lock, std::chrono::milliseconds(1), [&]{return size > 0;})) {
    val = array[head % array.size()];
    head++;
    size--;
    cond_var_push.notify_one();
    return true;
  } else {
    return false;
  }
}
