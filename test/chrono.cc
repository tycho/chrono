// macro to check for GCC version
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// C++ headers
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <chrono>

#ifdef HAVE_BOOST_CHRONO
// boost headers
#include <boost/chrono.hpp>
#endif // HAVE_BOOST_CHRONO

#if defined HAVE_BOOST_CHRONO || defined HAVE_BOOST_TIMER
// boost version
#include <boost/version.hpp>
#endif // defined HAVE_BOOST_CHRONO || defined HAVE_BOOST_TIMER

#ifdef HAVE_TBB
// TBB version
#include <tbb/tbb_stddef.h>
#endif // HAVE_TBB

// for uname
#ifndef _WIN32
#include <sys/utsname.h>
#else
#include <windows.h>
#endif

#ifdef __linux__
// for gnu_get_libc_version
#include <gnu/libc-version.h>
#endif // __linux__

// other clocks
#include "interface/absl_time.h"
#include "interface/syscall_clock_gettime.h"
#include "interface/posix_clock.h"
#include "interface/posix_clock_gettime.h"
#include "interface/posix_gettimeofday.h"
#include "interface/posix_times.h"
#include "interface/posix_times_f.h"
#include "interface/posix_times_d.h"
#include "interface/posix_getrusage.h"
#include "interface/mach_clock_get_time.h"
#include "interface/mach_absolute_time.h"
#include "interface/mach_thread_info.h"
#include "interface/x86_tsc_clock.h"
#include "interface/boost_timer.h"
#include "interface/tbb_tick_count.h"
#include "interface/omp_get_wtime.h"

#include "interface/native/mach_absolute_time.h"
#include "interface/native/x86_tsc_clock.h"

#include "benchmark.h"


