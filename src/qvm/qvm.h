#pragma once
#include <stdint.h>
#include <string.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

#define MEMORY_SIZE 512
#define smemcpy(dest, src) memcpy(dest, src, sizeof(src))
