add_executable(loading4DO main.cpp)
target_include_directories(loading4DO PRIVATE "../../includes/")

install(TARGETS loading4DO RUNTIME DESTINATION bin)
