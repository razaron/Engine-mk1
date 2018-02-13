file(GLOB CORE_SOURCES "*.cpp")
file(GLOB CORE_HEADERS "*.hpp")

add_library(engine-core ${CORE_SOURCES} ${CORE_HEADERS})

set_target_properties(engine-core PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
