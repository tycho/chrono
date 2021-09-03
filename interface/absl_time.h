#ifndef absl_time_h
#define absl_time_h

#include <chrono>
#include <absl/time/clock.h>

struct absl_time
{
	using duration   = std::chrono::nanoseconds;
	using rep        = duration::rep;
	using period     = duration::period;
	using time_point = std::chrono::time_point<absl_time>;
	static const bool is_steady = true;

	static time_point now() noexcept
	{
		rep now = absl::GetCurrentTimeNanos();
		duration period(now);
		time_point t(period);
		return t;
	}
};

#endif // absl_time_h
