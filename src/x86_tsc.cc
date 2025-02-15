// C++ standard headers
#include <chrono>
#include <thread>
#include <cmath>

// for usleep
#ifndef _WIN32
#include <unistd.h>
#endif

#include "interface/x86_tsc.h"

#ifdef CHRONO_HAVE_TSC

// for rdtsc, rdtscp, lfence, mfence, cpuid
#ifdef CHRONO_HAVE_X86_INTRINSICS
#include <emmintrin.h>
#endif
#if !defined(_MSC_VER)
#include <cpuid.h>
#else
#include <windows.h>
#endif

#ifdef __linux__
#include <linux/version.h>
#include <sys/prctl.h>
#endif // __linux__


// CPUID, EAX = 0x01, EDX values
#ifndef bit_TSC
#define bit_TSC             (1 << 4)
#endif

// CPUID, EAX = 0x80000001, EDX values
#ifndef bit_RDTSCP
#define bit_RDTSCP          (1 << 27)
#endif

// CPUID, EAX = 0x80000007, EDX values
#ifndef bit_InvariantTSC
#define bit_InvariantTSC    (1 << 8)
#endif

#ifdef CHRONO_HAVE_X86_INTRINSICS
#ifdef _MSC_VER
static inline int __get_cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    int out[4];
    __cpuid(out, leaf);
    *eax = out[0];
    *ebx = out[1];
    *ecx = out[2];
    *edx = out[3];
    return 1;
}
#endif

// check if the processor has a TSC (Time Stamp Counter) and supports the RDTSC instruction
bool has_tsc() {
  unsigned int eax, ebx, ecx, edx;
  if (__get_cpuid(0x01, & eax, & ebx, & ecx, & edx))
    return (edx & bit_TSC) != 0;
  else
    return false;
}

// check if the processor supports RDTSCP serialising instruction
bool has_rdtscp() {
  unsigned int eax, ebx, ecx, edx;
  if (__get_cpuid(0x80000001, & eax, & ebx, & ecx, & edx))
    return (edx & bit_RDTSCP) != 0;
  else
    return false;
}

// check if the processor supports the Invariant TSC feature (constant frequency TSC)
bool has_invariant_tsc() {
  unsigned int eax, ebx, ecx, edx;
  if (__get_cpuid(0x80000007, & eax, & ebx, & ecx, & edx))
    return (edx & bit_InvariantTSC) != 0;
  else
    return false;
}
#else
#ifdef CHRONO_HAVE_TSC
bool has_tsc() { return true; }
bool has_rdtscp() { return false; }
bool has_invariant_tsc() { return true; }
#else
bool has_tsc() { return false; }
bool has_rdtscp() { return false; }
bool has_invariant_tsc() { return false; }
#endif
#endif

// Check if the RDTSC and RDTSCP instructions are allowed in user space.
// This is controlled by the x86 control register 4, bit 4 (CR4.TSD), but that is only readable by the kernel.
// On Linux, the flag can be read (and possibly set) via the prctl interface.
#ifdef __linux__
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#define _HAS_PR_TSC_ENABLE
#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#endif // __linux__

bool tsc_allowed() {
#if defined __linux__ && defined _HAS_PR_TSC_ENABLE
    int tsc_val = 0;
    prctl(PR_SET_TSC, PR_TSC_ENABLE);
    prctl(PR_GET_TSC, & tsc_val);
    return (tsc_val == PR_TSC_ENABLE);
#else
    return true;
#endif
}

#undef _HAS_PR_TSC_ENABLE

static inline void spin_for(uint32_t usec)
{
  uint32_t elapsed;
  auto startTime = std::chrono::high_resolution_clock::now();
  do {
      auto nowTime = std::chrono::high_resolution_clock::now();
      elapsed = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - startTime).count();
  } while (elapsed < usec);
}

