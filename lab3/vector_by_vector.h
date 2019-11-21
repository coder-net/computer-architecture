#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>


double single_thread(const std::vector<int>& v1, const std::vector<int>& v2) {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> result(v1.size(), 0);
  int ans = 0;
  for (int i = 0; i < v1.size(); i++) {
    ans += v1[i] * v2[i];
  }
  auto end = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
  std::cout << ans;
  std::cout << std::endl << "Time for single_thread computation: "
            << duration << " sec"
            << std::endl;
  return duration;
}

double parallel(const std::vector<int>& v1, const std::vector<int>& v2) {
  auto start = std::chrono::high_resolution_clock::now();
  int ans = 0;
#pragma omp parallel for reduction(+:ans) schedule(static)
  for (int i = 0; i < v1.size(); i++) {
    ans += v1[i] * v2[i];
  }

  auto end = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
  std::cout << ans;
  std::cout << std::endl << "Time for parallel computation: "
            << duration << " sec"
            << std::endl;
  return duration;
}

void test_vector_by_vector(size_t n) {
  std::vector<int> a(n, 1), b(n, 2);
  double t1 = single_thread(a, b);
  double t2 = parallel(a, b);
  std::cout << "Boost: " << t1 / t2;
}