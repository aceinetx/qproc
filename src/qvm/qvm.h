#pragma once
#include <stdint.h>
#include <string.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

#define smemcpy(dest, src) memcpy(dest, src, sizeof(src))
#define KiB(times) (times * 1024)
#define MiB(times) (KiB(times) * 1024)
#define GiB(times) (MiB(times) * 1024)
#define MEMORY_SIZE KiB(16)
#define CALLEOWNS
