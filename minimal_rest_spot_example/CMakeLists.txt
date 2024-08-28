set(EXECUTABLE_NAME minimal_rest_spot_example)

add_executable(${EXECUTABLE_NAME} main.cc)

target_link_libraries(${EXECUTABLE_NAME}
        SmartSpectra::Container
        SmartSpectra::VideoSource
)
