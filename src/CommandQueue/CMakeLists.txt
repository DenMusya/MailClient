add_library(CommandQueue INTERFACE)

target_include_directories(CommandQueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CommandQueue INTERFACE boost_project)