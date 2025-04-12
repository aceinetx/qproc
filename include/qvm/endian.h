#pragma once
#include <qvm.h>
#include <stdio.h>

dword fromQendian(byte* bytes);
CALLEOWNS byte* toQendian(dword n);
void ftoQendian(FILE* fd, dword n);
