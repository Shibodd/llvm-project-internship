#ifndef SHM_KERNEL_HPP
#define SHM_KERNEL_HPP

#include "PluginInterface.h"
#include "shm_debug.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

/// Class implementing common functionalities of offload kernels. Each plugin
/// should define the specific kernel class, derive from this generic one, and
/// implement the necessary virtual function members.
struct ShmKernelTy : public GenericKernelTy {
  ShmKernelTy(const char *Name, OMPTgtExecModeFlags ExecMode)
      : GenericKernelTy(Name, ExecMode) {}

  virtual ~ShmKernelTy() {}

  /// Initialize the kernel object from a specific device.
  virtual Error initImpl(GenericDeviceTy &GenericDevice,
                         DeviceImageTy &Image) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Launch the kernel on the specific device. The device must be the same
  /// one used to initialize the kernel.
  virtual Error launchImpl(GenericDeviceTy &GenericDevice, uint32_t NumThreads,
                           uint64_t NumBlocks, KernelArgsTy &KernelArgs,
                           void *Args,
                           AsyncInfoWrapperTy &AsyncInfoWrapper) const override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }
};


} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_HPP