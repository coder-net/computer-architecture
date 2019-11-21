#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <condition_variable>


template <typename T>
class LockFreeFixedAtomicQueue {
 private:
  std::vector<T> array;
  std::atomic_size_t head, tail, size;
  std::mutex mutex_pop, mutex_push;
  std::condition_variable cond_var_pop, cond_var_push;
  std::atomic_bool free;

 public:
  LockFreeFixedAtomicQueue(size_t size) : free(true), head(0), tail(0), size(0) {
    array.resize(size);
  }

  bool pop(T& val);
  void push(T val);
};

template<typename T>
void LockFreeFixedAtomicQueue<T>::push(T val) {
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
bool LockFreeFixedAtomicQueue<T>::pop(T &val) {
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


//template <typename T>
//class NewFixedAtomicQueue {
// private:
//  std::vector<T> array;
//  std::atomic_size_t head, tail;
//  std::atomic_bool is_free;
//
// public:
//  NewFixedAtomicQueue(size_t size) : is_free(false), head(0), tail(0) {
//    array.resize(size);
//  }
//
//  bool pop(T& val);
//  void push(T val);
//};
//
//template<typename T>
//void NewFixedAtomicQueue<T>::push(T val) {
//  while (true) {
//    size_t tail_pos = tail.load();
//    if ((tail_pos + 1) % array.size() == head.load() % array.size()) {
//      continue;
//    }
//    T old_value = array[tail_pos];
//    bool required = false;
//    if (is_free.compare_exchange_strong(required, true)) {
//      if (tail_pos == tail && array[tail_pos] == old_value) {
//        array[tail_pos] = val;
//        tail++;
//        is_free = false;
//        return;
//      }
//      is_free = false;
//    }
//  }
//}
//
//template <typename T>
//bool NewFixedAtomicQueue<T>::pop(T &val) {
//  while (true) {
//    size_t head_pos = head.load();
//    if (head % array.size() == tail.load() % array.size()) {
//      return false;
//    }
//    T old_value = array[head_pos];
//    if (head.compare_exchange_strong(head_pos, head_pos + 1)) {
//      val = old_value;
//      return true;
//    }
//  }
//}


template <typename T>
class NewFixedAtomicQueue {
 private:
  std::vector<T> arr;
  size_t capacity;
  alignas(128) std::atomic_size_t head;
  alignas(128) std::atomic_size_t tail;
  std::atomic_bool is_writing;

 public:
  NewFixedAtomicQueue(size_t s): head(0), tail(0), is_writing(false) {
    capacity = s + 1;
    arr.resize(capacity);
  }
  bool pop(T &v);
  void push(T v);
};

template <typename T>
void NewFixedAtomicQueue<T>::push(T v) {
  for (;;) {
    size_t tail_pos = tail;
    // Проверка на переполнение
    if ((tail_pos + 1) % capacity == head) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(10));
      continue;
    }
    T tail_value = arr[tail_pos];
    // проверка на правильное значение tail_value
    if (tail_pos != tail) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(10));
      continue;
    }
    // DCAS
    bool smb_write = false;
    if (is_writing.compare_exchange_strong(smb_write, true)) {
      if (arr[tail_pos] == tail_value && tail == tail_pos) {
        arr[tail_pos] = v;
        tail = (tail_pos + 1) % capacity;
        is_writing = false;
        return;
      }
      is_writing = false;
    }

  }
}

template <typename T>
bool NewFixedAtomicQueue<T>::pop(T &v) {
  for(;;) {
    size_t head_pos = head;
    // очередь пуста, подождем и выйдем
    if (head_pos == tail) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(10));
      head_pos = head;
      if (head_pos == tail)
        return false;
    }
    T head_value = arr[head_pos];
    if (head.compare_exchange_strong(head_pos, (head_pos+1) % capacity)) {
      v = head_value;
      return true;
    }
  }
}