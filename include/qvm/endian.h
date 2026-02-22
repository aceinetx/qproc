#pragma once
#include <qvm.h>
#include <stdio.h>

dword fromQendian(byte bytes[4]);
void toQendian(dword n, byte bytes[4]);
void ftoQendian(FILE* fd, dword n);
