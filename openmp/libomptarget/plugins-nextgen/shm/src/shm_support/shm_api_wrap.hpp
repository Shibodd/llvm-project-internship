#ifndef SHM_API_WRAP_HPP
#define SHM_API_WRAP_HPP

// Linux C API wrap

extern "C" {
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
}

#include "shm_support.hpp"
#include "shm_errs.hpp"

static Err wrap_shm_open(const std::string& name, int& file_out) {
  int file = shm_open(name.c_str(), O_RDWR, S_IREAD | S_IWRITE);
  if (file < 0)
    return make_err_withno("Could not open the shared memory object.");

  file_out = file;
  return make_success();
}

static Err wrap_shm_mmap(int file, size_t size, void*& address_out) {
  void* address = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
  if (address == MAP_FAILED)
    return make_err_withno("Could not mmap the shared memory object.");

  address_out = address;
  return make_success();
}

static Err wrap_ftruncate(int file, size_t size) {
  if (ftruncate(file, size) != 0)
    return make_err_msg("Could not resize the shared memory object.");
  return make_success();
}

static Err wrap_munmap(void* address, size_t size) {
  if (munmap(address, size) != 0)
    return make_err_msg("Could not munmap the shared memory object.");
  return make_success();
}

static Err wrap_close(int file) {
  if (close(file) != 0)
    return Err("Could not close the shared memory object.");
  return make_success();
}

#endif // !SHM_API_WRAP_HPP