#include <iostream>
#include "vector_by_vector.h"
#include "matrix_by_vector.h"



int main() {
  size_t n = 100000000;
  std::cout << "Vector by vector" << std::endl;
  test_vector_by_vector(n);
  size_t a = 1000;
  size_t b = 200000;
  std::cout << std::endl << "Matrix by vector" << std::endl;
  test_matrix_by_vector(a, b);
  return 0;
}