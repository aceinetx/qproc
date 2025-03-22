#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#ifdef __cplusplus
extern "C" {
#endif

char *args_shift(int *argc, char ***argv) {
	assert(*argc > 0 && "argc <= 0");
	--(*argc);
	return *(*argv)++;
}

byte *fs_read(const char *filename, dword *outSize) {
	FILE *file;
	long fileSize;
	byte *buffer;
	dword bytesRead;

	file = fopen(filename, "rb");
	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (fileSize < 0) {
		fclose(file);
		return NULL;
	}

	buffer = (byte *)malloc(fileSize);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

	bytesRead = fread(buffer, 1, fileSize, file);
	if (bytesRead != fileSize) {
		free(buffer);
		fclose(file);
		return NULL;
	}

	fclose(file);

	if (outSize) {
		*outSize = (dword)fileSize;
	}

	return buffer;
}

#ifdef __cplusplus
}
#endif
