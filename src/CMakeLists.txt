# include GLM
add_library(engine-mk1-lib Space.cpp Component.cpp EventStream.cpp PhysicsSystem.cpp)

set_target_properties(engine-mk1-lib PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
