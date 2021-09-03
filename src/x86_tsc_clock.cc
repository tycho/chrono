#include "interface/x86_tsc.h"
#include "interface/x86_tsc_clock.h"

const bool clock_rdtsc::is_available                = has_tsc() && tsc_allowed();
const bool clock_rdtsc::is_steady                   = has_invariant_tsc();

const bool clock_rdtsc_lfence::is_available         = has_tsc() && tsc_allowed();
const bool clock_rdtsc_lfence::is_steady            = has_invariant_tsc();

const bool clock_rdtsc_mfence::is_available         = has_tsc() && tsc_allowed();
const bool clock_rdtsc_mfence::is_steady            = has_invariant_tsc();

#ifdef CHRONO_HAVE_RDTSCP
const bool clock_rdtscp::is_available               = has_rdtscp() && tsc_allowed();
const bool clock_rdtscp::is_steady                  = has_invariant_tsc();
#endif

const bool clock_serialising_rdtsc::is_available    = has_tsc() && tsc_allowed();
const bool clock_serialising_rdtsc::is_steady       = has_invariant_tsc();
