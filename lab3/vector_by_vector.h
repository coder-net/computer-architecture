#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>


void single_thread(const std::vector<int>& v1, const std::vector<int>& v2) {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> result(v1.size(), 0);
  int ans = 0;
  for (int i = 0; i < v1.size(); i++) {
    ans += v1[i] * v2[i];
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << ans;
  std::cout << std::endl << "Time for single_thread computation: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
            << std::endl;
}

void parallel(const std::vector<int>& v1, const std::vector<int>& v2) {
  auto start = std::chrono::high_resolution_clock::now();
  int ans = 0;
#pragma omp parallel for reduction(+:ans) schedule(static)
  for (int i = 0; i < v1.size(); i++) {
    ans += v1[i] * v2[i];
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::cout << ans;
  std::cout << std::endl << "Time for parallel computation: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
            << std::endl;
}

void test_vector_by_vector(size_t n) {
  std::vector<int> a(n, 1), b(n, 2);
  single_thread(a, b);
  parallel(a, b);
}