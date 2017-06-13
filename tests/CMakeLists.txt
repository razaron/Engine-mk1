set (TEST_SOURCES main.cpp ${TEST_SOURCES})

add_executable (${TEST_NAME} ${TEST_SOURCES})
set_target_properties(${TEST_NAME} PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (${TEST_NAME} ${REQUIRED_LIBS})
