cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(SETTINGS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${SETTINGS_CMAKE_DIR}/settings.cmake)

include_directories(${THIRDPARTY_DIR})

aux_source_directory(. CURRENT_SOURCE)

add_library(${RTTI_LIBRARY} STATIC ${CURRENT_SOURCE})