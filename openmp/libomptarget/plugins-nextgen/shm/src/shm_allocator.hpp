#ifndef SHM_ALLOCATOR_HPP
#define SHM_ALLOCATOR_HPP

#include "span.hpp"
#include "shm_err.hpp"

// #define ALIGN_UP(base, size)	ALIGN_DOWN ((base) + (size) - 1, (size))

class MyAllocator {
  Span<char> memory;
  uintptr_t free_ptr;

public:
  MyAllocator() : memory(Span<char>::empty()), free_ptr(0) { }
  MyAllocator(Span<char> memory) : memory(memory), free_ptr((uintptr_t)memory.begin()) { }

  void* allocate(size_t size) {
    SHM_TRACE_FN;

    intptr_t free_space = (uintptr_t)memory.end() - free_ptr;

    SHM_DP("Allocating %lu bytes in region [%lx - %lx] (%lu free)", size, free_ptr, (uintptr_t)memory.end(), free_space);

    if (size > free_space) {
      SHM_DP("Allocation failed (no space available)\n");
      return nullptr;
    }

    // Misalignment-fest
    void* ans = (void*)free_ptr;
    free_ptr += size;
    return ans;
  }

  void deallocate(void* ptr) {
    // noop, who cares
  }
};


#endif // !SHM_ALLOCATOR_HPP