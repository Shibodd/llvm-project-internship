#ifndef SHM_DEVICE_HPP
#define SHM_DEVICE_HPP

#include "PluginInterface.h"
#include "llvm/Frontend/OpenMP/OMPGridValues.h"
#include "shm_debug.hpp"
#include "shm_allocator.hpp"
#include "shm_object.hpp"
#include "shm_err.hpp"
#include "control_register.hpp"


namespace llvm {
namespace omp {
namespace target {
namespace plugin {

// TODO: decide whether this makes sense at all
static constexpr GV HsmGridValues = {
  1,       // GV_Slot_Size
  1,        // GV_Warp_Size
  1, // GV_Max_Teams
  1,      // GV_Default_Num_Teams
  1,       // GV_SimpleBufferSize
  1,      // GV_Max_WG_Size
  1,       // GV_Default_WG_Size
};

struct ShmDeviceTy : public GenericDeviceTy {
  ShmDeviceTy(int32_t DeviceId, int32_t NumDevices)
      : GenericDeviceTy(DeviceId, NumDevices, HsmGridValues),
        shm_object("/shm_dev" + std::to_string(DeviceId)),
        shm_allocator(),
        device_ctrl_reg(nullptr) {}

  /// Allocate a memory of size \p Size . \p HstPtr is used to assist the
  /// allocation.
  virtual void *allocate(size_t Size, void *HstPtr,
                         TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override {
    SHM_NOT_IMPLEMENTED;
    return nullptr;
  }

  /// Delete the pointer \p TgtPtr on the device
  virtual int free(void *TgtPtr, TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override {
    SHM_NOT_IMPLEMENTED;
    return OFFLOAD_SUCCESS;
  }

  /// Set the context of the device if needed, before calling device-specific
  /// functions. Plugins may implement this function as a no-op if not needed.
  virtual Error setContext() override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize the device. After this call, the device should be already
  /// working and ready to accept queries or modifications.
  virtual Error initImpl(GenericPluginTy &Plugin) override {
    SHM_TRACE_FN;

    constexpr size_t MiB = 1024 * 1024;
    if (auto err = shm_object.create(4*MiB))
      return err;
    
    shm_allocator = MyAllocator(shm_object.get_span());
    device_ctrl_reg = (ShmDeviceControlRegister*)shm_allocator.allocate(sizeof(ShmDeviceControlRegister));

    if (device_ctrl_reg == nullptr) {
      shm_object.unlink();
      ASSERT_OR_ERR(false, "Failed to allocate Device Control Register!");
    }

    device_ctrl_reg->echo = 0;

    while (device_ctrl_reg->echo == 0)
    {

    }
    device_ctlr_reg->echo = 0;

    return Plugin::success();
  }

  /// Deinitialize the device and free all its resources. After this call, the
  /// device is no longer considered ready, so no queries or modifications are
  /// allowed.
  virtual Error deinitImpl() override {
    SHM_TRACE_FN;

    shm_object.unlink();
    device_ctrl_reg = nullptr;
    shm_allocator = MyAllocator();
    
    return Plugin::success();
  }

  /// Load the binary image into the device and return the target table.
  virtual Expected<DeviceImageTy *> loadBinaryImpl(const __tgt_device_image *TgtImage, int32_t ImageId) override {
    SHM_NOT_IMPLEMENTED;
    return new DeviceImageTy(ImageId, TgtImage);
  }

  /// Synchronize the current thread with the pending operations on the
  /// __tgt_async_info structure.
  virtual Error synchronizeImpl(__tgt_async_info &AsyncInfo) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Query for the completion of the pending operations on the __tgt_async_info
  /// structure in a non-blocking manner.
  virtual Error queryAsyncImpl(__tgt_async_info &AsyncInfo) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Lock the host buffer \p HstPtr with \p Size bytes with the vendor-specific
  /// API and return the device accessible pointer.
  virtual Expected<void *> dataLockImpl(void *HstPtr, int64_t Size) override {
    SHM_NOT_IMPLEMENTED;
    return nullptr;
  }

  /// Unlock a previously locked host buffer starting at \p HstPtr.
  virtual Error dataUnlockImpl(void *HstPtr) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Check whether the host buffer with address \p HstPtr is pinned by the
  /// underlying vendor-specific runtime (if any). Retrieve the host pointer,
  /// the device accessible pointer and the size of the original pinned buffer.
  virtual Expected<bool> isPinnedPtrImpl(void *HstPtr, void *&BaseHstPtr,
                                         void *&BaseDevAccessiblePtr,
                                         size_t &BaseSize) const override {
    SHM_NOT_IMPLEMENTED;
    HstPtr = nullptr;
    BaseHstPtr = nullptr;
    BaseDevAccessiblePtr = nullptr;
    BaseSize = 0;
    return false;
  }

  /// Submit data to the device (host to device transfer).
  virtual Error dataSubmitImpl(void *TgtPtr, const void *HstPtr, int64_t Size,
                               AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Retrieve data from the device (device to host transfer).
  virtual Error dataRetrieveImpl(void *HstPtr, const void *TgtPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Exchange data between devices (device to device transfer). Calling this
  /// function is only valid if GenericPlugin::isDataExchangable() passing the
  /// two devices returns true.
  virtual Error dataExchangeImpl(const void *SrcPtr, GenericDeviceTy &DstDev,
                                 void *DstPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize a __tgt_async_info structure. Related to interop features.
  virtual Error initAsyncInfoImpl(AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize a __tgt_device_info structure. Related to interop features.
  virtual Error initDeviceInfoImpl(__tgt_device_info *DeviceInfo) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Create an event.
  virtual Error createEventImpl(void **EventPtrStorage) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Destroy an event.
  virtual Error destroyEventImpl(void *EventPtr) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Start the recording of the event.
  virtual Error recordEventImpl(void *EventPtr,
                                AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Wait for an event to finish. Notice this wait is asynchronous if the
  /// __tgt_async_info is not nullptr.
  virtual Error waitEventImpl(void *EventPtr,
                              AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Synchronize the current thread with the event.
  virtual Error syncEventImpl(void *EventPtr) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Print information about the device.
  virtual Error obtainInfoImpl(InfoQueueTy &Info) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }

private:
  ShmObject shm_object;
  MyAllocator shm_allocator;
  volatile ShmDeviceControlRegister* device_ctrl_reg;

  /// Allocate and construct a kernel object.
  virtual Expected<GenericKernelTy &>
  constructKernel(const __tgt_offload_entry &KernelEntry,
                  OMPTgtExecModeFlags ExecMode) override {
    SHM_TRACE_FN;
    ShmKernelTy *ShmKernel = Plugin::get().allocate<ShmKernelTy>();
    if (!ShmKernel)
      return Plugin::error("Failed to allocate memory for kernel");

    new (ShmKernel) ShmKernelTy(KernelEntry.name, ExecMode);

    return *ShmKernel;
  }

  /// Get and set the stack size and heap size for the device. If not used, the
  /// plugin can implement the setters as no-op and setting the output
  /// value to zero for the getters.
  virtual Error getDeviceStackSize(uint64_t &V) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }
  virtual Error setDeviceStackSize(uint64_t V) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }
  virtual Error getDeviceHeapSize(uint64_t &V) override{
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }
  virtual Error setDeviceHeapSize(uint64_t V) override {
    SHM_NOT_IMPLEMENTED;
    return Plugin::success();
  }
};

GenericDeviceTy *Plugin::createDevice(int32_t DeviceId, int32_t NumDevices) {
  return new ShmDeviceTy(DeviceId, NumDevices);
}

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_HPP