#ifndef IMGBUFFER_H_INCLUDED
#define IMGBUFFER_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
//#include <libswscale/swscale.h>
//#include <libavutil/frame.h>
//#include <libavutil/imgutils.h>
//#include <libavcodec/avcodec.h>
#include "common.h"
#include "pngformat.h"
#include "vector2d.h"
#include "matrix2d.h"
#include "vector3d.h"
#include "matrix3d.h"

typedef struct IMGBUFFER {
    int w;
    int h;
    unsigned char *data;
    int s;
} IMGBUFFER;

CRESULT imgbuffer_create(IMGBUFFER *v);
CRESULT imgbuffer_destroy(IMGBUFFER *v);
CRESULT imgbuffer_new(IMGBUFFER *v, int w, int h);
CRESULT imgbuffer_clear(IMGBUFFER *v);
CRESULT imgbuffer_clearcolor(IMGBUFFER *v, unsigned int color);
CRESULT imgbuffer_load(IMGBUFFER *v, char *filename);
CRESULT imgbuffer_copy(IMGBUFFER *dest, IMGBUFFER *src);
CRESULT imgbuffer_copypos(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, int srcx, int srcy, int w, int h);
CRESULT imgbuffer_blend(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char alpha);
CRESULT imgbuffer_blendpossize(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, int srcx, int srcy, int w, int h, unsigned int color);
CRESULT imgbuffer_setpixel(IMGBUFFER *v, int x, int y, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char alpha);
CRESULT imgbuffer_setpixelblend(IMGBUFFER *dest, int x, int y, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char alpha);
CRESULT imgbuffer_getpixel(IMGBUFFER *v, int x, int y, unsigned char *cr, unsigned char *cg, unsigned char *cb, unsigned char *alpha);
void imgbuffer_rotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY,
    float fAngle, float fScale);

void imgbuffer_rotatep2(
    IMGBUFFER *dst, IMGBUFFER *src,
    float dstcentx, float dstcenty,
    float srccentx, float srccenty,
    float angle, float scale, unsigned int color);

void imgbuffer_rotateithclip(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY,
    float fAngle, float fScale);
void imgbuffer_myrotate(IMGBUFFER *dest, IMGBUFFER *src, double degree, unsigned int color);
CRESULT imgbuffer_blendmatrix(IMGBUFFER *dest, IMGBUFFER *src, MATRIX2D *rm, MATRIX2D *sm, int pixelw, int pixelh, int srcx, int srcy, int w, int h, unsigned int color);
//AVFrame *imgbuffer_converttoyuv420(IMGBUFFER *v);
//CRESULT imgbuffer_convertfromyuv420(IMGBUFFER *dest, AVFrame *src);

#endif
