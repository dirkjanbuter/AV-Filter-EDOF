#include "imgbuffer.h"

#define sR (char)(rgb[s+0])
#define sG (char)(rgb[s+1])
#define sB (char)(rgb[s+2])
char *rgb2yuv420p(int width, int height, int colors, char *rgb) {
    int j;
    int k;
    unsigned int i = 0;
    unsigned int numpixels = width * height;
    unsigned int ui = numpixels;
    unsigned int vi = numpixels + numpixels / 4;
    unsigned int s = 0;
    char *yuv420p;

    yuv420p = (char*)malloc(sizeof(char) * (numpixels * 3 / 2));
    if(!yuv420p)
        return NULL;

    for (j = 0; j < height; j++) for (k = 0; k < width; k++)
    {
      yuv420p[i] = (char)( (66*sR + 129*sG + 25*sB + 128) >> 8) + 16;

      if (0 == j%2 && 0 == k%2)
      {
        yuv420p[ui++] = (char)( (-38*sR - 74*sG + 112*sB + 128) >> 8) + 128;
        yuv420p[vi++] = (char)( (112*sR - 94*sG - 18*sB + 128) >> 8) + 128;
      }
      i++;
      s += colors;
    }

    return yuv420p;
}

CRESULT imgbuffer_create(IMGBUFFER *v) {
    v->w = 0;
    v->h = 0;
    v->s = 0;
    v->data = NULL;
    return CSUCCESS;
}

CRESULT imgbuffer_destroy(IMGBUFFER *v) {
    free(v->data);
    return CSUCCESS;
}

CRESULT imgbuffer_new(IMGBUFFER *v, int w, int h) {
    v->w = w;
    v->h = h;
    v->s = v->w * v->h * 4;
    v->data = (unsigned char*)malloc(v->s);
    return CSUCCESS;
}

CRESULT imgbuffer_clear(IMGBUFFER *v) {
    memset(v->data, 0, v->s);
    return CSUCCESS;
}

CRESULT imgbuffer_clearcolor(IMGBUFFER *v, unsigned int color) {
    int x, y;
    int a = (color >> 24) & 0xff;
    int r = (color >> 16) & 0xff;
    int g = (color >> 8) & 0xff;
    int b = color & 0xff;
    for(y=0;y<v->h;y++) for(x=0;x<v->w;x++) {
        imgbuffer_setpixel(v, x, y, r, g, b, a);
    }
    return CSUCCESS;
}


CRESULT imgbuffer_load(IMGBUFFER *v, char *filename){
    PNGFORMAT pngimg;

    if(pngformat_create(&pngimg) == CFAILED) {
        return CFAILED;
    }
    if(pngformat_read(&pngimg, filename) == CFAILED) {
        return CFAILED;
    }
    imgbuffer_new(v, pngformat_getwidth(&pngimg), pngformat_getheight(&pngimg));

    memcpy(v->data, pngformat_getdata(&pngimg), v->s);

    pngformat_destroy(&pngimg);

    return CSUCCESS;
}


CRESULT imgbuffer_copy(IMGBUFFER *dest, IMGBUFFER *src) {
    if(dest->s != src->s)
        return CFAILED;
    memcpy(dest->data, src->data, src->s);
    return CSUCCESS;
}

CRESULT imgbuffer_copypos(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, int srcx, int srcy, int w, int h) {
    int y;
    int len = w;
    // max destination range
    if(destx+len > dest->w)
        len = dest->w - destx;
    // max source range
    if(srcx+len > src->w)
        len = src->w - srcx;
    for(y=0;y<dest->h && y<src->h && y<h;y++) {
        // destenation start index
        int di = (desty+y)*dest->w*4+destx*4;
        // source start index
        int si = (srcy+y)*src->w*4+srcx*4;
        // copy mem
        memcpy(&dest->data[di], &src->data[si], len*4);
    }
    return CSUCCESS;
}