void init_timers(std::vector<BenchmarkBase *> & timers) 
{
  // std::chrono timers
  timers.push_back(new Benchmark<std::chrono::steady_clock>("std::chrono::steady_clock"));
  timers.push_back(new Benchmark<std::chrono::system_clock>("std::chrono::system_clock"));
  timers.push_back(new Benchmark<std::chrono::high_resolution_clock>("std::chrono::high_resolution_clock"));

  // syscall clock_gettime
#ifdef HAVE_SYSCALL_CLOCK_REALTIME
  if (clock_syscall_realtime::is_available)
    timers.push_back(new Benchmark<clock_syscall_realtime>("syscall(SYS_clock_gettime, CLOCK_REALTIME)"));
#endif // HAVE_SYSCALL_CLOCK_REALTIME
#ifdef HAVE_SYSCALL_CLOCK_REALTIME_COARSE
  if (clock_syscall_realtime_coarse::is_available)
    timers.push_back(new Benchmark<clock_syscall_realtime_coarse>("syscall(SYS_clock_gettime, CLOCK_REALTIME_COARSE)"));
#endif // HAVE_SYSCALL_CLOCK_REALTIME_COARSE
#ifdef HAVE_SYSCALL_CLOCK_MONOTONIC
  if (clock_syscall_monotonic::is_available)
    timers.push_back(new Benchmark<clock_syscall_monotonic>("syscall(SYS_clock_gettime, CLOCK_MONOTONIC)"));
#endif // HAVE_SYSCALL_CLOCK_MONOTONIC
#ifdef HAVE_SYSCALL_CLOCK_MONOTONIC_COARSE
  if (clock_syscall_monotonic_coarse::is_available)
    timers.push_back(new Benchmark<clock_syscall_monotonic_coarse>("syscall(SYS_clock_gettime, CLOCK_MONOTONIC_COARSE)"));
#endif // HAVE_SYSCALL_CLOCK_MONOTONIC_COARSE
#ifdef HAVE_SYSCALL_CLOCK_MONOTONIC_RAW
  if (clock_syscall_monotonic_raw::is_available)
    timers.push_back(new Benchmark<clock_syscall_monotonic_raw>("syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW)"));
#endif // HAVE_SYSCALL_CLOCK_MONOTONIC_RAW
#ifdef HAVE_SYSCALL_CLOCK_BOOTTIME
  if (clock_syscall_boottime::is_available)
    timers.push_back(new Benchmark<clock_syscall_boottime>("syscall(SYS_clock_gettime, CLOCK_BOOTTIME)"));
#endif // HAVE_SYSCALL_CLOCK_BOOTTIME
#ifdef HAVE_SYSCALL_CLOCK_PROCESS_CPUTIME_ID
  if (clock_syscall_process_cputime::is_available)
    timers.push_back(new Benchmark<clock_syscall_process_cputime>("syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID)"));
#endif // HAVE_SYSCALL_CLOCK_PROCESS_CPUTIME_ID
#ifdef HAVE_SYSCALL_CLOCK_THREAD_CPUTIME_ID
  if (clock_syscall_thread_cputime::is_available)
    timers.push_back(new Benchmark<clock_syscall_thread_cputime>("syscall(SYS_clock_gettime, CLOCK_THREAD_CPUTIME_ID)"));
#endif // HAVE_SYSCALL_CLOCK_THREAD_CPUTIME_ID

  // POSIX clock_gettime
#ifdef HAVE_POSIX_CLOCK_REALTIME
  if (clock_gettime_realtime::is_available)
    timers.push_back(new Benchmark<clock_gettime_realtime>("clock_gettime(CLOCK_REALTIME)"));
#endif // HAVE_POSIX_CLOCK_REALTIME
#ifdef HAVE_POSIX_CLOCK_REALTIME_COARSE
  if (clock_gettime_realtime_coarse::is_available)
    timers.push_back(new Benchmark<clock_gettime_realtime_coarse>("clock_gettime(CLOCK_REALTIME_COARSE)"));
#endif // HAVE_POSIX_CLOCK_REALTIME_COARSE
#ifdef HAVE_POSIX_CLOCK_MONOTONIC
  if (clock_gettime_monotonic::is_available)
    timers.push_back(new Benchmark<clock_gettime_monotonic>("clock_gettime(CLOCK_MONOTONIC)"));
#endif // HAVE_POSIX_CLOCK_MONOTONIC
#ifdef HAVE_POSIX_CLOCK_MONOTONIC_COARSE
  if (clock_gettime_monotonic_coarse::is_available)
    timers.push_back(new Benchmark<clock_gettime_monotonic_coarse>("clock_gettime(CLOCK_MONOTONIC_COARSE)"));
#endif // HAVE_POSIX_CLOCK_MONOTONIC_COARSE
#ifdef HAVE_POSIX_CLOCK_MONOTONIC_RAW
  if (clock_gettime_monotonic_raw::is_available)
    timers.push_back(new Benchmark<clock_gettime_monotonic_raw>("clock_gettime(CLOCK_MONOTONIC_RAW)"));
#endif // HAVE_POSIX_CLOCK_MONOTONIC_RAW
#ifdef HAVE_POSIX_CLOCK_BOOTTIME
  if (clock_gettime_boottime::is_available)
    timers.push_back(new Benchmark<clock_gettime_boottime>("clock_gettime(CLOCK_BOOTTIME)"));
#endif // HAVE_POSIX_CLOCK_BOOTTIME
#ifdef HAVE_POSIX_CLOCK_PROCESS_CPUTIME_ID
  if (clock_gettime_process_cputime::is_available)
    timers.push_back(new Benchmark<clock_gettime_process_cputime>("clock_gettime(CLOCK_PROCESS_CPUTIME_ID)"));
#endif // HAVE_POSIX_CLOCK_PROCESS_CPUTIME_ID
#ifdef HAVE_POSIX_CLOCK_THREAD_CPUTIME_ID
  if (clock_gettime_thread_cputime::is_available)
    timers.push_back(new Benchmark<clock_gettime_thread_cputime>("clock_gettime(CLOCK_THREAD_CPUTIME_ID)"));
#endif // HAVE_POSIX_CLOCK_THREAD_CPUTIME_ID

  // POSIX gettimeofday
#ifdef HAVE_GETTIMEOFDAY
  timers.push_back(new Benchmark<clock_gettimeofday>("gettimeofday()"));
#endif

  // POSIX times
#if !defined(_WIN32)
  timers.push_back(new Benchmark<clock_times_realtime>("times() (wall-clock time)"));
  timers.push_back(new Benchmark<clock_times_cputime>("times() (cpu time)"));
  timers.push_back(new Benchmark<clock_times_realtime_d>("times() (wall-clock time) (using double)"));
  timers.push_back(new Benchmark<clock_times_cputime_d>("times() (cpu time) (using double)"));
#if defined(CHRONO_HAVE_TSC)
// 128-bit wide int is only available on x86
  timers.push_back(new Benchmark<clock_times_realtime_f>("times() (wall-clock time) (using fixed math)"));
  timers.push_back(new Benchmark<clock_times_cputime_f>("times() (cpu time) (using fixed math)"));
#endif // defined(CHRONO_HAVE_TSC)
#endif

  // abseil time
  timers.push_back(new Benchmark<absl_time>("abseil GetCurrentTimeNanos"));

  // POSIX clock
  timers.push_back(new Benchmark<clock_clock>("clock()"));

  // POSIX getrusage
#ifdef HAVE_GETRUSAGE
  timers.push_back(new Benchmark<clock_getrusage_self>("getrusage(RUSAGE_SELF)"));
#ifdef HAVE_POSIX_CLOCK_GETRUSAGE_THREAD
  timers.push_back(new Benchmark<clock_getrusage_thread>("getrusage(RUSAGE_THREAD)"));
#endif // HAVE_POSIX_CLOCK_GETRUSAGE_THREAD
#endif

  // MACH clock_get_time
#ifdef HAVE_MACH_SYSTEM_CLOCK
  if (mach_system_clock::is_available)
    timers.push_back(new Benchmark<mach_system_clock>("host_get_clock_service(SYSTEM_CLOCK), clock_get_time(...)"));
#endif // HAVE_MACH_SYSTEM_CLOCK
#ifdef HAVE_MACH_REALTIME_CLOCK
  if (mach_realtime_clock::is_available)
    timers.push_back(new Benchmark<mach_realtime_clock>("host_get_clock_service(REALTIME_CLOCK), clock_get_time(...)"));
#endif // HAVE_MACH_REALTIME_CLOCK
#ifdef HAVE_MACH_CALENDAR_CLOCK
  if (mach_calendar_clock::is_available)
    timers.push_back(new Benchmark<mach_calendar_clock>("host_get_clock_service(CALENDAR_CLOCK), clock_get_time(...)"));
#endif // HAVE_MACH_CALENDAR_CLOCK
#ifdef HAVE_MACH_ABSOLUTE_TIME
  if (mach_absolute_time_clock::is_available) {
    timers.push_back(new Benchmark<mach_absolute_time_clock>("mach_absolute_time() (using nanoseconds)"));
    timers.push_back(new Benchmark<native::mach_absolute_time_clock>("mach_absolute_time() (native)"));
  }
#endif // HAVE_MACH_ABSOLUTE_TIME
#ifdef HAVE_MACH_THREAD_INFO_CLOCK
  if (mach_thread_info_clock::is_available)
    timers.push_back(new Benchmark<mach_thread_info_clock>("thread_info(mach_thread_self(), THREAD_BASIC_INFO, ...)"));
#endif // HAVE_MACH_THREAD_INFO_CLOCK

#if defined(CHRONO_HAVE_TSC)
// TSC is only available on x86
  
  // read TSC clock frequency
  std::stringstream buffer;
  buffer << std::fixed << std::setprecision(3) << (tsc_tick::ticks_per_second / 1.e6) << " MHz";
  std::string tsc_freq = buffer.str();

  // x86 DST-based clock (via std::chrono::nanosecond)
  if (clock_rdtsc::is_available)
    timers.push_back(new Benchmark<clock_rdtsc>("RDTSC (" + tsc_freq + ") (using nanoseconds)"));
  if (clock_rdtsc_lfence::is_available)
    timers.push_back(new Benchmark<clock_rdtsc_lfence>("LFENCE; RDTSC (" + tsc_freq + ") (using nanoseconds)"));
  if (clock_rdtsc_mfence::is_available)
    timers.push_back(new Benchmark<clock_rdtsc_mfence>("MFENCE; RDTSC (" + tsc_freq + ") (using nanoseconds)"));
#ifdef CHRONO_HAVE_RDTSCP
  if (clock_rdtscp::is_available)
    timers.push_back(new Benchmark<clock_rdtscp>("RDTSCP (" + tsc_freq + ") (using nanoseconds)"));
#endif
  if (clock_serialising_rdtsc::is_available)
    timers.push_back(new Benchmark<clock_serialising_rdtsc>("run-time selected serialising RDTSC (" + tsc_freq + ") (using nanoseconds)"));
  // x86 DST-based clock (native)
  if (native::clock_rdtsc::is_available)
    timers.push_back(new Benchmark<native::clock_rdtsc>("RDTSC (" + tsc_freq + ") (native)"));
  if (native::clock_rdtsc_lfence::is_available)
    timers.push_back(new Benchmark<native::clock_rdtsc_lfence>("LFENCE; RDTSC (" + tsc_freq + ") (native)"));
  if (native::clock_rdtsc_mfence::is_available)
    timers.push_back(new Benchmark<native::clock_rdtsc_mfence>("MFENCE; RDTSC (" + tsc_freq + ") (native)"));
#ifdef CHRONO_HAVE_RDTSCP
  if (native::clock_rdtscp::is_available)
    timers.push_back(new Benchmark<native::clock_rdtscp>("RDTSCP (" + tsc_freq + ") (native)"));
#endif
  if (native::clock_serialising_rdtsc::is_available)
    timers.push_back(new Benchmark<native::clock_serialising_rdtsc>("run-time selected serialising RDTSC (" + tsc_freq + ") (native)"));

#endif // defined(CHRONO_HAVE_TSC)

#ifdef HAVE_BOOST_TIMER
  // boost timer clocks
  timers.push_back(new Benchmark<clock_boost_timer_realtime>("boost::timer (wall-clock time)"));
  timers.push_back(new Benchmark<clock_boost_timer_cputime>("boost::timer (cpu time)"));
#endif // HAVE_BOOST_TIMER

#ifdef HAVE_BOOST_CHRONO
  // boost chrono clocks
  timers.push_back(new Benchmark<boost::chrono::steady_clock>("boost::chrono::steady_clock"));
  timers.push_back(new Benchmark<boost::chrono::system_clock>("boost::chrono::system_clock"));
  timers.push_back(new Benchmark<boost::chrono::high_resolution_clock>("boost::chrono::high_resolution_clock"));
  #ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
  timers.push_back(new Benchmark<boost::chrono::process_real_cpu_clock>("boost::chrono::process_real_cpu_clock"));
  timers.push_back(new Benchmark<boost::chrono::process_user_cpu_clock>("boost::chrono::process_user_cpu_clock"));
  timers.push_back(new Benchmark<boost::chrono::process_system_cpu_clock>("boost::chrono::process_system_cpu_clock"));
  //timers.push_back(new Benchmark<boost::chrono::process_cpu_clock>("boost::chrono::process_cpu_clock"));
  #endif // BOOST_CHRONO_HAS_PROCESS_CLOCKS
  #ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
  timers.push_back(new Benchmark<boost::chrono::thread_clock>("boost::chrono::thread_clock"));
  #endif // BOOST_CHRONO_HAS_THREAD_CLOCK
#endif // HAVE_BOOST_CHRONO

#ifdef HAVE_TBB
  // TBB tick_count (this interface does not expose the underlying type, so it cannot easily be used to build a "native" clock interface)
  timers.push_back(new Benchmark<clock_tbb_tick_count>("tbb::tick_count"));
#endif // HAVE_TBB

  // OpenMP timer
  timers.push_back(new Benchmark<clock_omp_get_wtime>("omp_get_wtime"));
}


