cmake_minimum_required (VERSION 2.6)
project (engine-mk1)

SET(ENVIRONMENT "32" CACHE STRING "Build option. 32 or 64")

# set default executable names
set (TEST_EXE tests_32)

# set default compile flags
set (MY_COMPILE_FLAGS "-Wall -Wextra -std=c++1z")

# set default linker flags
set (MY_LINKER_FLAGS "-Wall -Wextra -std=c++1z")


# set ENVIRONMENT specific variables
if("${ENVIRONMENT}" EQUAL "32")
    set (MY_COMPILE_FLAGS "${MY_COMPILE_FLAGS} -m32")
    set (MY_LINKER_FLAGS "${MY_LINKER_FLAGS} -m32")

elseif("${ENVIRONMENT}" EQUAL "64")
    set (TEST_EXE tests_64)

else()
    message( FATAL_ERROR "ENVIRONMENT must equal 32 or 64." )
endif()

# include src and create engine-mk1-lib
include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)

# connect all libs
set (EXTRA_LIBS ${EXTRA_LIBS} engine-mk1-lib)

# connect all sources for the test suite
set (TEST_SOURCES ${TEST_SOURCES} tests/main.cpp tests/testComponent.cpp tests/testObjectPool.cpp tests/testGraph.cpp tests/testEventStream.cpp)

add_executable (${TEST_EXE} ${TEST_SOURCES})
set_target_properties(${TEST_EXE} PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (${TEST_EXE} ${EXTRA_LIBS})
