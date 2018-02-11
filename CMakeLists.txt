CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (engine-mk1)

# Setup directories
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/3rd_party/includes ${PROJECT_SOURCE_DIR}/3rd_party/includes/lua ${PROJECT_SOURCE_DIR}/framework ${PROJECT_SOURCE_DIR}/core)

# Set default compile flags for GCC/MSVC
if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    MESSAGE(STATUS "Adding Unix64 compile flags")
    LINK_DIRECTORIES (${PROJECT_SOURCE_DIR}/3rd_party/libs/Unix64)
    SET(MY_COMPILE_FLAGS "-m64 -std=c++17 -Wall -Wextra" CACHE STRING "Build option. 32 or 64")
    SET(MY_LINKER_FLAGS "-m64 -std=c++17 -Wall -Wextra" CACHE STRING "Build option. 32 or 64")
elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64")
    MESSAGE(STATUS "Adding Win64 compile flags")
    LINK_DIRECTORIES (${PROJECT_SOURCE_DIR}/3rd_party/libs/Win64)
    SET(MY_COMPILE_FLAGS "/std:c++17 /W3" CACHE STRING "Build option. 32 or 64")
    SET(MY_LINKER_FLAGS "/std:c++17 /W3" CACHE STRING "Build option. 32 or 64")
endif()

# For -lpthread
FIND_PACKAGE (Threads)

# Create engine-framework-lib
ADD_SUBDIRECTORY(framework)
SET (REQUIRED_LIBS ${REQUIRED_LIBS} engine-framework ${CMAKE_THREAD_LIBS_INIT})

# Create engine-core-lib
ADD_SUBDIRECTORY(core)
SET (REQUIRED_LIBS ${REQUIRED_LIBS} engine-core)

# Create the test exe
ADD_SUBDIRECTORY (tests)

# Create games
SET (REQUIRED_LIBS ${REQUIRED_LIBS} sfml-system-d sfml-window-d sfml-graphics-d lua)

ADD_SUBDIRECTORY(game/phase_1)
