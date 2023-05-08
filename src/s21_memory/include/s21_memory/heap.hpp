#pragma once

#include <cstddef>
#include <memory>

#include "s21_memory/block.hpp"

namespace s21::memory {

using local_ptr = std::size_t;

class heap {
 public:
  heap(std::size_t size);

  auto data() const -> raw_ptr;

  auto size() const -> std::size_t;

  auto resolve_pointer(local_ptr pointer) const -> raw_ptr;

 private:
  std::size_t size_;

  std::unique_ptr<raw_byte[]> data_;
};

}  // namespace s21::memory
