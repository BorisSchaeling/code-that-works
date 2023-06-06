add_library(src INTERFACE user.h blockchain.h block.h)
target_include_directories(src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${BOTAN_INCLUDE_DIR})
target_sources(src INTERFACE user.h blockchain.h block.h)
target_link_libraries(src INTERFACE ${BOTAN_LIBRARY})
