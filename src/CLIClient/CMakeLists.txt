add_library(CLIClient CLIClient.cpp)

target_include_directories(CLIClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CLIClient 
boost_project
SMTPClient
IMAPClient
Logger
)
