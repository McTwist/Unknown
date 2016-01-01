# Minimum version required
cmake_minimum_required(VERSION 2.6)
# Project name
project(Unknown)

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
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-D UNIX=1)
	# Note: Somehow doesn't work correctly
	#set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j 4")
endif()

file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")
file(GLOB c_headers "*.h")

# Add include directories
include_directories(".")

# Create executable
add_executable(unknown ${sources} ${headers} ${c_headers})

# Link together everything
target_link_libraries(unknown)
