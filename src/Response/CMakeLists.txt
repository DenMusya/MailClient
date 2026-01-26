add_library(Response SMTPResponse.cpp IMAPResponse.cpp)

target_include_directories(Response PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})