set(EXECUTABLE_NAME cpp_grpc_example)

add_executable(${EXECUTABLE_NAME} main.cc)

target_link_libraries(${EXECUTABLE_NAME}
        Physiology::Container
        Physiology::VideoSource
)
