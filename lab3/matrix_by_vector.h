#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <tuple>
#include <algorithm>


// почему reduction всё равно не работает
// не совсем понял, как его для вектора сделать
#pragma omp declare reduction(vec_int_plus : std::vector<int> : \
                              std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<int>())) \
                    initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))


std::pair<std::vector<std::vector<int>>, std::vector<int>> initialize(size_t n, size_t m) {
  std::vector<std::vector<int>> matrix(n);
  for (auto& v : matrix)
    v.resize(m, 1);
  std::vector<int> vector(m, 2);
  return std::make_pair(matrix, vector);
}

void single_thread_matrix(const std::vector<std::vector<int>>& matrix, const std::vector<int>& vector) {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> result(matrix.size(), 0);
  for (int i = 0; i < matrix.size(); i++) {
    for (int j = 0; j < vector.size(); j++) {
      result[i] += matrix[i][j] * vector[j];
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
//  for (const auto& i : result) {
//    std::cout << i << " ";
//  }
  std::cout << std::endl << "Time for single_thread computation: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
            << std::endl;
}

void parallel_matrix(const std::vector<std::vector<int>>& matrix, const std::vector<int>& vector) {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> result(matrix.size(), 0);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < matrix.size(); i++) {
    for (int j = 0; j < vector.size(); j++) {
      result[i] += matrix[i][j] * vector[j];
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
//  for (const auto& i : result) {
//    std::cout << i << " ";
//  }
  std::cout << std::endl << "Time for parallel computation: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
            << std::endl;
}

void test_matrix_by_vector(size_t n, size_t m) {
  std::vector<std::vector<int>> matrix;
  std::vector<int> vector;
  std::tie(matrix, vector) = initialize(n, m);
  single_thread_matrix(matrix, vector);
  parallel_matrix(matrix, vector);
}