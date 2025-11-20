add_library(BaseMailClient BaseMailClient.cpp)

target_include_directories(BaseMailClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(BaseMailClient 
boost_project
SSLConnection
Logger
CommandQueue
)