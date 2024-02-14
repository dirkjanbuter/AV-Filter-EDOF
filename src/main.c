#include <math.h>
#include <stdarg.h>

#include "imgbuffer.h"

#define PACK_NUM 8
#define LAYERS_NUM 1
#define SHARP_NUM 0xff

static int _w = 0;
static int _h = 0;
static int _framecount = 0;
static int _fps = 0;

static int _s = 0;
static int _sizebytes = 0;

static int _i = 0;
static int _j = 0;

static int _t = 0;
static int _tl = LAYERS_NUM;
static IMGBUFFER _backbuffer;
static IMGBUFFER _colorbuffer[LAYERS_NUM];
static long *_layerNumber;
static int *_pixelNumber;
static int _first = TRUE;

static int _pack[] = {
	-1, -1,
	 0, -1,
	 1, -1,
	-1,  0,
	 1,  0,
	-1,  1,
	 0,  1,
	 1,  1,
};


void draw(char *text)
{	
	IMGBUFFER tempBuffer;
	unsigned char tr;
	unsigned char tg;
	unsigned char tb;
	unsigned char talpha;
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	unsigned char calpha;
	long pr[PACK_NUM];
	long pg[PACK_NUM];
	long pb[PACK_NUM];
	long palpha[PACK_NUM];
	long fr;
	long fg;
	long fb;
	long falpha;

	imgbuffer_create(&tempBuffer);
	imgbuffer_new(&tempBuffer, _w, _h);
	imgbuffer_clearcolor(&tempBuffer, 0xff000000);

	if(_first)
	{
		for(_t = 0; _t < _tl; _t++)
		{
			imgbuffer_create(&_colorbuffer[_t]);
			imgbuffer_new(&_colorbuffer[_t], _w, _h);
			imgbuffer_clearcolor(&_colorbuffer[_t], 0xff000000);
		}
		imgbuffer_clearcolor(&_backbuffer, 0xff000000);

		_layerNumber = (unsigned long*)malloc(_s * sizeof(unsigned long) * 3);
		memset(_layerNumber, 0, _s * sizeof(unsigned long) * 3);
		
		_pixelNumber = (int *)malloc(_s * sizeof(int) * _tl);
		memset(_pixelNumber, 0, _s * sizeof(int) * _tl);

	
	}
	
	imgbuffer_copy(&tempBuffer, &_backbuffer);
	
	for(_j = 1; _j < _h-2; _j++) for(_i = 1; _i < _w-2; _i++)
	{			
		for(int p = 0; p < PACK_NUM; p++)
		{	
			imgbuffer_getpixel(&tempBuffer, _i+_pack[p], _j+_pack[p], &tr, &tg, &tb, &talpha);
			pr[p] = tr;
			pg[p] = tg;
			pb[p] = tb;
		}

		imgbuffer_getpixel(&tempBuffer, _i, _j, &tr, &tg, &tb, &talpha);
		
		fr = 0;
		fg = 0;
		fb = 0;		

		for(int p = 0; p < PACK_NUM; p++)
		{
			if(
				(tr > pr[p] || tg > pg[p] || tb > pb[p])					
			)
			{
				fr += abs(tr - pr[p]);
				fg += abs(tg - pg[p]);
				fb += abs(tb - pb[p]);		
			}
		}

/*
		for(int p = 0; p < PACK_NUM; p++)
		{
			if(
				(tr > pr[p])					
			)
			{
				fr += abs(tr - pr[p]);
			}
			if(
				(tg > pg[p])					
			)
			{
				fg += abs(tg - pg[p]);
			}
			if(
				(tb > pb[p])					
			)
			{
				fb += abs(tb - pb[p]);		
			}
		}

*/
		if( fr > _layerNumber[(_j*_w+_i)*3+0] )
		{				
			unsigned int di = (_j*_w+_i)*4;
			_layerNumber[(_j*_w+_i)*3+0] = fr;
			_colorbuffer[0].data[di+1] = tr;
		}
		if( fg > _layerNumber[(_j*_w+_i)*3+1] )
		{				
			unsigned int di = (_j*_w+_i)*4;
			_layerNumber[(_j*_w+_i)*3+1] = fg;
			_colorbuffer[0].data[di+2] = tg;
		}
		if( fb > _layerNumber[(_j*_w+_i)*3+2] )
		{				
			unsigned int di = (_j*_w+_i)*4;
			_layerNumber[(_j*_w+_i)*3+2] = fb;
			_colorbuffer[0].data[di+3] = tb;
		}
	}
	
	imgbuffer_clearcolor(&_backbuffer, 0xff000000);
	for(_t = 0; _t < _tl; _t++)
	{
		imgbuffer_blend(&_backbuffer, &_colorbuffer[_t], 0, 0, 0xff, 0xff, 0xff, 0xff);
	}		
	
	imgbuffer_destroy(&tempBuffer);
	if(_first)
	{
		_first = FALSE;
	}
}

int filtercreate(int fps)
{
	_fps = fps;

	_first = TRUE;
	
	return 1;
}

void filterdestroy()
{
	free(_layerNumber);	

	if(!_first)
	{
		for(_t = 0; _t < _tl; _t++)
		{
 			imgbuffer_destroy(&_colorbuffer[_t]);
 		}
	}
}


int filtervideo(unsigned char *buffer, int w, int h, unsigned int color, char *text, int64_t framecount)
{
    _w = w;
    _h = h;
    _framecount = framecount;

    _s = _w * _h;
    _sizebytes = _s * 4;

    _backbuffer.data = buffer;
    _backbuffer.w = _w;
    _backbuffer.h = _h;
    _backbuffer.s = _sizebytes;

    srand(_framecount);

    draw(text);

    return 1;
}
