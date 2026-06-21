cmake_minimum_required(VERSION 3.16)
project(EnginePatchCLI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../../ThirdParty/EnginePatchCore EnginePatchCore)

add_executable(EnginePatchCLI main.cpp)
target_link_libraries(EnginePatchCLI PRIVATE EnginePatchCore)

if(WIN32)
    target_link_libraries(EnginePatchCLI PRIVATE Advapi32)
endif()
