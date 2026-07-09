add_subdirectory(Logger)
add_subdirectory(Response)
add_subdirectory(Error)
add_subdirectory(SSLConnection)
add_subdirectory(Base64Encode)
add_subdirectory(CommandQueue)
add_subdirectory(VirtualMailbox)
add_subdirectory(BaseMailClient)
add_subdirectory(IMAPClient)
add_subdirectory(SMTPClient)
add_subdirectory(CLIClient)


add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  SMTPClient
  IMAPClient
  CLIClient
)