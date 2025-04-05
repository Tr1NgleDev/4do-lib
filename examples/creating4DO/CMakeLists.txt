add_executable(creating4DO main.cpp)
target_include_directories(creating4DO PRIVATE "../../include/")

install(TARGETS creating4DO RUNTIME DESTINATION bin)
