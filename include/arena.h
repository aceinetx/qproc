#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

typedef struct Arena {
	void** data;
	uint32_t size;
	uint32_t length;
} Arena;

Arena arena_new(void);

void* arena_alloc(Arena* arena, uint32_t size);
void arena_append(Arena* arena, void* ptr);

void arena_free(Arena* arena);

#endif
