cmake_minimum_required(VERSION 3.10)
project(water_distribution_assignment)

include_directories(include)

# Add the executable
add_executable(water_distribution water_distribution.cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)



