file(GLOB FRAMEWORK_SOURCES "*.cpp")
file(GLOB FRAMEWORK_HEADERS "*.hpp")

add_library(engine-framework ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS})

set_target_properties(engine-framework PROPERTIES COMPILE_FLAGS ${MY_COMPILE_FLAGS} LINK_FLAGS ${MY_LINKER_FLAGS})
