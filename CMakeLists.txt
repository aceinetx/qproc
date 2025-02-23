cmake_minimum_required(VERSION 3.20)
project(qvm)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE QVM_SOURCES "src/qvm/*.cc" "src/qvm/inst/*.cc")
file(GLOB_RECURSE QAS_SOURCES "src/qas/*.cc")
set(SOURCES "src/fs.cc" "src/util.cc")

if (MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Wno-type-limits)
endif()

include_directories("src/qvm" "src/qvm/inst" "src/qas" "src")
add_executable("qvm" ${QVM_SOURCES} ${SOURCES})
add_executable("qas" ${QAS_SOURCES} ${SOURCES})
