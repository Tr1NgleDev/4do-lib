add_executable(loading4DO main.cpp)
target_include_directories(loading4DO PRIVATE "../../include/")

install(TARGETS loading4DO RUNTIME DESTINATION bin)

add_custom_command(
        TARGET loading4DO POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/loading4DO/example.4do
        ${CMAKE_CURRENT_BINARY_DIR}/example.4do)
