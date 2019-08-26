# Setup directories
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/includes ${CMAKE_CURRENT_SOURCE_DIR}/framework ${CMAKE_CURRENT_SOURCE_DIR}/core)

# For -lpthread
find_package (Threads)

# for -llua
IF(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    find_package(Lua)
    set (REQUIRED_LIBS ${LUA_LIBRARIES} ${REQUIRED_LIBS})
ELSEIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 16 2019")
    set (REQUIRED_LIBS lua ${REQUIRED_LIBS})
ENDIF()

# Create engine-framework-lib
add_subdirectory(framework)
set (REQUIRED_LIBS engine-framework ${CMAKE_THREAD_LIBS_INIT} ${REQUIRED_LIBS})

# Create engine-core-lib
add_subdirectory(core)
set (REQUIRED_LIBS engine-core ${REQUIRED_LIBS})

# Create the test exe
add_subdirectory (tests)