add_library(SSLConnection SSLConnection.cpp)

target_include_directories(SSLConnection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SSLConnection 
boost_project
Logger
)