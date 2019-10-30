#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>
#include <vector>
#include "output.h"


template <typename T, size_t S>
bool mutex_increment(std::array<T, S>& array, size_t& index, std::mutex& mutex, int sleep_time) {
  size_t old_index;

  // in order to when index incremented, other threads can use it
  {
    // lock_guard lock mutex on create and unlock on delete
    std::lock_guard<std::mutex> lock(mutex);
    if (index >= array.size()) {
      return false;
    }
    old_index = index;
    index++;
  }

  // simulate long computations
  std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_time));
  array[old_index]++;
  return true;
}

template <typename T, size_t S>
bool atomic_increment(std::array<T, S>& array, std::atomic_size_t& index, int sleep_time) {
  size_t old_index = index++; // the same as index.fetch_add(1);
  if (old_index >= array.size())
    return false;

  std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_time));
  array[old_index]++;
  return true;
}

template <typename T, size_t S>
void atomic_execution(int num_thread, int sleep_time = 0) {
  std::array<T, S> array{{0}};
  std::atomic_size_t index(0);
  std::vector<std::thread> threads(num_thread);

  for (auto& thread: threads) {
    thread = std::thread([sleep_time](std::array<T, S>& array, std::atomic_size_t& index) {
      while (atomic_increment(array, index, sleep_time));
    }, std::ref(array), std::ref(index));
  }

  for (auto& thread: threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  // std::cout << array;
}

template <typename T, size_t S>
void mutex_execution(int num_thread, int sleep_time = 0) {
  std::array<T, S> array{{0}};
  std::mutex mutex;
  size_t index = 0;
  std::vector<std::thread> threads(num_thread);

  for (auto& thread: threads) {
    thread = std::thread([sleep_time](std::array<T, S>& array, size_t& index, std::mutex& mutex) {
      while (mutex_increment(array, index, mutex, sleep_time));
    }, std::ref(array), std::ref(index), std::ref(mutex));
  }

  for (auto& thread: threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
   // std::cout << array;
}


// why std::array: https://stackoverflow.com/questions/46717870/c-concurrent-writes-to-array-not-stdvector-of-bools
template <typename T, size_t S>
void parallel_array_traversal() {
  std::vector<int> num_threads = {4, 8, 16, 32};
  std::vector<int> sleep_times = {0, 10};

  // for simulated computation time
  for (int sleep_time : sleep_times) {
    // choose num_threads
    for (int num_thread: num_threads) {
      // for mutex
      auto start = std::chrono::high_resolution_clock::now();
      mutex_execution<T, S>(num_thread, sleep_time);
      auto end = std::chrono::high_resolution_clock::now();
      double mutex_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9; // std::chrono::sec round to integer
      // for atomic
      start = std::chrono::high_resolution_clock::now();
      atomic_execution<T, S>(num_thread, sleep_time);
      end = std::chrono::high_resolution_clock::now();
      double atomic_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

      std::cout << "Execution time for thread_nums = " << num_thread << ", sleep_time = " << sleep_time << std::endl
                << "mutex: " << mutex_time << " sec" << std::endl
                << "atomic: " << atomic_time << " sec" << std::endl
                << std::endl;
    }
  }
}