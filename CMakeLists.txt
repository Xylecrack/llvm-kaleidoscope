cmake_minimum_required(VERSION 3.31.6)
project(KaleidoscopeCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES "src/*.cpp")
add_executable(KaleidoscopeCompiler ${SOURCES})