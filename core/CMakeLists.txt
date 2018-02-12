add_library(engine-core Space.cpp System.cpp)

set_target_properties(engine-core PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
