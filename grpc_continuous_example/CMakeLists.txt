set(EXECUTABLE_NAME grpc_continuous_example)

add_executable(${EXECUTABLE_NAME} main.cc)

target_link_libraries(${EXECUTABLE_NAME}
        SmartSpectra::Container
        SmartSpectra::VideoSource
)
