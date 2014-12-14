# Minimum version required
cmake_minimum_required(VERSION 2.6)
# Project name
project(main)

# Wall
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# Make sure build.sh is called!

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

# Default flags
add_definitions(-D UNIX=1)

file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")
file(GLOB c_headers "*.h")

# Add include directories
include_directories(".")

# Create executable
add_executable(unknown ${sources} ${headers} ${c_headers})

# Link together everything
target_link_libraries(unknown)
