#include <iostream>
#include "modules/parallel_traversal.h"
#include "modules/queues_test.h"

const size_t NUM_TASKS = 1024 * 1024;


int main() {
  int x = 2;
  switch (x) {
    case 0:
      parallel_array_traversal<int, NUM_TASKS>();
      break;
    case 1:
      test_dynamic_queue();
      break;
    case 2:
      test_fixed_mutex_queue();
      break;
    case 3:
      test_fixed_atomic_queue();
      break;
  }
  return 0;
}