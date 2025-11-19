cmake_minimum_required(VERSION 4.1)
project(MailClient)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 NEW)

find_package(Boost 1.89 REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(boost_project INTERFACE)
target_include_directories(boost_project INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(boost_project INTERFACE 
  OpenSSL::SSL
  OpenSSL::Crypto
  ${Boost_LIBRARIES}
)

add_subdirectory(src)