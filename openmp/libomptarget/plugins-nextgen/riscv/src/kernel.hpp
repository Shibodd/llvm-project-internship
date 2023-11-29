#ifndef RISCV_KERNEL_HPP
#define RISCV_KERNEL_HPP

#include "PluginInterface.h"
#include "debug_helpers.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

/// Class implementing common functionalities of offload kernels. Each plugin
/// should define the specific kernel class, derive from this generic one, and
/// implement the necessary virtual function members.
struct RiscvKernelTy : public GenericKernelTy {
  RiscvKernelTy(const char *Name, OMPTgtExecModeFlags ExecMode)
      : GenericKernelTy(Name, ExecMode) {}

  virtual ~RiscvKernelTy() {}

  /// Initialize the kernel object from a specific device.
  virtual Error initImpl(GenericDeviceTy &GenericDevice,
                         DeviceImageTy &Image) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Launch the kernel on the specific device. The device must be the same
  /// one used to initialize the kernel.
  virtual Error launchImpl(GenericDeviceTy &GenericDevice, uint32_t NumThreads,
                           uint64_t NumBlocks, KernelArgsTy &KernelArgs,
                           void *Args,
                           AsyncInfoWrapperTy &AsyncInfoWrapper) const override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }
};


} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !RISCV_DEVICE_HPP