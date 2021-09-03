find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

add_library(chrono STATIC
	mach_absolute_time.cc
	mach_clock_get_time.cc
	native/x86_tsc_clock.cc
	posix_clock_gettime.cc
	syscall_clock_gettime.cc
	tbb_tick_count.cc
	x86_tsc.cc
	x86_tsc_clock.cc
	x86_tsc_tick.cc)

target_include_directories(chrono PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(chrono Threads::Threads OpenMP::OpenMP_CXX)
if(NOT MSVC)
	target_link_libraries(chrono m)
	target_compile_options(chrono PRIVATE -Wno-deprecated-declarations)
endif()
target_link_libraries(chrono absl::time)

# vim: set ts=4 sts=4 sw=4 noet:
