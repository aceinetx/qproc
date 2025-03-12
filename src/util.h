#pragma once
#include <qvm.h>

#ifdef __cplusplus
extern "C" {
#endif

char *args_shift(int *argc, char ***argv);
byte *fs_read(const char *filename, dword *outSize);

#ifdef __cplusplus
}
#endif
