add_library(IMAPClient IMAPClient.cpp)

target_include_directories(IMAPClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(IMAPClient boost_project BaseMailClient Error Response)