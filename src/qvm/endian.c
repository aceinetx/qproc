#include <qvm.h>
#include <stdio.h>

dword fromQendian(byte bytes[4]) {
	dword value, i;
	value = 0;

	for (i = 0; i < 4; ++i) {
		value |= ((dword)(bytes[i]) << (i * 8));
	}
	return value;
}

void toQendian(dword n, byte bytes[4]) {
	dword i;

	for (i = 0; i < 4; ++i) {
		bytes[i] = (n >> (i * 8)) & 0xFF;
	}
}

void ftoQendian(FILE* fd, dword n) {
	size_t i;
	for (i = 0; i < 4; ++i) {
		fprintf(fd, "%c", (n >> (i * 8)) & 0xFF);
	}
}
