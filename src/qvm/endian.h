#pragma once
#include <qvm.h>
#include <stdio.h>

int fromQendian(byte *bytes);
CALLEOWNS byte *toQendian(int n);
void ftoQendian(FILE *fd, int n);
