cmake_minimum_required(VERSION 3.20)
project(qvm)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE QVM_SOURCES CONFIGURE_DEPENDS "src/qvm/*.c")
file(GLOB_RECURSE QAS_SOURCES CONFIGURE_DEPENDS "src/qas/*.c")
file(GLOB_RECURSE QCC_SOURCES CONFIGURE_DEPENDS "src/qcc/*.c")
set(SOURCES "src/util.c")

if(LINUX)
	add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-type-limits)
endif()

include_directories("include/qvm" "include/qas" "include/qcc" "include")
add_executable(qvm ${QVM_SOURCES} ${SOURCES})
add_executable(qas ${QAS_SOURCES} ${SOURCES} "src/qvm/endian.c")
add_executable(qcc ${QCC_SOURCES} ${SOURCES} "src/qvm/endian.c")
