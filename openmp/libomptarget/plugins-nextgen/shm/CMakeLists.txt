
libomptarget_say("Building shm NextGen offloading plugin.")

# Define macro to be used as prefix of the runtime messages for this target.
add_definitions("-DTARGET_NAME=shm")

# Define debug prefix. TODO: This should be automatized in the Debug.h but
# it requires changing the original plugins.
add_definitions(-DDEBUG_PREFIX="TARGET shm RTL")
add_definitions(-DSHM_TRACE_ENABLED=1)

add_llvm_library("omptarget.rtl.shm"
  SHARED

  ${CMAKE_CURRENT_SOURCE_DIR}/src/rtl.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/shm_support/shm_support.cpp

  ADDITIONAL_HEADER_DIRS
  ${LIBOMPTARGET_INCLUDE_DIR}

    LINK_LIBS
    PRIVATE
    elf_common
    MemoryManager
    OMPT
    PluginInterface
    ${OPENMP_PTHREAD_LIB}

  NO_INSTALL_RPATH
)

if (LIBOMP_HAVE_VERSION_SCRIPT_FLAG)
  target_link_libraries("omptarget.rtl.shm" PRIVATE
    "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/../exports")
endif()

# Install plugin under the lib destination folder.
install(TARGETS "omptarget.rtl.shm"
  LIBRARY DESTINATION "${OPENMP_INSTALL_LIBDIR}")
set_target_properties("omptarget.rtl.shm" PROPERTIES
  INSTALL_RPATH "$ORIGIN" BUILD_RPATH "$ORIGIN:${CMAKE_CURRENT_BINARY_DIR}/.."
  CXX_VISIBILITY_PRESET protected)

target_include_directories( "omptarget.rtl.shm" PRIVATE
  ${LIBOMPTARGET_INCLUDE_DIR})

list(APPEND LIBOMPTARGET_TESTED_PLUGINS "omptarget.rtl.shm")
set(LIBOMPTARGET_TESTED_PLUGINS
    "${LIBOMPTARGET_TESTED_PLUGINS}" PARENT_SCOPE)
set(LIBOMPTARGET_SYSTEM_TARGETS 
    "${LIBOMPTARGET_SYSTEM_TARGETS} shm shm-LTO" PARENT_SCOPE)