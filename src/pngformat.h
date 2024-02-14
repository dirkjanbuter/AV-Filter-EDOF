#ifndef PNGFORMAT_H_INCLUDED
#define PNGFORMAT_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#include "common.h"

typedef struct PNGFORMAT {
    int x;
    int y;

    int width;
    int height;
    int color_type;
    int bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
    size_t rowbytes;
    png_infop end_info;
    png_byte *image_data;
    unsigned char *argb;

    double default_display_exponent;
} PNGFORMAT;

CRESULT pngformat_create(PNGFORMAT *v);
CRESULT pngformat_destroy(PNGFORMAT *v);
CRESULT pngformat_read(PNGFORMAT *v, char* filename);
unsigned char *pngformat_getdata(PNGFORMAT *v);
int pngformat_getwidth(PNGFORMAT *v);
int pngformat_getheight(PNGFORMAT *v);

#endif
