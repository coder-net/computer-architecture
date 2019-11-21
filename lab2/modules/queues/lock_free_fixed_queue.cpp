#include <thread>
#include <vector>
#include <atomic>

template <typename T>
struct SNode {
  uint8_t is_valid = 0;
  T value;

  SNode() : value(T()), is_valid(0) {}
  SNode(T val) : value(val), is_valid(0) {}
};

template <typename T>
class LockFreeQueue {
 private:
  std::vector<std::atomic<SNode<T>>> arr;
  size_t capacity;
  alignas(128) std::atomic_size_t head;
  alignas(128) std::atomic_size_t tail;

 public:
  LockFreeQueue(size_t s): head(0), tail(0), arr(s + 1) {
    capacity = s + 1;
    std::cout << arr[0].is_lock_free() << std::endl;
    std::cout << "Size: " << sizeof(arr[0]) * arr.size() << std::endl;
  }
  bool pop(T& v);
  void push(T v);
};

template <typename T>
void LockFreeQueue<T>::push(T v) {
  SNode<T> new_node(v);
  new_node.is_valid = 1;
  while (true) {
    size_t tail_pos = tail;
    // Проверка на переполнение
    if ((tail_pos + 1) % capacity == head)
      continue;
    auto tail_value = arr[tail_pos];
    // проверка на правильное значение tail_value
    if (tail_pos != tail)
      continue;
    // пробуем установить новое значение, двигаем хвост
    if (tail_value.is_valid == 0 and arr[tail_pos].compare_exchange_strong(tail_value, new_node)) {
      tail.compare_exchange_strong(tail_pos, (tail_pos + 1) % capacity);
      return;
    } else {
      tail.compare_exchange_strong(tail_pos, (tail_pos + 1) % capacity);
    }
  }
}

template <typename T>
bool LockFreeQueue<T>::pop(T &v) {
  while(true) {
    size_t head_pos = head;
    // очередь пуста
    if (head_pos == tail) {
      return false;
    }
    SNode<T> head_value = arr[head_pos];
    // проверка на правильное значение head_value
    if (head_pos != head)
      continue;
    SNode<T> empty_node = SNode<T>();
    // пытаемся заменить на пустой узел с is_valid = 0, двигаем голову
    if (head_value.is_valid != 0 and (arr[head_pos]).compare_exchange_strong(head_value, empty_node)) {
      head.compare_exchange_strong(head_pos, (head_pos+1) % capacity);
      v = head_value.value;
      return true;
    } else {
      head.compare_exchange_strong(head_pos, (head_pos+1) % capacity);
    }
  }
}