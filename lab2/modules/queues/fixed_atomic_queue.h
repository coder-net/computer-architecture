#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <condition_variable>


template <typename T>
class FixedAtomicQueue {
 private:
  std::vector<T> array;
  std::atomic_size_t head, tail, size;
  std::mutex mutex_pop, mutex_push;
  std::condition_variable cond_var_pop, cond_var_push;
  std::atomic_bool free;

 public:
  FixedAtomicQueue(size_t size) : free(true), head(0), tail(0), size(0) {
    array.resize(size);
  }

  bool pop(T& val);
  void push(T val);
};

template<typename T>
void FixedAtomicQueue<T>::push(T val) {
  while (true) {
    bool required = true;
    bool is_free = free.compare_exchange_strong(required, false);
    while (!is_free) {
      std::unique_lock<std::mutex>lock(mutex_push);
      cond_var_push.wait(lock, [&]() {
        bool required = true;
        return free.compare_exchange_strong(required, false);
      });
      is_free = true;
    }
    if (size < array.size()){
      break;
    } else {
      free = true;
      cond_var_pop.notify_one();
    }
  }
  array[tail++ % array.size()] = val;
  size++;
  free = true;
  cond_var_pop.notify_one();
}

template <typename T>
bool FixedAtomicQueue<T>::pop(T &val) {
  bool result = false;
  bool required = true;
  bool is_free = free.compare_exchange_strong(required, false);
  if (!is_free) {
    std::unique_lock < std::mutex > lock(mutex_pop);
    is_free = cond_var_pop.wait_for(lock, std::chrono::milliseconds(1), [&]() {
              bool required = true;
              return free.compare_exchange_strong(required, false);
            });
  }
  if (is_free) {
    if (size == 0) {
      result = false;
    } else {
      val = array[head++ % array.size()];
      size--;
      result = true;
    }
    free = true;
    cond_var_push.notify_one();
  }
  return result;
}

