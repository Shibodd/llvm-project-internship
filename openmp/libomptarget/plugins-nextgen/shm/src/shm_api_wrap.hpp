#ifndef SHM_API_WRAP_HPP
#define SHM_API_WRAP_HPP

// Linux C API wrap

#include "llvm/Support/Error.h"

llvm::Expected<int> wrap_shm_open(const char* name, bool create);
llvm::Expected<void*> wrap_shm_mmap(int file, size_t size);
llvm::Error wrap_ftruncate(int file, size_t size);
llvm::Error wrap_munmap(void* address, size_t size);
llvm::Error wrap_close(int file);
llvm::Expected<size_t> wrap_fstat_size(int file);
llvm::Error wrap_shm_unlink(const char* name);

#endif // !SHM_API_WRAP_HPP