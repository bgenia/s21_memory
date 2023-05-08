#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "s21_memory/block.hpp"

namespace s21::memory {

class heap {
 public:
  heap(std::size_t size);

  auto data() const -> raw_ptr;

  auto size() const -> std::size_t;

 private:
  std::size_t size_;

  std::unique_ptr<raw_byte[], std::function<void(raw_ptr)>> data_;
};

}  // namespace s21::memory
