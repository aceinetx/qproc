cmake_minimum_required(VERSION 3.20)
project(qvm)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)

file(GLOB_RECURSE QVM_SOURCES "src/qvm/*.c")
file(GLOB_RECURSE QAS_SOURCES "src/qas/*.c")
set(SOURCES "src/util.c")

if(LINUX)
	add_compile_options(-Wall -Wextra -Wpedantic -Wno-type-limits -Werror -Wno-unused-parameter)
endif()

include_directories("src/qvm" "src/qas" "src")
add_executable("qvm" ${QVM_SOURCES} ${SOURCES})
add_executable("qas" ${QAS_SOURCES} ${SOURCES} "src/qvm/endian.c")
