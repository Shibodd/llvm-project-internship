// Linux C API wrap

extern "C" {
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
}

#include <string>
#include "shm_err.hpp"

llvm::Expected<int> wrap_shm_open(const char* name, bool create) {
  int flags = O_RDWR;
  if (create) {
    flags |= O_CREAT | O_TRUNC;
  }

  int file = shm_open(name, flags, S_IREAD | S_IWRITE);
  ASSERT_OR_PERR(file >= 0, "Could not open the shared memory object.");
  return file;
}

llvm::Expected<void*> wrap_shm_mmap(int file, size_t size) {
  void* address = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
  ASSERT_OR_PERR(address != MAP_FAILED, "Could not mmap the shared memory object.");
  return address;
}

llvm::Error wrap_ftruncate(int file, size_t size) {
  ASSERT_OR_PERR(ftruncate(file, size) == 0, "Could not resize the shared memory object.");
  return make_success();
}

llvm::Error wrap_munmap(void* address, size_t size) {
  ASSERT_OR_PERR(munmap(address, size) == 0, "Could not munmap the shared memory object.");
  return make_success();
}

llvm::Error wrap_close(int file) {
  ASSERT_OR_PERR(close(file) == 0, "Could not close the shared memory object.");
  return make_success();
}

llvm::Expected<size_t> wrap_fstat_size(int file) {
  struct stat buf;
  ASSERT_OR_PERR(fstat(file, &buf) == 0, "Could not get shared memory object length.");
  return buf.st_size;
}

llvm::Error wrap_shm_unlink(const char* name) {
  ASSERT_OR_PERR(shm_unlink(name) == 0, "Couldn't unlink shared memory object.");
  return make_success();
}