set (GAME_V1_SOURCES main.cpp InputSystem.cpp RenderSystem.cpp PhysicsSystem.cpp TransformComponent.cpp)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.4 REQUIRED system window graphics network audio)

# Create and setup executable
add_executable (game_v1 ${GAME_V1_SOURCES})
set_target_properties(game_v1 PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (game_v1 ${REQUIRED_LIBS} ${SFML_LIBRARIES})
