#ifndef native_x86_tsc_clock_h
#define native_x86_tsc_clock_h

// C++ standard headers
#include <chrono>

#include "interface/x86_tsc.h"
#include "interface/x86_tsc_tick.h"
#include "interface/native/native.h"

namespace native {

  // TSC-based clock with native duration, using rdtsc (non-serialising)
  struct clock_rdtsc
  {
    // std::chrono-like native interface
    typedef native_duration<int64_t, tsc_tick>                          duration;
    typedef duration::rep                                               rep;
    typedef duration::period                                            period;
    typedef native_time_point<clock_rdtsc, duration>                    time_point;

    static const bool is_steady;
    static const bool is_available;

    static time_point now() noexcept
    {
      rep        ticks = rdtsc();
      duration   d(ticks);
      time_point t(d);
      return t;
    }
  };


  // TSC-based clock with native duration, using lfence as serialising instruction
  struct clock_rdtsc_lfence
  {
    // std::chrono-like native interface
    typedef native_duration<int64_t, tsc_tick>                          duration;
    typedef duration::rep                                               rep;
    typedef duration::period                                            period;
    typedef native_time_point<clock_rdtsc_lfence, duration>             time_point;

    static const bool is_steady;
    static const bool is_available;

    static time_point now() noexcept
    {
#ifdef CHRONO_HAVE_X86_INTRINSICS
      _mm_lfence();
#elif defined(_M_ARM64)
      __dmb(_ARM64_BARRIER_SY);
#endif
      rep        ticks = rdtsc();
      duration   d(ticks);
      time_point t(d);
      return t;
    }
  };


  // TSC-based clock with native duration, using mfence as serialising instruction
  struct clock_rdtsc_mfence
  {
    // std::chrono-like native interface
    typedef native_duration<int64_t, tsc_tick>                          duration;
    typedef duration::rep                                               rep;
    typedef duration::period                                            period;
    typedef native_time_point<clock_rdtsc_mfence, duration>             time_point;

    static const bool is_steady;
    static const bool is_available;

    static time_point now() noexcept
    {
#ifdef CHRONO_HAVE_X86_INTRINSICS
      _mm_mfence();
#elif defined(_M_ARM64)
      __dmb(_ARM64_BARRIER_SY);
#endif
      rep        ticks = rdtsc();
      duration   d(ticks);
      time_point t(d);
      return t;
    }
  };


#ifdef CHRONO_HAVE_RDTSCP
  // TSC-based clock with native duration, using rdtscp as serialising instruction
  struct clock_rdtscp
  {
    // std::chrono-like native interface
    typedef native_duration<int64_t, tsc_tick>                          duration;
    typedef duration::rep                                               rep;
    typedef duration::period                                            period;
    typedef native_time_point<clock_rdtscp, duration>                   time_point;

    static const bool is_steady;
    static const bool is_available;

    static time_point now() noexcept
    {
      unsigned int id;
      rep        ticks = rdtscp(& id);
      duration   d(ticks);
      time_point t(d);
      return t;
    }
  };
#endif


  // TSC-based clock, determining at run-time the best strategy to serialise the reads from the TSC
  struct clock_serialising_rdtsc
  {
    // std::chrono-like native interface
    typedef native_duration<int64_t, tsc_tick>                          duration;
    typedef duration::rep                                               rep;
    typedef duration::period                                            period;
    typedef native_time_point<clock_serialising_rdtsc, duration>        time_point;

    static const bool is_steady;
    static const bool is_available;

    static time_point now() noexcept
    {
      rep        ticks = serialising_rdtsc();
      duration   d(ticks);
      time_point t(d);
      return t;
    }
  };


} // namespace native

#endif // native_x86_tsc_clock_h
