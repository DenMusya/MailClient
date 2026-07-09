add_library(IMAPClient IMAPClient.cpp)

target_include_directories(IMAPClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(IMAPClient PUBLIC
  boost_project
  BaseMailClient
  Error
  Response
  VirtualMailbox
  Logger
)
