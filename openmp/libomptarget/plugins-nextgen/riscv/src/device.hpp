#ifndef RISCV_DEVICE_HPP
#define RISCV_DEVICE_HPP

#include "PluginInterface.h"
#include "llvm/Frontend/OpenMP/OMPGridValues.h"
#include "debug_helpers.hpp"
#include "riscvaccel/riscvaccel.hpp"
#include "riscvaccel/elf_loader.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

static constexpr GV RiscvGridValues = {
  1, // GV_Slot_Size
  1, // GV_Warp_Size
  1, // GV_Max_Teams
  1, // GV_Default_Num_Teams
  1, // GV_SimpleBufferSize
  1, // GV_Max_WG_Size
  1, // GV_Default_WG_Size
};

struct RiscvDeviceTy : public GenericDeviceTy {
  RiscvDeviceTy(int32_t DeviceId, int32_t NumDevices)
      : GenericDeviceTy(DeviceId, NumDevices, RiscvGridValues) {}

  Error fixup_relas(const ELFFile<ELF64LE> &Elf);
  Error get_rela_fixup_value(const Elf_Rel_Impl<ELF64LE, true>& rela, uint64_t& base_fixup);
  Error map_to_mem(const ELFFile<ELF64LE> &Elf);

  /// Allocate a memory of size \p Size . \p HstPtr is used to assist the
  /// allocation.
  virtual void *allocate(size_t Size, void *HstPtr,
                         TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override {
    RISCV_TRACE_FN;
    return (void*)riscvaccel_malloc_host(Size);
  }

  /// Delete the pointer \p TgtPtr on the device
  virtual int free(void *TgtPtr, TargetAllocTy Kind = TARGET_ALLOC_DEFAULT) override {
    RISCV_TRACE_FN;
    riscvaccel_free_host((uint64_t)TgtPtr);
    return OFFLOAD_SUCCESS;
  }

  /// Set the context of the device if needed, before calling device-specific
  /// functions. Plugins may implement this function as a no-op if not needed.
  virtual Error setContext() override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize the device. After this call, the device should be already
  /// working and ready to accept queries or modifications.
  virtual Error initImpl(GenericPluginTy &Plugin) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  virtual bool shouldSetupDeviceEnvironment() const override { return false; }

  /// Deinitialize the device and free all its resources. After this call, the
  /// device is no longer considered ready, so no queries or modifications are
  /// allowed.
  virtual Error deinitImpl() override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  /// Load the binary image into the device and return the target table.
  virtual Expected<DeviceImageTy *> loadBinaryImpl(const __tgt_device_image *TgtImage, int32_t ImageId) override {
    RISCV_TRACE_FN;
    
    std::unique_ptr<DeviceImageTy> Image = std::make_unique<DeviceImageTy>(ImageId, TgtImage);
    Expected<ELF64LEObjectFile> ElfOrErr = ELF64LEObjectFile::create(Image->getMemoryBuffer());
    const ELFFile<ELF64LE> &Elf = ElfOrErr.get().getELFFile();

    size_t MemSize = get_mem_size(Elf);
    if (MemSize <= 0) {
      return Plugin::error("Failed to get memory size of binary from ELF.");
    }

    base_address = (uint64_t)allocate(MemSize, nullptr, TARGET_ALLOC_DEFAULT);
    if (base_address == 0) {
      return Plugin::error("Failed to allocate memory.");
    }

    if (auto err = map_to_mem(Elf)) {
      free((void*)base_address, TARGET_ALLOC_DEFAULT);
      return err;
    }

    if (auto err = fixup_relas(Elf)) {
      free((void*)base_address, TARGET_ALLOC_DEFAULT);
      return err;
    }

    return Image.release();
  }

  /// Synchronize the current thread with the pending operations on the
  /// __tgt_async_info structure.
  virtual Error synchronizeImpl(__tgt_async_info &AsyncInfo) override {
    RISCV_TRACE_FN;
    AsyncInfo.Queue = nullptr;
    return Plugin::success();
  }

  /// Query for the completion of the pending operations on the __tgt_async_info
  /// structure in a non-blocking manner.
  virtual Error queryAsyncImpl(__tgt_async_info &AsyncInfo) override {
    RISCV_TRACE_FN;
    AsyncInfo.Queue = nullptr;
    return Plugin::success();
  }

  /// Lock the host buffer \p HstPtr with \p Size bytes with the vendor-specific
  /// API and return the device accessible pointer.
  virtual Expected<void *> dataLockImpl(void *HstPtr, int64_t Size) override {
    RISCV_NOT_IMPLEMENTED;
    return nullptr;
  }

  /// Unlock a previously locked host buffer starting at \p HstPtr.
  virtual Error dataUnlockImpl(void *HstPtr) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Check whether the host buffer with address \p HstPtr is pinned by the
  /// underlying vendor-specific runtime (if any). Retrieve the host pointer,
  /// the device accessible pointer and the size of the original pinned buffer.
  virtual Expected<bool> isPinnedPtrImpl(void *HstPtr, void *&BaseHstPtr,
                                         void *&BaseDevAccessiblePtr,
                                         size_t &BaseSize) const override {
    RISCV_TRACE_FN;
    HstPtr = nullptr;
    BaseHstPtr = nullptr;
    BaseDevAccessiblePtr = nullptr;
    BaseSize = 0;
    return false;
  }

  /// Submit data to the device (host to device transfer).
  virtual Error dataSubmitImpl(void *TgtPtr, const void *HstPtr, int64_t Size,
                               AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_TRACE_FN;
    riscvaccel_memcpy_h2d((uint64_t)TgtPtr, (uint64_t)HstPtr, Size);
    return Plugin::success();
  }

  /// Retrieve data from the device (device to host transfer).
  virtual Error dataRetrieveImpl(void *HstPtr, const void *TgtPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_TRACE_FN;
    riscvaccel_memcpy_d2h((uint64_t)HstPtr, (uint64_t)TgtPtr, Size);
    return Plugin::success();
  }

  /// Exchange data between devices (device to device transfer). Calling this
  /// function is only valid if GenericPlugin::isDataExchangable() passing the
  /// two devices returns true.
  virtual Error dataExchangeImpl(const void *SrcPtr, GenericDeviceTy &DstDev,
                                 void *DstPtr, int64_t Size,
                                 AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize a __tgt_async_info structure. Related to interop features.
  virtual Error initAsyncInfoImpl(AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Initialize a __tgt_device_info structure. Related to interop features.
  virtual Error initDeviceInfoImpl(__tgt_device_info *DeviceInfo) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  /// Create an event.
  virtual Error createEventImpl(void **EventPtrStorage) override {
    RISCV_TRACE_FN;
    EventPtrStorage = nullptr;
    return Plugin::success();
  }

  /// Destroy an event.
  virtual Error destroyEventImpl(void *EventPtr) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  /// Start the recording of the event.
  virtual Error recordEventImpl(void *EventPtr,
                                AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  /// Wait for an event to finish. Notice this wait is asynchronous if the
  /// __tgt_async_info is not nullptr.
  virtual Error waitEventImpl(void *EventPtr,
                              AsyncInfoWrapperTy &AsyncInfoWrapper) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  /// Synchronize the current thread with the event.
  virtual Error syncEventImpl(void *EventPtr) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }

  /// Print information about the device.
  virtual Error obtainInfoImpl(InfoQueueTy &Info) override {
    RISCV_NOT_IMPLEMENTED;
    return Plugin::success();
  }

  uint64_t getBaseAddress() { return base_address; }
private:
  uint64_t base_address;

  /// Allocate and construct a kernel object.
  virtual Expected<GenericKernelTy &>
  constructKernel(const __tgt_offload_entry &KernelEntry,
                  OMPTgtExecModeFlags ExecMode) override {
    RISCV_TRACE_FN;
    RiscvKernelTy *RiscvKernel = Plugin::get().allocate<RiscvKernelTy>();
    if (!RiscvKernel)
      return Plugin::error("Failed to allocate memory for kernel");

    new (RiscvKernel) RiscvKernelTy(KernelEntry.name, ExecMode);

    return *RiscvKernel;
  }

  /// Get and set the stack size and heap size for the device. If not used, the
  /// plugin can implement the setters as no-op and setting the output
  /// value to zero for the getters.
  virtual Error getDeviceStackSize(uint64_t &V) override {
    RISCV_TRACE_FN;
    V = 0;
    return Plugin::success();
  }
  virtual Error setDeviceStackSize(uint64_t V) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }
  virtual Error getDeviceHeapSize(uint64_t &V) override{
    RISCV_TRACE_FN;
    V = 0;
    return Plugin::success();
  }
  virtual Error setDeviceHeapSize(uint64_t V) override {
    RISCV_TRACE_FN;
    return Plugin::success();
  }
};

Error RiscvDeviceTy::map_to_mem(const ELFFile<ELF64LE> &Elf) {
  for (auto &phdr : Elf.program_headers().get()) {
    if (phdr.p_type == ELF::PT_LOAD && phdr.p_memsz > 0) {
      auto r = Elf.getSegmentContents(phdr).get();

      void* dst = (void*)(reinterpret_cast<char*>(base_address) + (uint64_t)phdr.p_vaddr);
      void* src = (void*)(r.data());
      size_t len = r.size();

      DP("%p -> %p (%zu)\n", dst, src, len);

      if (auto err = dataSubmit(dst, src, len, nullptr))
        return err;
    }
  }
  return Plugin::success();
}





Error RiscvDeviceTy::get_rela_fixup_value(const Elf_Rel_Impl<ELF64LE, true>& rela, uint64_t& base_fixup) {
  uint32_t type = rela.getType(false);

  switch (type) {
  case ELF::R_X86_64_JUMP_SLOT:
  case ELF::R_X86_64_GLOB_DAT:
    break;
  case ELF::R_X86_64_RELATIVE:
    base_fixup += (uint64_t)rela.r_addend;
    break;
  default:
    return Plugin::error("Unsupported rela type %lu", type);
  }
  return Plugin::success();
}

Error RiscvDeviceTy::fixup_relas(const ELFFile<ELF64LE> &Elf) {
  for (auto &shdr : Elf.sections().get()) {
    if (shdr.sh_type == ELF::SHT_RELA) {
      auto relas = Elf.relas(shdr).get();
      auto symTab = Elf.getSection(shdr.sh_link).get();
      // auto symStrTab = Elf.getSection(symTab->sh_link).get();

      for (auto &rela : relas) {

        auto sym = Elf.getRelocationSymbol(rela, symTab).get();
        if (sym == nullptr)
          continue;

        uint64_t loadAddress = (uint64_t)getBaseAddress();

        uint64_t *fixup_location = (uint64_t *)(loadAddress + rela.r_offset);
        uint64_t fixup_with = loadAddress + sym->getValue();

        if (auto err = get_rela_fixup_value(rela, fixup_with))
          return err;

        DP("Fixup rela @%p with %p\n", fixup_location, (void *)fixup_with);
        if (auto err = dataSubmit(fixup_location, &fixup_with, sizeof(uint64_t),
                                  nullptr))
          return err;
      }
    } else if (shdr.sh_type == ELF::SHT_REL || shdr.sh_type == ELF::SHT_RELR) {
      return Plugin::error("Unsupported section type %lu",
                          (uint64_t)shdr.sh_type);
    }
  }
  return Plugin::success();
}


} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !RISCV_DEVICE_HPP