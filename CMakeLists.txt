cmake_minimum_required(VERSION 3.6)

project(CppRtti)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(SETTINGS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(${SETTINGS_CMAKE_DIR}/settings.cmake)

add_subdirectory (${RTTI_DIR})
add_subdirectory (${RTTI_CLASS_DIR})

include_directories(${RTTI_DIR})
include_directories(${RTTI_CLASS_DIR})
include_directories(${THIRDPARTY_DIR})

add_executable(CppRtti main.cpp rtti_test.cpp)
target_link_libraries(
        CppRtti
        ${RTTI_CLASS_LIBRARY}
)


