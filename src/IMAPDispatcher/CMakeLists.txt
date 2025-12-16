add_library(IMAPDispatcher IMAPDispatcher.cpp)

target_include_directories(IMAPDispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(IMAPDispatcher boost_project)