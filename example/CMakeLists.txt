cmake_minimum_required(VERSION 2.8)

project(example)

set(CXX_DEFAULT_FLAGS "-Wall -Wextra -Weffc++ -pedantic -std=c++17")

if(CMAKE_COMPILER_IS_GNUCXX)                          # using GCC
	set(OPTIMIZE_FLAGS "-march=native")
	set(DEBUG_FLAGS "-fsanitize=address,undefined,integer-divide-by-zero,null,signed-integer-overflow")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")  # using Clang
	set(OPTIMIZE_FLAGS "-march=native")
	set(DEBUG_FLAGS "-fsanitize=address,undefined,bounds,bool,integer,integer-divide-by-zero,null")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")  # using Intel C++
	set(OPTIMIZE_FLAGS "-xhost")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")   # using Visual Studio C++

endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_DEFAULT_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OPTIMIZE_FLAGS}")

# file(GLOB_RECURSE SRC_FILES RELATIVE_PATH ${PROJECT_SOURCE_DIR}/*.cpp)
set(SOURCES_FILES
	00-readme.cpp
	01-basic.cpp)

string( REPLACE ".cpp" "" EXE_FILES "${SOURCES_FILES}" )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/..)

foreach( name ${EXE_FILES} )
    add_executable( ${name} ${name}.cpp )

endforeach()


## C++14 is needed
# set_property(TARGET ${EXE_FILES} PROPERTY CXX_STANDARD 14)
# set_property(TARGET ${EXE_FILES} PROPERTY CXX_STANDARD_REQUIRED ON)
