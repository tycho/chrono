#ifndef x86_tsc_h
#define x86_tsc_h

#include <cstdint>

#if defined(__x86_64__) || defined(__i386__) || defined(_M_IX86) || defined(_M_AMD64) || (defined(_WIN32) && defined(_M_ARM64))
#define CHRONO_HAVE_TSC
#define CHRONO_HAVE_RDTSCP
#endif

#if defined(__x86_64__) || defined(__i386__) || defined(_M_IX86) || defined(_M_AMD64)
#define CHRONO_HAVE_X86_INTRINSICS
#endif

#ifdef CHRONO_HAVE_TSC
#if defined __GNUC__
// GCC and ICC provide intrinsics for rdtsc and rdtscp
#include <x86intrin.h>

extern inline uint64_t rdtsc(void)
{
#if __has_builtin(__builtin_readcyclecounter)
    return __builtin_readcyclecounter();
#else
    return __rdtsc();
#endif
}

extern inline uint64_t rdtscp(uint32_t *aux)
{
    return __rdtscp(aux);
}
#elif defined(_MSC_VER)
#include <intrin.h>

#ifdef _M_ARM64
#pragma intrinsic(_ReadStatusReg)

#ifndef ARM64_CNTVCT
#define ARM64_CNTVCT            ARM64_SYSREG(3,3,14, 0,2)  // Generic Timer counter register
#endif
#ifndef ARM64_PMCCNTR_EL0
#define ARM64_PMCCNTR_EL0       ARM64_SYSREG(3,3, 9,13,0)  // Cycle Count Register [CP15_PMCCNTR]
#endif

extern inline uint64_t rdtsc(void)
{
    return _ReadStatusReg( ARM64_CNTVCT );
}

#undef CHRONO_HAVE_RDTSCP
#else
#pragma intrinsic(__rdtsc)
#pragma intrinsic(__rdtscp)

extern inline uint64_t rdtsc(void)
{
    return __rdtsc();
}

extern inline uint64_t rdtscp(uint32_t *aux)
{
    return __rdtscp(aux);
}
#endif
#else
#  error "Unsupported compiler"
#endif // __clang__ / __GNUC__

bool has_tsc();
bool has_rdtscp();
bool has_invariant_tsc();
bool tsc_allowed();

double calibrate_tsc_hz();


// IFUNC support requires GCC >= 4.6.0 and GLIBC >= 2.11.1
#if ( defined __GNUC__ && (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) ) \
  && ( defined __GLIBC__ && (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 11) )

// processor specific serialising access to the TSC
extern uint64_t serialising_rdtsc(void);

#else

// processor specific serialising access to the TSC
extern uint64_t (*serialising_rdtsc)(void);

#endif // IFUNC support
#endif

#endif // x86_tsc_h
