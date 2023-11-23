#ifndef SHM_DEVICE_IMAGE_HPP
#define SHM_DEVICE_IMAGE_HPP

#include "PluginInterface.h"
#include "shm_debug.hpp"
#include "shm_support/shm_support.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

struct ShmDeviceImageTy : public DeviceImageTy {
  ShmDeviceImageTy(int32_t Id, const __tgt_device_image *Image)
      : DeviceImageTy(Id, Image), shm_object("/shm_img" + std::to_string(Id)) { }

  Error create_shm_object() {
    SHM_TRACE_FN;
    assert(!shm_object.is_open() && "The SHM object is already opened.");
    
    size_t size = getSize();
    if (auto err = shm_object.create(size))
      return err;

    void* addr = shm_object.get_address();
    std::memcpy(addr, getStart(), size);

    return Plugin::success();
  }

  Error delete_shm_object() {
    SHM_TRACE_FN;
    assert(!shm_object.is_open() && "The SHM object is not opened.");
    shm_object.close();
    return Plugin::success();
  }
private:
  ShmObject shm_object;
};

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_IMAGE_HPP