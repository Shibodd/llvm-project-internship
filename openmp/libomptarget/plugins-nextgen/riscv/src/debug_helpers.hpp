#ifndef DEBUG_HELPERS_HPP
#define DEBUG_HELPERS_HPP


#include "Debug.h" // Libomptarget Debug (for DP)

#define RISCV_DP(s, ...) DP("[%s]" s, __func__ __VA_OPT__(,) __VA_ARGS__)
#define RISCV_NOT_IMPLEMENTED RISCV_DP("Not implemented!")


#ifdef RISCV_TRACE_ENABLED
#define RISCV_TRACE_FN DP("Entering " __func__ ".")
#else
#define RISCV_TRACE_FN
#endif // RISCV_TRACE_ENABLED

#endif // !DEBUG_HELPERS_HPP