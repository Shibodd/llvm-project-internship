#include <cstdlib>
#include <cstring>
#include <cassert>
#include "Debug.h"
#include "riscvaccel.hpp"

extern "C" {
  #include <unistd.h>
  #include <sys/mman.h>
}

void* map_exec(size_t size) {
  void* addr = mmap(nullptr, size * 10, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(addr != nullptr);
  return addr;
}

// Launches the execution of a Kernel.
void riscvaccel_launch( uint64_t entry_point, uint64_t stack_pointer, int argc, uint64_t* argv ) {
  DP("[%s] Launching kernel @%lx with stack @%lx and %d arguments\n", __func__, entry_point, stack_pointer, argc);
  ((void(*)(uint64_t))entry_point)(*argv);
}
// Reads whether the accelerator has
// completed the kernel execution
void riscvaccel_wait_until_cmplt() {
  DP("[%s] Waiting...\n", __func__);
  DP("[%s] Accelerator has finished executing the last kernel!\n", __func__);
}
// Allocates pinned memory on the host like cudaMallocHost
// and returns the physical memory address.
uint64_t riscvaccel_malloc_host(size_t size) {
  DP("[%s] Attempting to allocate %zu bytes...\n", __func__, size);
  uint64_t p = (uint64_t)map_exec(size);
  DP("[%s] Allocated %zu bytes @%lx\n", __func__, size, p);
  return p;
}
// Frees pinned memory like cudaFreeHost.
void riscvaccel_free_host(uint64_t physical_addr) {
  DP("[%s] Freeing %lx.\n", __func__, physical_addr);
}
// Transfer data from and to pinned memory , like cudaMemcpy.
void riscvaccel_memcpy_h2d(uint64_t physical_addr ,
const uint64_t host_vaddr , size_t size) {
  DP("[%s] Submitting %zu bytes (%lx -> %lx).\n", __func__, size, host_vaddr, physical_addr);
  memcpy((void*)physical_addr, (void*)host_vaddr, size);
}
void riscvaccel_memcpy_d2h(uint64_t host_vaddr ,
const uint64_t physical_addr , size_t size) {
  DP("[%s] Retrieving %zu bytes (%lx -> %lx).\n", __func__, size, physical_addr, host_vaddr);
  memcpy((void*)host_vaddr, (void*)physical_addr, size);
}
