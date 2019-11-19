#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "queues/dynamic_queue.h"


const std::vector<int> producer_num = {1, 2, 4};
const std::vector<int> consumer_num = {1, 2, 4};
const std::vector<size_t> queue_size = {1, 4, 16};
const int TASK_NUM = 1 << 15; // 4 * 1024 * 1024;


void test_dynamic_queue() {
  DynamicQueue<uint8_t> dynamicQueue;
  int task_num = TASK_NUM;
  for (int prod_num : producer_num) {
    for (int cons_num : consumer_num) {
      std::atomic_int sum(0);
      std::cout << "Result for producer_num: " << prod_num
                << ", consumer_num: " << cons_num << std::endl;\
      std::vector<std::thread> consumers(cons_num);
      std::vector<std::thread> producers(prod_num);
      auto start = std::chrono::high_resolution_clock::now();
      for (auto& thread : consumers) {
        thread = std::thread(
                [&sum, &dynamicQueue, task_num, prod_num]() {
                  uint8_t val;
                  while (sum.load() < prod_num * task_num) {
                    bool res;
                    res = dynamicQueue.pop(val);
                    sum += res;
                  }
                }
                );
      }
      for (auto& thread : producers) {
        thread = std::thread(
                [&task_num, &dynamicQueue]() {
                  for (int i = 0; i < task_num; ++i)
                    dynamicQueue.push(1);
                }
                );
      }

      for (auto& thread : consumers) {
          if (thread.joinable())
            thread.join();
      }
      for (auto& thread : producers) {
        if (thread.joinable())
          thread.join();
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout <<" " << sum.load() << ", time: "
                << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec";
      std::cout << std::endl;
    }
  }
}


template <typename T>
void test_fixed_queue() {
  int task_num = TASK_NUM;
  for (auto size : queue_size) {
    for (int prod_num : producer_num) {
      for (int cons_num : consumer_num) {

        T queue(size);
        std::atomic_int sum(0);
        std::cout << "Result for producer_num: " << prod_num
                  << ", consumer_num: " << cons_num
                  << ", size: " << size << std::endl;

        std::vector<std::thread> consumers(cons_num);
        std::vector<std::thread> producers(prod_num);
        auto start = std::chrono::high_resolution_clock::now();
        for (auto &thread : consumers) {
          thread = std::thread(
                  [&sum, &queue, task_num, prod_num]() {
                    uint8_t val;
                    while (sum.load() < prod_num * task_num) {
                      sum += queue.pop(val);
                      // std::cout << sum.load() << std::endl;
                    }
                  }
          );
        }
        for (auto &thread : producers) {
          thread = std::thread(
                  [&task_num, &queue]() {
                    for (int i = 0; i < task_num; i++) {
                      queue.push(1);
                      // std::cout << i << std::endl;
                    }

                  }
          );
        }
        for (auto &thread : producers) {
          if (thread.joinable())
            thread.join();
        }

        for (auto &thread : consumers) {
          if (thread.joinable())
            thread.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << " " << sum.load() << ", time: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec";
        std::cout << std::endl;
      }
    }
  }
}
