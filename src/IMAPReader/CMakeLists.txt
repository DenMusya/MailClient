add_library(IMAPReader IMAPReader.cpp)

target_include_directories(IMAPReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(IMAPReader boost_project)