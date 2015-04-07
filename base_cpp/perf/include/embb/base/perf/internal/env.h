#ifndef EMBB_BASE_CPP_PERF_INTERNAL_ENV_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_ENV_H_

#include <embb/base/c/internal/config.h>

// Windows
#ifdef EMBB_PLATFORM_THREADING_WINTHREADS
#define EMBB_BASE_CPP_PERF_TIMER_WIN32
#endif
// OS X
#if defined(__MACH__) && defined(__APPLE__)
#define EMBB_BASE_CPP_PERF_TIMER_OSX
#endif
// HPUX / Sun
#if (defined(__hpux) || defined(hpux)) || \
((defined(__sun__) || defined(__sun) || defined(sun)) && \
(defined(__SVR4) || defined(__svr4__)))
#define EMBB_BASE_CPP_PERF_TIMER_UX
#endif
// POSIX
#if defined(EMBB_PLATFORM_THREADING_POSIXTHREADS)
#define EMBB_BASE_CPP_PERF_TIMER_POSIX
#endif
// Linux
#if defined(__linux__)
#define EMBB_BASE_CPP_PERF_TIMER_LINUX
#endif
// FreeBSD
#if defined(__FreeBSD__)
#define EMBB_BASE_CPP_PERF_TIMER_FREEBSD
#endif
// C++11 (std::chrono)
#if (__cplusplus > 199711L)
#define EMBB_BASE_CPP_PERF_TIMER_CXX11__DISABLED__
#endif

// Architecture specific defines

// Intel 386
#if defined(EMBB_PLATFORM_ARCH_X86_32)
#define EMBB_BASE_CPP_PERF__ARCH_I386
#define EMBB_BASE_CPP_PERF__ARCH_X86

// AMD64, Intel x64
#elif defined(EMBB_PLATFORM_ARCH_X86_64)
#define EMBB_BASE_CPP_PERF__ARCH_X64
#define EMBB_BASE_CPP_PERF__ARCH_X86

// ARM
#elif defined(EMBB_PLATFORM_ARCH_ARM)
// ARM versions consolidated to major architecture version. 
// See: https://wiki.edubuntu.org/ARM/Thumb2PortingHowto
#if defined(__ARM_ARCH_7__) || \
    defined(__ARM_ARCH_7R__) || \
    defined(__ARM_ARCH_7A__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV7 1
#endif
#if defined(EMBB_BASE_CPP_PERF__ARCH_ARMV7) || \
    defined(__ARM_ARCH_6__) || \
    defined(__ARM_ARCH_6J__) || \
    defined(__ARM_ARCH_6K__) || \
    defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_6ZK__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV6 1
#endif
#if defined(EMBB_BASE_CPP_PERF__ARCH_ARMV6) || \
    defined(__ARM_ARCH_5T__) || \
    defined(__ARM_ARCH_5E__) || \
    defined(__ARM_ARCH_5TE__) || \
    defined(__ARM_ARCH_5TEJ__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV5 1
#endif
#if defined(EMBB_BASE_CPP_PERF__ARCH_ARMV5) || \
    defined(__ARM_ARCH_4__) || \
    defined(__ARM_ARCH_4T__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV4 1
#endif
#if defined(EMBB_BASE_CPP_PERF__ARCH_ARMV4) || \
    defined(__ARM_ARCH_3__) || \
    defined(__ARM_ARCH_3M__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV3 1
#endif
#if defined(EMBB_BASE_CPP_PERF__ARCH_ARMV3) || \
    defined(__ARM_ARCH_2__)
#define EMBB_BASE_CPP_PERF__ARCH_ARMV2 1
#define EMBB_BASE_CPP_PERF__ARCH_ARM 1
#endif

#endif // Architecture

#if defined(__PAPI__)
#define EMBB_BASE_CPP_PERF_TIMER_PAPI
#endif

#endif // EMBB_BASE_CPP_PERF_INTERNAL_ENV_H_

