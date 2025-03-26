#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>

dword fromQendian(byte *bytes) {
	dword value = 0;
	dword i = 0;
	for (; i < 4; ++i) {
		value |= ((dword)(bytes[i]) << (i * 8));
	}
	return value;
}

CALLEOWNS byte *toQendian(dword n) {
	byte *bytes = malloc(4);
	size_t i = 0;
	for (; i < 4; ++i) {
		bytes[i] = (n >> (i * 8)) & 0xFF;
	}
	return bytes;
}

void ftoQendian(FILE *fd, dword n) {
	size_t i = 0;
	for (; i < 4; ++i) {
		fprintf(fd, "%c", (n >> (i * 8)) & 0xFF);
	}
}
