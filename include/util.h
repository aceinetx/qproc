#pragma once
#include <qvm.h>

char *args_shift(int *argc, char ***argv);
byte *fs_read(const char *filename, dword *outSize);