// calibrate TSC with respect to std::chrono::high_resolution_clock
double calibrate_tsc_hz() {
  if (!has_tsc() || !tsc_allowed())
    return 0;

  constexpr unsigned int sample_size = 1000;        // 1000 samples
  constexpr unsigned int sleep_time  = 1000;        //    1 ms
  unsigned long long ticks[sample_size];
  double             times[sample_size];

  auto reference = std::chrono::high_resolution_clock::now();
  for (unsigned int i = 0; i < sample_size; ++i) {
    spin_for(sleep_time);
    ticks[i] = rdtsc();
    times[i] = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::high_resolution_clock::now() - reference ).count();
  }

  double mean_x = 0, mean_y = 0;
  for (unsigned int i = 0; i < sample_size; ++i) {
    mean_x += (double) times[i];
    mean_y += (double) ticks[i];
  }
  mean_x /= (double) sample_size;
  mean_y /= (double) sample_size;

  double sigma_xy = 0, sigma_xx = 0;
  for (unsigned int i = 0; i < sample_size; ++i) {
    sigma_xx += (double) (times[i] - mean_x) * (double) (times[i] - mean_x);
    sigma_xy += (double) (times[i] - mean_x) * (double) (ticks[i] - mean_y);
  }

  // ticks per second
  return sigma_xy / sigma_xx;
}


// new processors can use rdtscp;
uint64_t serialising_rdtscp(void)
{
#ifdef CHRONO_HAVE_RDTSCP
    unsigned int id;
    return rdtscp(& id);
#else
    return 0;
#endif
}

// older Intel processors can use lfence; rdtsc;
uint64_t serialising_rdtsc_lfence(void)
{
#ifdef CHRONO_HAVE_X86_INTRINSICS
    _mm_lfence();
#endif
    return rdtsc();
}

// older AMD processors can use mfence; rdtsc;
uint64_t serialising_rdtsc_mfence(void)
{
#ifdef CHRONO_HAVE_X86_INTRINSICS
    _mm_mfence();
#endif
    return rdtsc();
}

// very old processors do not have a TSC
uint64_t serialising_rdtsc_unimplemented(void)
{
    return 0;
}


namespace {

  static inline constexpr
  unsigned int _(const char b[4]) {
    return * (unsigned int *)(b);
  }

} // namespace

extern "C" {

  static uint64_t (*serialising_rdtsc_resolver(void))(void)
  {
    if (!tsc_allowed())
      return serialising_rdtsc_unimplemented;

    if (has_rdtscp())
      // if available, use the RDTSCP instruction
      return serialising_rdtscp;

    if (has_tsc()) {
#ifdef CHRONO_HAVE_X86_INTRINSICS
      // if the TSC is available, chck the processor vendor
      unsigned int eax, ebx, ecx, edx;
      __get_cpuid(0x00, & eax, & ebx, & ecx, & edx);
      if (ebx == _("Genu") && edx == _("ineI") && ecx == _("ntel"))
        // for Intel processors, LFENCE can be used as a serialising instruction before RDTSC
        return serialising_rdtsc_lfence;
      else if (ebx == _("Auth") && edx == _("enti") && ecx == _("cAMD"))
        // for AMD processors, MFENCE can be used as a serialising instruction before RDTSC
        return serialising_rdtsc_mfence;
      else
#endif
        // for other processors, assume that MFENCE can be used as a serialising instruction before RDTSC
        return serialising_rdtsc_mfence;
    }

    return serialising_rdtsc_unimplemented;
  }

}

// IFUNC support requires GCC >= 4.6.0 and GLIBC >= 2.11.1
#if ( defined __GNUC__ && (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) ) \
  && ( defined __GLIBC__ && (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 11) )

uint64_t serialising_rdtsc(void) __attribute__((ifunc("serialising_rdtsc_resolver"))) __attribute__((externally_visible));

#else

uint64_t (*serialising_rdtsc)(void) = serialising_rdtsc_resolver();

#endif // IFUNC support

#endif
