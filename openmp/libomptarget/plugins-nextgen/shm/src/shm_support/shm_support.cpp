#include "shm_support.hpp"
#include "../shm_helpers/shm_errs.hpp"
#include "shm_api_wrap.hpp"

#include <string>

llvm::Error ShmObject::create(size_t size_in) {
  if (opened)
    return make_err_msg("The shared memory object was already open!");

  if (auto err = wrap_shm_open(name, file, true))
    return err;

  if (auto err = wrap_ftruncate(file, size_in)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  void* addr;
  if (auto err = wrap_shm_mmap(file, size_in, addr)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  mapped_memory = { (char*)addr, size_in };
  opened = true;
  return make_success();
}

llvm::Error ShmObject::open() {
  if (opened)
    return make_err_msg("The shared memory object was already open!");

  if (auto err = wrap_shm_open(name, file, false))
    return err;

  size_t size;
  if (auto err = wrap_fstat_size(file, size)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  void* addr;
  if (auto err = wrap_shm_mmap(file, size, addr)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  mapped_memory = { (char*)addr, size };
  opened = true;
  return make_success();
}

void ShmObject::close() {
  static_cast<void>(wrap_munmap(mapped_memory.begin(), mapped_memory.len()));
  static_cast<void>(wrap_close(file));

  file = -1;
  mapped_memory = Span<char>::empty();
  opened = false;
}

void ShmObject::unlink() {
  close();
  static_cast<void>(wrap_shm_unlink(name));
}