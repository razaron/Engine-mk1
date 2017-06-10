add_library(engine-mk1 Space.cpp Component.cpp EventStream.cpp)

set_target_properties(engine-mk1 PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
