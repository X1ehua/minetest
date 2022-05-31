add_library(bitop STATIC bit.c)
target_link_libraries(bitop)

include_directories(${LUA_INCLUDE_DIR})

set_target_properties(bitop PROPERTIES FOLDER 3rd)
