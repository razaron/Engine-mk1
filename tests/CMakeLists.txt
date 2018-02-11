set (TEST_SOURCES main.cpp testComponent.cpp testEntity.cpp testEventStream.cpp testGraph.cpp testObjectPool.cpp testSpace.cpp testSystem.cpp testTaskScheduler.cpp)

add_executable (engine-test ${TEST_SOURCES})
set_target_properties(engine-test PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
target_link_libraries (engine-test ${REQUIRED_LIBS})
