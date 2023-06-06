add_library(src INTERFACE blockchain.h block.h)
target_include_directories(src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(src INTERFACE blockchain.h block.h)
