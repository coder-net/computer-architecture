#include <ostream>
#include <vector>
#include <array>


template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v) {
  for (const auto& el: v) {
    stream << el << " ";
  }
  stream << std::endl;
  return stream;
}

template <typename T, size_t S>
std::ostream& operator<<(std::ostream& stream, const std::array<T, S>& array) {
  for (const auto& val: array) {
    stream << val << " ";
  }
  stream << std::endl;
  return stream;
}