CRESULT imgbuffer_blend(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int x, y;
    for(y=0;y<src->h;y++) for(x=0;x<src->w;x++) {
        unsigned char cr, cg, cb, alpha;
        if(imgbuffer_getpixel(src, x, y, &cr, &cg, &cb, &alpha) == CSUCCESS) {
                imgbuffer_setpixelblend(dest, destx+x, desty+y, cr & r, cg & g, cb & b, alpha & a);
        }
    }
    return CSUCCESS;
}

CRESULT imgbuffer_blendpossize(IMGBUFFER *dest, IMGBUFFER *src, int destx, int desty, int srcx, int srcy, int w, int h, unsigned int color) {
    int x, y;
    int a = color >> 24 & 0xff;
    int r = color >> 16 & 0xff;
    int g = color >> 8 & 0xff;
    int b = color & 0xff;
    for(y=0;y<h;y++) for(x=0;x<w;x++) {
        unsigned int di = ((desty+y)*dest->w+destx+x)*4;
        unsigned int si = ((srcy+y)*src->w+srcx+x)*4;
        unsigned int alpha = src->data[si] & a;
        unsigned int invalpha = 0xff - alpha;
        if(alpha && destx+x >= 0 && desty+y >= 0 && destx+x < dest->w && desty+y < dest->h) {
            dest->data[di+1] = ((((unsigned int)dest->data[di+1] * invalpha) + ((src->data[si+1] & r) * alpha)) >> 8);
            dest->data[di+2] = ((((unsigned int)dest->data[di+2] * invalpha) + ((src->data[si+2] & g) * alpha)) >> 8);
            dest->data[di+3] = ((((unsigned int)dest->data[di+3] * invalpha) + ((src->data[si+3] & b) * alpha)) >> 8);
            dest->data[di] = 0xff;
        }
    }
    return CSUCCESS;
}

//////////////////////////////////////////////////////////////////
// Rotate - wrapping version
// This version takes any dimension source bitmap and wraps.
//////////////////////////////////////////////////////////////////
void imgbuffer_rotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY,
    float fAngle, float fScale)
{

    srcDelta /= sizeof(WORD);
    dstDelta /= sizeof(WORD);

    float duCol = (float)sin(-fAngle) * (1.0f / fScale);
    float dvCol = (float)cos(-fAngle) * (1.0f / fScale);
    float duRow = dvCol;
    float dvRow = -duCol;

    float startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
    float startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);

    float rowu = startingu;
    float rowv = startingv;

    for(int y = 0; y < dstH; y++)
    {
        float u = rowu;
        float v = rowv;

        WORD *pDst = pDstBase + (dstDelta * y);

        for(int x = 0; x < dstW ; x++)
        {
            int sx = ((u < 0.0 ? (int)-u : (int)u) + srcW) % srcW;
            int sy = ((v < 0.0 ? (int)-v : (int)v) + srcH) % srcH;

            WORD *pSrc = pSrcBase + sx + (sy * srcDelta);

            *pDst++ = *pSrc++;

            u += duRow;
            v += dvRow;
        }

        rowu += duCol;
        rowv += dvCol;
    }
}

// images with power of two - fast rotation
void imgbuffer_rotatep2(
    IMGBUFFER *dst, IMGBUFFER *src,
    float dstcentx, float dstcenty,
    float srccentx, float srccenty,
    float angle, float scale, unsigned int color)
{
    unsigned char cr=0, cg=0, cb=0, alpha=0;
    int a = color >> 24 & 0xff;
    int r = color >> 16 & 0xff;
    int g = color >> 8 & 0xff;
    int b = color & 0xff;
    float cu = (float)sin(-angle) * (1.0f / scale);
    float cv = (float)cos(-angle) * (1.0f / scale);
    float ru = cv;
    float rv = -cu;

    float startingu = srccentx - (dstcentx * cv + dstcenty * cu);
    float startingv = srccenty - (dstcentx * rv + dstcenty * ru);

    float rowu = startingu;
    float rowv = startingv;

    for(int y = 0; y < dst->h; y++)
    {
        float u = rowu;
        float v = rowv;
        for(int x = 0; x < dst->w ; x++)
        {
            if(imgbuffer_getpixel(src, u, v, &cr, &cg, &cb, &alpha) == CSUCCESS)
                imgbuffer_setpixelblend(dst, x, y, cr & r, cg & g, cb & b, alpha & a);
            u += ru;
            v += rv;
        }
        rowu += cu;
        rowv += cv;
    }
}

