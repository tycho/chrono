cmake_minimum_required(VERSION 3.21)

project(chrono CXX)

set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(abseil EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(test)

# vim: set ts=4 sts=4 sw=4 noet:
