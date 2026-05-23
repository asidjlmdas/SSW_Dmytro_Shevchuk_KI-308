cmake_minimum_required(VERSION 3.15)
project(VMProcessManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/Process.cpp
    src/ProcessManager.cpp
    src/Logger.cpp
    src/VirtualMachine.cpp
)

add_executable(vmpm src/main.cpp ${SOURCES})

# Tests
add_executable(vmpm_tests tests/test_main.cpp ${SOURCES})
target_compile_options(vmpm_tests PRIVATE -DTESTING)
