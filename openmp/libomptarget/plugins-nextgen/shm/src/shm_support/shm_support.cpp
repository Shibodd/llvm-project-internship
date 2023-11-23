#include "shm_support.hpp"
#include "shm_errs.hpp"
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

  if (auto err = wrap_shm_mmap(file, size_in, address)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  opened = true;
  size = size_in;
  return make_success();
}

llvm::Error ShmObject::open() {
  if (opened)
    return make_err_msg("The shared memory object was already open!");

  if (auto err = wrap_shm_open(name, file, false))
    return err;

  if (auto err = wrap_fstat_size(file, size)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  if (auto err = wrap_shm_mmap(file, size, address)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  opened = true;
  return make_success();
}

/*
ShmError ShmObject::resize(size_t new_size) {
  if (!opened)
    return make_err_msg("This shared memory object is not open!");
    
  ShmError err;
  if (!(err = wrap_ftruncate(file, new_size)))
    return err;

  size = new_size;
  return make_success();
}
*/

void ShmObject::close() {
  static_cast<void>(wrap_munmap(address, size));
  static_cast<void>(wrap_close(file));

  file = -1;
  address = nullptr;
  opened = false;
}

void ShmObject::unlink() {
  close();
  static_cast<void>(wrap_shm_unlink(name));
}