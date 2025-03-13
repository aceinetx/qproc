#pragma once
#include <qvm.h>
#include <stdio.h>

int fromQendian(byte *bytes);
byte *toQendian(int n);
void ftoQendian(FILE *fd, int n);
