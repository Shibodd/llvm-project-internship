#ifndef SHM_DEBUG_HPP
#define SHM_DEBUG_HPP


#include "Debug.h" // Libomptarget Debug (for DP)

#define SHM_DP(s, ...) DP("[%s] " s "\n", __func__ __VA_OPT__(,) __VA_ARGS__)
#define SHM_NOT_IMPLEMENTED SHM_DP("Not implemented!\n")


#ifdef SHM_TRACE_ENABLED
#define SHM_TRACE_FN DP("Entering " __func__ ".")
#else
#define SHM_TRACE_FN
#endif // SHM_TRACE_ENABLED

#endif // !SHM_DEBUG_HPP