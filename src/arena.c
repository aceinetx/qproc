#include <arena.h>
#include <stdlib.h>

Arena arena_new(void) {
	Arena arena;
	arena.data = NULL;
	arena.length = 0;
	arena.size = 0;
	return arena;
}

void* arena_alloc(Arena* arena, uint32_t size) {
	arena->data = realloc(arena->data, arena->size + sizeof(void*));
	arena->data[arena->length] = malloc(size);

	arena->length++;
	arena->size = arena->length * sizeof(void*);

	return arena->data[arena->length - 1];
}

void arena_append(Arena* arena, void* ptr) {
	uint32_t i;
	for (i = 0; i < arena->length; i++) {
		if (arena->data[i] == ptr) {
			return; /* Skip adding duplicate pointer */
		}
	}

	arena->data = realloc(arena->data, arena->size + sizeof(void*));
	arena->data[arena->length] = ptr;
	arena->length++;
	arena->size = arena->length * sizeof(void*);
}

void arena_free(Arena* arena) {
	uint32_t i;

	if (!arena->data)
		return;

	for (i = 0; i < arena->length; i++) {
		free(arena->data[i]);
	}
	free(arena->data);
	arena->data = NULL;
	arena->length = 0;
	arena->size = 0;
}
