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

#define rKiB(times) (times / 1024)
#define rMiB(times) (rKiB(times) / 1024)
#define rGiB(times) (rMiB(times) / 1024)

#define MEMORY_SIZE KiB(16)

#define SWI_TABLE 0x10000
#define SCHEDULER 0x11000
#define SCHEDULER_TIMEOUT SCHEDULER
#define SCHEDULER_FUNCTION (SCHEDULER + 0x4)

enum { CR_SCHEDULER = 1 };

/* Indicator for functions: the calle should take ownership of the return value and free it when not needed anymore */
#define CALLEOWNS