//////////////////////////////////////////////////////////////////
// Rotate - nowrapping clipping version
//
// this version does not have the limitation of power of 2
// dimensions and will clip the source image instead of wrapping.
//////////////////////////////////////////////////////////////////

void imgbuffer_rotateithclip(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY,
    float fAngle, float fScale)
{
    srcDelta /= sizeof(WORD);
    dstDelta /= sizeof(WORD);

    float duCol = (float)sin(-fAngle) * (1.0f / fScale);
    float dvCol = (float)cos(-fAngle) * (1.0f / fScale);
    float duRow = dvCol;
    float dvRow = -duCol;

    float startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
    float startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);

    float rowu = startingu;
    float rowv = startingv;


    for(int y = 0; y < dstH; y++)
    {
        float u = rowu;
        float v = rowv;

        WORD *pDst = pDstBase + (dstDelta * y);

        for(int x = 0; x < dstW ; x++)
        {
            if(u>0 && v>0 && u<srcW && v<srcH)
            {
                WORD *pSrc = pSrcBase + (int)u +
                                ((int)v * srcDelta);

                *pDst++ = *pSrc++;
            }
            else
            {
                *pDst++ = 0;
            }

            u += duRow;
            v += dvRow;
        }

        rowu += duCol;
        rowv += dvCol;
    }
}


CRESULT imgbuffer_blendmatrix(IMGBUFFER *dest, IMGBUFFER *src, MATRIX2D *rm, MATRIX2D *stm, int pixelw, int pixelh, int srcx, int srcy, int w, int h, unsigned int color) {
    unsigned char cr=0, cg=0, cb=0, alpha=0;
    int x, y, x2=0,y2=0;
    int a = color >> 24 & 0xff;
    int r = color >> 16 & 0xff;
    int g = color >> 8 & 0xff;
    int b = color & 0xff;
    int stepx;
    int stepy;
    int stepx2;
    int stepy2;
    VECTOR2D *sv = vector2d_create((float)w, (float)h);
    VECTOR2D *svn = matrix2d_vector(stm, sv);

    stepx2 = stepx = sv->x / w;
    stepy2 = stepy = sv->y / h;
    if(stepx < 1) stepx = 1;
    if(stepy < 1) stepy = 1;

    if(stepx2 < 0) stepx2 = abs(stepx2);
    else stepx2 = 1;
    if(stepy2 < 0) stepy2 = abs(stepy2);
    else stepy2 = 1;

    for(y=0;y<h;y+=stepy) for(x=0;x<w;x+=stepx) {

        VECTOR2D *pv = vector2d_create((float)x, (float)y);
        VECTOR2D *pvn = matrix2d_vector(stm, pv);
        if(imgbuffer_getpixel(src, srcx+x, srcy+y, &cr, &cg, &cb, &alpha) == CSUCCESS) {
            for(y2 = 0; y2 < 1; y2++) for(x2 = 0; x2 < 1; x2++) {
                imgbuffer_setpixelblend(dest, (int)pvn->x + x2, (int)pvn->y + y2, cr & r, cg & g, cb & b, alpha & a);
            }
        }
        vector2d_destroy(pvn);
        vector2d_destroy(pv);
    }
    vector2d_destroy(svn);
    vector2d_destroy(sv);
    return CSUCCESS;
}


CRESULT imgbuffer_setpixel(IMGBUFFER *v, int x, int y, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char alpha) {
    unsigned int di = (y*v->w+x)*4;
    if(x >= 0 && y >= 0 && x < v->w && y < v->h) {
        v->data[di] = alpha;
        v->data[di+1] = cr;
        v->data[di+2] = cg;
        v->data[di+3] = cb;
        return CSUCCESS;
    }
    return CFAILED;
}

