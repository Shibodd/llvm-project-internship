#ifndef SHM_DEVICE_IMAGE_HPP
#define SHM_DEVICE_IMAGE_HPP

#include "PluginInterface.h"
#include "shm_helpers/shm_debug.hpp"
#include "shm_helpers/shm_errs.hpp"
#include "shm_support/shm_support.hpp"
#include <fstream>
#include "elf_img_loader/elf_loader.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

struct ShmDeviceImageTy : public DeviceImageTy {
  ShmDeviceImageTy(int32_t Id, const __tgt_device_image *Image)
      : DeviceImageTy(Id, Image), 
        shm_object("/shm_img" + std::to_string(Id)), 
        elf_loader(
          { (char*)Image->ImageStart, (char*)Image->ImageEnd },
          [this](size_t s) { return mapper(s); }, 
          [this](Span<char> mem) { return unmapper(mem); }
        )
  { }

  Error create_shm_object() {
    SHM_TRACE_FN;
    

    /* // Dump the image for debug
    std::ofstream file;
    file.open("/home/sbondi/workspace/test/openmp_IR/devimg.img", std::ios::binary | std::ios::trunc);
    file.write(static_cast<char*>(getStart()), getSize());
    file.close();
    */

    return elf_loader.load();
  }

  Error delete_shm_object() {
    SHM_TRACE_FN;
    return elf_loader.unload();
  }

private:
  ShmObject shm_object;
  ElfLoader elf_loader;

  Expected<Span<char>> mapper(size_t size) {
    ASSERT_OR_ERR(!shm_object.is_open(), "The SHM object is already opened.");

    if (auto err = shm_object.create(size))
      return err;

    return shm_object.get_span();
  }

  Error unmapper(Span<char> area) {
    shm_object.close();
    return make_success();
  }
};

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_IMAGE_HPP