std::string read_kernel_version() {
#if !defined(_WIN32)
  struct utsname names;
  if (not uname(& names)) {
    std::stringstream buffer;
    buffer << names.sysname << " " << names.release << " " << names.machine;
    return buffer.str();
  } else {
    return std::string("unknown");
  }
#else
    OSVERSIONINFOEXA ver;
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    GetVersionExA((LPOSVERSIONINFOA)&ver);
    std::stringstream buffer;
    buffer << "Windows " << ver.dwMajorVersion << "." << ver.dwMinorVersion << " build " << ver.dwBuildNumber;
    return buffer.str();
#endif
}


#ifdef __linux__
std::string read_glibc_version() {
  std::string version( gnu_get_libc_version() );
  return version;
}


std::string read_clock_source() {
  std::ifstream current_clocksource("/sys/devices/system/clocksource/clocksource0/current_clocksource");
  if (current_clocksource.good()) {
    std::string value;
    current_clocksource >> value;
    return value;
  } else {
    return std::string("unknown");
  }
}
#endif // __linux__


int main(void) {
  std::vector<BenchmarkBase *> timers;
  init_timers(timers);

  std::cout << read_kernel_version() << std::endl;
#ifdef __linux__
  std::cout << "glibc version: " << read_glibc_version() << std::endl;
  std::cout << "clock source:  " << read_clock_source() << std::endl;
#endif // __linux__
#if defined HAVE_BOOST_CHRONO || defined HAVE_BOOST_TIMER
  std::cout << "boost version: " << (BOOST_VERSION / 100000) << '.' << (BOOST_VERSION / 100 % 1000) << '.' << (BOOST_VERSION % 100) << std::endl;
#endif // defined HAVE_BOOST_CHRONO || defined HAVE_BOOST_TIMER
#ifdef HAVE_TBB
  std::cout << "tbb version:   " << (TBB_INTERFACE_VERSION / 1000) << '.' << (TBB_INTERFACE_VERSION % 1000) << " (interface) " 
            << (tbb::TBB_runtime_interface_version() / 1000) << '.' << (tbb::TBB_runtime_interface_version() % 1000) << " (runtime)" << std::endl;
#endif // HAVE_TBB

  std::cout << "For each timer the resolution reported is the MINIMUM (MEDIAN) (MEAN +/- its STDDEV) of the increments measured during the test." << std::endl << std::endl; 

  for (BenchmarkBase * timer: timers) {
    timer->measure();
    timer->compute();
    timer->report();
  }

  return 0;
}