CRESULT imgbuffer_setpixelblend(IMGBUFFER *v, int x, int y, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char alpha) {
    unsigned int di = (y*v->w+x)*4;
    unsigned int invalpha = 0xff - alpha;
    if(alpha && di+3 < v->s && x >= 0 && y >= 0 && x < v->w && y < v->h) {
        v->data[di] = 0xff;
        v->data[di+1] = ((v->data[di+1] * invalpha) + (cr * alpha)) >> 8;
        v->data[di+2] = ((v->data[di+2] * invalpha) + (cg * alpha)) >> 8;
        v->data[di+3] = ((v->data[di+3] * invalpha) + (cb * alpha)) >> 8;
        return CSUCCESS;
    }
    return CFAILED;
}

CRESULT imgbuffer_getpixel(IMGBUFFER *v, int x, int y, unsigned char *cr, unsigned char *cg, unsigned char *cb, unsigned char *alpha) {
    unsigned int di = (y*v->w+x)*4;
    if(di+3 < v->s && x >= 0 && y >= 0 && x < v->w && y < v->h) {
        *alpha = v->data[di];
        *cr = v->data[di+1];
        *cg = v->data[di+2];
        *cb = v->data[di+3];
        return CSUCCESS;
    }
    return CFAILED;
}
/*
AVFrame *imgbuffer_converttoyuv420(IMGBUFFER *v) {
    int ret;
    AVFrame *frame = av_frame_alloc();
    int size;
    uint8_t *buffer;

    if(!frame)
        return NULL;
    size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, v->w, v->h, 64);
    buffer = (uint8_t *)av_malloc(size);
    if (!buffer) {
        av_free(frame);
        return NULL;
    }
    ret = av_image_fill_arrays(frame->data, frame->linesize, buffer, AV_PIX_FMT_YUV420P, v->w, v->h, 64);
    if (ret < 0) {
        av_free(frame);
        av_free(buffer);
		fprintf(stderr, "Could not allocate picture: %s\n", av_err2str(ret));
		return NULL;
	}
	frame->width = v->w;
	frame->height = v->h;
	frame->format = AV_PIX_FMT_YUV420P;

    struct SwsContext *ctx = sws_getContext(v->w, v->h,
                                      AV_PIX_FMT_RGBA, v->w, v->h,
                                      AV_PIX_FMT_YUV420P,
                                      SWS_BILINEAR, NULL, NULL, NULL);

    const uint8_t srcSlice[4] = {v->data, 0, 0, 0};
    const int srcStride[4] = {v->w*4, 0, 0, 0};
    int srcSliceY = 0;

    //sws_scale(ctx, srcSlice, srcStride, srcSliceY, v->h, frame->data, frame->linesize);
    sws_freeContext(ctx);

    return frame;
}

CRESULT imgbuffer_convertfromyuv420(IMGBUFFER *v, AVFrame *src) 
{
	struct SwsContext *ctx = NULL;
	AVFrame* dst = NULL;

	dst = av_frame_alloc();
	
	av_image_fill_arrays(dst->data, dst->linesize,
                                v->data, AV_PIX_FMT_RGBA, v->w, v->h, 1);
        	
	ctx = sws_getContext(
		src->width, src->height, src->format, 
		v->w, v->h, AV_PIX_FMT_ARGB, 
		SWS_BILINEAR, 0, 0, 0);
		
		
	if (!ctx) 
	{
		fprintf(stderr,
		        "Impossible to create scale context for the conversion "
		        "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
		        av_get_pix_fmt_name(src->format), src->width, src->height,
		        av_get_pix_fmt_name(dst->format), dst->width, dst->height);
		return CFAILED;
	}
	

	sws_scale(ctx, (const uint8_t * const *)src->data, src->linesize, 0, src->height, 
		(uint8_t * const *)dst->data, dst->linesize);
	
	av_free(dst);
	sws_freeContext(ctx);

	return CSUCCESS;
 }
*/
