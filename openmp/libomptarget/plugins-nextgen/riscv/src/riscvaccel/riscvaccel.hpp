#ifndef RISCVACCEL_HPP
#define RISCVACCEL_HPP

#include <cstdint>

// Launches the execution of a Kernel.
void riscvaccel_launch( uint64_t entry_point, uint64_t stack_pointer, int argc, uint64_t* argv );

void riscvaccel_wait_until_cmplt();
// Allocates pinned memory on the host like cudaMallocHost
// and returns the physical memory address.
uint64_t riscvaccel_malloc_host(size_t size);
// Frees pinned memory like cudaFreeHost.
void riscvaccel_free_host(uint64_t physical_addr );
// Transfer data from and to pinned memory , like cudaMemcpy.
void riscvaccel_memcpy_h2d(uint64_t physical_addr ,
const uint64_t host_vaddr , size_t size);
void riscvaccel_memcpy_d2h(uint64_t host_vaddr ,
const uint64_t physical_addr , size_t size);

#endif // !RISCVACCEL_HPP