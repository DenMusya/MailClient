add_library(Error SMTPError.cpp NetworkError.cpp)

target_include_directories(Error PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Error 
boost_project
Response
)