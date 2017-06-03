cmake_minimum_required (VERSION 2.6)
project (engine-mk1)

SET(MY_COMPILE_FLAGS "fail" CACHE STRING "Build option. 32 or 64")
SET(MY_LINKER_FLAGS "fail" CACHE STRING "Build option. 32 or 64")
SET(MY_INCLUDES "fail" CACHE STRING "Build option. 32 or 64")
SET(TEST_NAME "fail" CACHE STRING "Build option. 32 or 64")

# Setup includes
include_directories (${MY_INCLUDES} ${PROJECT_SOURCE_DIR}/src)
add_subdirectory (src)

# connect all libs
set (EXTRA_LIBS ${EXTRA_LIBS} engine-mk1-lib)

# connect all sources for the test suite
set (TEST_SOURCES ${TEST_SOURCES} tests/main.cpp tests/testComponent.cpp tests/testObjectPool.cpp tests/testGraph.cpp tests/testEventStream.cpp)

add_executable (${TEST_NAME} ${TEST_SOURCES})
set_target_properties(${TEST_NAME} PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (${TEST_NAME} ${EXTRA_LIBS})
