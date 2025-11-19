add_subdirectory(Logger)
add_subdirectory(SSLConnection)
add_subdirectory(BaseMailClient)
add_subdirectory(IMAPClient)
add_subdirectory(SMTPClient)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  SMTPClient
  IMAPClient
)