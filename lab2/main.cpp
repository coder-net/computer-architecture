#include <iostream>
#include "modules/parallel_traversal.h"
#include "modules/queues_test.h"
#include "modules/queues/fixed_atomic_queue.h"
#include "modules/queues/fixed_mutex_queue.h"

const size_t NUM_TASKS = 1024 * 1024;


int main() {
  int x;
  std::cin >> x;
  switch (x) {
    case 0:
      parallel_array_traversal<int, NUM_TASKS>();
      break;
    case 1:
      std::cout << "Dynamic queue \"mutex\"" << std::endl << std::endl;
      test_dynamic_queue();
      break;
    case 2:
      std::cout << "Fixed mutex queue with 3 mutex" << std::endl << std::endl;
      test_fixed_queue< FixedMutexQueue<uint8_t> >();
      break;
    case 3:
      std::cout << "Fixed mutex queue with 1 mutex" << std::endl << std::endl;
      test_fixed_queue< FixedOneMutexQueue<uint8_t> >();
      break;
    case 4:
      std::cout << "Fixed atomic queue with \"mutex\"" << std::endl << std::endl;
      test_fixed_queue< FixedAtomicQueue<uint8_t> >();
      break;
    case 5:
      std::cout << "Fixed atomic queue without \"mutex\"" << std::endl << std::endl;
      test_fixed_queue< NewFixedAtomicQueue<uint8_t> >();
      break;
  }
  return 0;
}