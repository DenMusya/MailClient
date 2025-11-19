add_library(Logger Logger.cpp)

target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Logger boost_project)