#ifndef RISCV_KERNEL_HPP
#define RISCV_KERNEL_HPP

#include "PluginInterface.h"
#include "debug_helpers.hpp"
#include "riscvaccel/riscvaccel.hpp"

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

  uint64_t entry_point;

  /// Initialize the kernel object from a specific device.
  virtual Error initImpl(GenericDeviceTy &GenericDevice,
                         DeviceImageTy &Image) override {
    RISCV_TRACE_FN;

    GlobalTy Global(getName(), 0);

    // Get the metadata (address) of the kernel function.
    GenericGlobalHandlerTy &GHandler = Plugin::get().getGlobalHandler();
    if (auto Err = GHandler.getGlobalMetadataFromDevice(GenericDevice, Image, Global))
      return Err;

    // Check that the function pointer is valid.
    if (!Global.getPtr())
      return Plugin::error("Invalid function for kernel %s", getName());

    // Save the function pointer.
    entry_point = (uint64_t)Global.getPtr();

    return Plugin::success();
  }

  /// Launch the kernel on the specific device. The device must be the same
  /// one used to initialize the kernel.
  virtual Error launchImpl(GenericDeviceTy &GenericDevice, uint32_t NumThreads,
                           uint64_t NumBlocks, KernelArgsTy &KernelArgs,
                           void *Args,
                           AsyncInfoWrapperTy &AsyncInfoWrapper) const override {
    RISCV_TRACE_FN;

    uint64_t stack = riscvaccel_malloc_host(4096);
    riscvaccel_launch(entry_point, stack, KernelArgs.NumArgs, (uint64_t*)*(void**)Args);
    riscvaccel_wait_until_cmplt();
    riscvaccel_free_host(stack);

    return Plugin::success();
  }
};


} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !RISCV_DEVICE_HPP