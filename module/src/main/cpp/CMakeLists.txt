cmake_minimum_required(VERSION 3.22.1)
project(prctl_skipper)

set(CXX_FLAGS "${CXX_FLAGS} -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

if(ANDROID_ABI STREQUAL "arm64-v8a")
    add_library(${MODULE_NAME} SHARED example.cpp And64InlineHook.cpp)
else()
    add_library(${MODULE_NAME} SHARED example.cpp)
endif()
target_link_libraries(${MODULE_NAME} cxx log)

add_subdirectory(external)