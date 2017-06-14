cmake_minimum_required (VERSION 2.6)
project (engine-mk1)

SET(MY_COMPILE_FLAGS "NULL" CACHE STRING "Build option. 32 or 64")
SET(MY_LINKER_FLAGS "NULL" CACHE STRING "Build option. 32 or 64")
SET(MY_INCLUDES "NULL" CACHE STRING "Build option. 32 or 64")

SET(TEST_NAME "NULL" CACHE STRING "Build option. 32 or 64")
SET(TEST_SOURCES "NULL" CACHE STRING "Build option. 32 or 64")

# Setup includes
include_directories (${MY_INCLUDES} ${PROJECT_SOURCE_DIR}/src)

# For -lpthread
find_package (Threads)

# Create engine-mk1-lib
add_subdirectory (src)

# connect all libs
set (REQUIRED_LIBS ${REQUIRED_LIBS} engine-mk1 ${CMAKE_THREAD_LIBS_INIT})

# Create the test exe
add_subdirectory (tests)
