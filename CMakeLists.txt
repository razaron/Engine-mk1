cmake_minimum_required (VERSION 2.6)
project (engine-mk1)

# Setup directories
include_directories (${PROJECT_SOURCE_DIR}/3rd_party/includes ${PROJECT_SOURCE_DIR}/3rd_party/includes/lua ${PROJECT_SOURCE_DIR}/framework ${PROJECT_SOURCE_DIR}/core)

# Set default compile flags for GCC/MSVC
IF(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    MESSAGE(STATUS "Adding Unix64 compile flags")
    LINK_DIRECTORIES (${PROJECT_SOURCE_DIR}/3rd_party/libs/Unix64)
    SET(MY_COMPILE_FLAGS "-m64 -std=c++17 -Wall -Wextra" CACHE STRING "Build option. 32 or 64")
    SET(MY_LINKER_FLAGS "-m64 -std=c++17 -Wall -Wextra" CACHE STRING "Build option. 32 or 64")
ELSEIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64")
    MESSAGE(STATUS "Adding Win64 compile flags")
    LINK_DIRECTORIES (${PROJECT_SOURCE_DIR}/3rd_party/libs/Win64/${CONFIGURATION})
    SET(MY_COMPILE_FLAGS "/std:c++17 /W3" CACHE STRING "Build option. 32 or 64")
    SET(MY_LINKER_FLAGS "/std:c++17 /W3" CACHE STRING "Build option. 32 or 64")
ENDIF()

# For -lpthread
find_package (Threads)

# Create engine-framework-lib
add_subdirectory(framework)
set (REQUIRED_LIBS engine-framework ${CMAKE_THREAD_LIBS_INIT} ${REQUIRED_LIBS})

# Create engine-core-lib
add_subdirectory(core)
set (REQUIRED_LIBS engine-core ${REQUIRED_LIBS})

# Create the test exe
add_subdirectory (tests)

# Create games
# Set default compile flags for GCC/MSVC
IF(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    find_package(Lua)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
    find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)
    set (REQUIRED_LIBS ${LUA_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${REQUIRED_LIBS})
ELSEIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64")
    IF(${CONFIGURATION} STREQUAL "Debug")
        set (REQUIRED_LIBS sfml-system-d sfml-window-d sfml-graphics-d lua ${REQUIRED_LIBS})
    ELSE()
        set (REQUIRED_LIBS sfml-system sfml-window sfml-graphics lua ${REQUIRED_LIBS})
    ENDIF()
ENDIF()

#add_subdirectory(game/phase_1)
#add_subdirectory(game/phase_2)
