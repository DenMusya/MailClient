
add_library(SMTPClient SMTPClient.cpp)

target_include_directories(SMTPClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SMTPClient 
boost_project
BaseMailClient
Base64Encode
Response
Error
)