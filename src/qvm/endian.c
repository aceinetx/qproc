#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>

dword fromQendian(byte* bytes) {
	dword value, i;
	value = 0;

	for (i = 0; i < 4; ++i) {
		value |= ((dword)(bytes[i]) << (i * 8));
	}
	return value;
}

CALLEOWNS byte* toQendian(dword n) {
	byte* bytes;
	dword i;

	bytes = malloc(4);
	for (i = 0; i < 4; ++i) {
		bytes[i] = (n >> (i * 8)) & 0xFF;
	}
	return bytes;
}

void ftoQendian(FILE* fd, dword n) {
	size_t i;
	for (i = 0; i < 4; ++i) {
		fprintf(fd, "%c", (n >> (i * 8)) & 0xFF);
	}
}
