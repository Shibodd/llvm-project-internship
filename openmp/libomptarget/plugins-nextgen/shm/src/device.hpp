#ifndef SHM_DEVICE_HPP
#define SHM_DEVICE_HPP

#include "PluginInterface.h"
#include "llvm/Frontend/OpenMP/OMPGridValues.h"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

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
      : GenericDeviceTy(DeviceId, NumDevices, HsmGridValues) {}

  /// Allocate a memory of size \p Size . \p HstPtr is used to assist the
  /// allocation.
  virtual void *allocate(size_t Size, void *HstPtr,
                         TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override { };

  /// Delete the pointer \p TgtPtr on the device
  virtual int free(void *TgtPtr, TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override { };

  /// Set the context of the device if needed, before calling device-specific
  /// functions. Plugins may implement this function as a no-op if not needed.
  virtual Error setContext() override { }

  /// Initialize the device. After this call, the device should be already
  /// working and ready to accept queries or modifications.
  virtual Error initImpl(GenericPluginTy &Plugin) override { }

  /// Deinitialize the device and free all its resources. After this call, the
  /// device is no longer considered ready, so no queries or modifications are
  /// allowed.
  virtual Error deinitImpl() override { }

  /// Load the binary image into the device and return the target table.
  virtual Expected<DeviceImageTy *> loadBinaryImpl(const __tgt_device_image *TgtImage, int32_t ImageId) override { }

  /// Synchronize the current thread with the pending operations on the
  /// __tgt_async_info structure.
  virtual Error synchronizeImpl(__tgt_async_info &AsyncInfo) override { }

  /// Query for the completion of the pending operations on the __tgt_async_info
  /// structure in a non-blocking manner.
  virtual Error queryAsyncImpl(__tgt_async_info &AsyncInfo) override { }

  /// Lock the host buffer \p HstPtr with \p Size bytes with the vendor-specific
  /// API and return the device accessible pointer.
  virtual Expected<void *> dataLockImpl(void *HstPtr, int64_t Size) override { }

  /// Unlock a previously locked host buffer starting at \p HstPtr.
  virtual Error dataUnlockImpl(void *HstPtr) override { }

  /// Check whether the host buffer with address \p HstPtr is pinned by the
  /// underlying vendor-specific runtime (if any). Retrieve the host pointer,
  /// the device accessible pointer and the size of the original pinned buffer.
  virtual Expected<bool> isPinnedPtrImpl(void *HstPtr, void *&BaseHstPtr,
                                         void *&BaseDevAccessiblePtr,
                                         size_t &BaseSize) const override { }

  /// Submit data to the device (host to device transfer).
  virtual Error dataSubmitImpl(void *TgtPtr, const void *HstPtr, int64_t Size,
                               AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Retrieve data from the device (device to host transfer).
  virtual Error dataRetrieveImpl(void *HstPtr, const void *TgtPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Exchange data between devices (device to device transfer). Calling this
  /// function is only valid if GenericPlugin::isDataExchangable() passing the
  /// two devices returns true.
  virtual Error dataExchangeImpl(const void *SrcPtr, GenericDeviceTy &DstDev,
                                 void *DstPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Initialize a __tgt_async_info structure. Related to interop features.
  virtual Error initAsyncInfoImpl(AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Initialize a __tgt_device_info structure. Related to interop features.
  virtual Error initDeviceInfoImpl(__tgt_device_info *DeviceInfo) override { }

  /// Create an event.
  virtual Error createEventImpl(void **EventPtrStorage) override { }

  /// Destroy an event.
  virtual Error destroyEventImpl(void *EventPtr) override { }

  /// Start the recording of the event.
  virtual Error recordEventImpl(void *EventPtr,
                                AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Wait for an event to finish. Notice this wait is asynchronous if the
  /// __tgt_async_info is not nullptr.
  virtual Error waitEventImpl(void *EventPtr,
                              AsyncInfoWrapperTy &AsyncInfoWrapper) override { }

  /// Synchronize the current thread with the event.
  virtual Error syncEventImpl(void *EventPtr) override { }

  /// Print information about the device.
  virtual Error obtainInfoImpl(InfoQueueTy &Info) override { }

private:

  /// Allocate and construct a kernel object.
  virtual Expected<GenericKernelTy &>
  constructKernel(const __tgt_offload_entry &KernelEntry,
                  OMPTgtExecModeFlags ExecMode) override { }

  /// Get and set the stack size and heap size for the device. If not used, the
  /// plugin can implement the setters as no-op and setting the output
  /// value to zero for the getters.
  virtual Error getDeviceStackSize(uint64_t &V) override { }
  virtual Error setDeviceStackSize(uint64_t V) override { }
  virtual Error getDeviceHeapSize(uint64_t &V) override { }
  virtual Error setDeviceHeapSize(uint64_t V) override { }
};

GenericDeviceTy *Plugin::createDevice(int32_t DeviceId, int32_t NumDevices) {
  return new ShmDeviceTy(DeviceId, NumDevices);
}

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_HPP