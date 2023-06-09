#include "s21_memory/heap.hpp"

#include <cstdlib>
#include <new>

namespace s21::memory {

heap::heap(std::size_t size) : size_(size), data_(nullptr, std::free) {
  auto data = std::malloc(size);

  if (!data) {
    throw std::bad_alloc();
  }

  data_.reset(reinterpret_cast<raw_ptr>(data));
}

auto heap::data() const -> raw_ptr { return data_.get(); }

auto heap::size() const -> std::size_t { return size_; }

}  // namespace s21::memory
