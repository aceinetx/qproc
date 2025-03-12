#include <endian.h>
#include <stdlib.h>

int fromQendian(byte *bytes) {
	dword value = 0;
	for (dword i = 0; i < 4; ++i) {
		value |= ((dword)(bytes[i]) << (i * 8));
	}
	return value;
}

byte *toQendian(int n) {
	byte *bytes = malloc(4);
	for (size_t i = 0; i < 4; ++i) {
		bytes[i] = (n >> (i * 8)) & 0xFF;
	}
	return bytes;
}
