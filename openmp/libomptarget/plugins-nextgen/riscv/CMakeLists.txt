
libomptarget_say("Building RISCV NextGen offloading plugin.")


add_llvm_library("omptarget.rtl.riscv" SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rtl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/riscvaccel/riscvaccel.cpp
  
  LINK_COMPONENTS
  Support
  Object

  LINK_LIBS PRIVATE
  elf_common
  MemoryManager
  PluginInterface
  ${OPENMP_PTHREAD_LIB}

  NO_INSTALL_RPATH
)

if ((OMPT_TARGET_DEFAULT) AND (LIBOMPTARGET_OMPT_SUPPORT))
  target_link_libraries(omptarget.rtl.riscv PRIVATE OMPT)
endif()

if (LIBOMP_HAVE_VERSION_SCRIPT_FLAG)
  target_link_libraries(omptarget.rtl.riscv PRIVATE
  "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/../exports,-z,defs")
endif()


# Define debug prefix. TODO: This should be automatized in the Debug.h but it
# requires changing the original plugins.
target_compile_definitions(omptarget.rtl.riscv PRIVATE TARGET_NAME="CUDA")
target_compile_definitions(omptarget.rtl.riscv PRIVATE DEBUG_PREFIX="TARGET RISCV RTL")

target_include_directories("omptarget.rtl.riscv" PRIVATE ${LIBOMPTARGET_INCLUDE_DIR})

# Install plugin under the lib destination folder.
install(TARGETS "omptarget.rtl.riscv" LIBRARY DESTINATION "${OPENMP_INSTALL_LIBDIR}")
set_target_properties("omptarget.rtl.riscv" PROPERTIES
  INSTALL_RPATH "$ORIGIN" BUILD_RPATH "$ORIGIN:${CMAKE_CURRENT_BINARY_DIR}/.."
  CXX_VISIBILITY_PRESET protected)
