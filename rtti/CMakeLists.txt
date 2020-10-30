cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

# json lib
include_directories(../json)

# target rtti
aux_source_directory(. rtti_source)

add_library(
        rtti
        shared
        ${rtti_source}
)