#pragma once
#include <qvm.h>

#define SOURCES_MAX 64

typedef struct Source {
	dword start_line;
	char *name;
} Source;

extern Source sources_store[SOURCES_MAX];
