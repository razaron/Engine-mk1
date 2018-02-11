add_library(engine-framework EventStream.cpp Planner.cpp TaskScheduler.cpp)

set_target_properties(engine-framework PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
