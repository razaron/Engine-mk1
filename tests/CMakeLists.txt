file(GLOB TEST_SOURCES "*.cpp")
file(GLOB TEST_HEADERS "*.hpp")

add_executable (engine-test ${TEST_SOURCES} ${TEST_HEADERS})
set_target_properties(engine-test PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (engine-test ${REQUIRED_LIBS})
