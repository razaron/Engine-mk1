set (GAME_SOURCES main.cpp InputSystem.cpp RenderSystem.cpp PhysicsSystem.cpp GameSystem.cpp)
set (GAME_SOURCES ${GAME_SOURCES} TransformComponent.cpp MotionComponent.cpp ShapeComponent.cpp AnimalComponent.cpp)
# Detect and add SFML
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package (SFML 2.4 REQUIRED system window graphics network audio)

# Create and setup executable
add_executable (game ${GAME_SOURCES})
set_target_properties(game PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (game ${REQUIRED_LIBS} ${SFML_LIBRARIES})
