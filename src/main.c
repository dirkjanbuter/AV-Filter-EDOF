#include <math.h>
#include <stdarg.h>

#include "imgbuffer.h"

#define PACK_NUM 24

static int _w = 0;
static int _h = 0;
static int _framecount = 0;
static int _fps = 0;

static int _s = 0;
static int _sizebytes = 0;

static int _i = 0;
static int _j = 0;

static IMGBUFFER _backbuffer;
static IMGBUFFER _colorbuffer;
static int _first = TRUE;

static int _pack[] = {
	-2,-2,
	-2,-1,
	-2,0,
	-2,1,
	-2,2,
	-1,-2,
	-1,-1,
	-1,0,
	-1,1,
	-1,2,
	0,-2,
	0,-1,
	0,1,
	0,2,
	1,-2,
	1,-1,
	1,0,
	1,1,
	1,2,
	2,-2,
	2,-1,
	2,0,
	2,1,
	2,2
};

#define BLOCK 10
#define DEPTH_LEVELS 3
#define SENSITIVITY 8
static int _depthW[DEPTH_LEVELS];
static int _depthH[DEPTH_LEVELS];
static int _depthS[DEPTH_LEVELS];
static int *_depthL[DEPTH_LEVELS];
static int *_depthF[DEPTH_LEVELS];
static int *_contrastL;
static IMGBUFFER _tempBuffer;

void depthRecursive(int g, int a, int b)
{
	int x, y;
	unsigned char tr;
	unsigned char tg;
	unsigned char tb;
	unsigned char talpha;
	
	if(g >= DEPTH_LEVELS)
	{
		return;
	}

	if(g > 0)
	{
		for(x = 0; x < BLOCK; x++) for(y = 0; y < BLOCK; y++)
		{
			int sx = a*BLOCK + x; 
			int sy = b*BLOCK + y;
			int di = sy * _depthW[g] + sx;
									
			if(di < _depthS[g] && abs(_depthF[g][di] - _depthL[g][di]) > SENSITIVITY)
			{
				
				_depthL[g][di] = _depthF[g][di];
				
				depthRecursive(g-1, sx, sy);
				
			}	
		}		
	}					
	else	
	{
		for(x = 0; x < BLOCK; x++) for(y = 0; y < BLOCK; y++)
		{
			int sx = a*BLOCK + x; 
			int sy = b*BLOCK + y;
			int di = sy * _depthW[0] + sx;
			int conF = 0;
			
			if(di < _depthS[g])
			{
				for(int p = 0; p < PACK_NUM*2; p+=2)
				{	
					int diCur = (sy+_pack[p+1]) * _depthW[g] + (sx+_pack[p]);
					if(diCur >= 0 && diCur < _depthS[g])
					{
						conF += abs(_depthF[g][di] - _depthF[g][diCur]);
					}
				}
							
				if(conF > _contrastL[di])
				{
					_contrastL[di] = conF;
					_depthL[g][di] = _depthF[g][di];
					imgbuffer_getpixel(&_tempBuffer, sx, sy, &tr, &tg, &tb, &talpha);
					imgbuffer_setpixel(&_colorbuffer, sx, sy, tr, tg, tb, 0xff);
				}
			}
		}	
	}
}

			
void draw(char *text)
{	
	
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
	int a, b, c, d, i, j, x, y, g, h;

	imgbuffer_create(&_tempBuffer);
	imgbuffer_new(&_tempBuffer, _w, _h);
	imgbuffer_clearcolor(&_tempBuffer, 0xff000000);
        imgbuffer_copy(&_tempBuffer, &_backbuffer);
	
	if(_first)
	{
		_depthW[0] = _w;
		_depthH[0] = _h;
		_depthS[0] = _depthW[0] * _depthH[0];
		_depthL[0] = (int*)malloc(_depthS[0]*sizeof(int));
		_depthF[0] = (int*)malloc(_depthS[0]*sizeof(int));
		_contrastL = (int*)malloc(_depthS[0]*sizeof(int));		

		for(g = 1; g < DEPTH_LEVELS; g++)
		{
			_depthW[g] = _w / (BLOCK*g);
			_depthH[g] = _h / (BLOCK*g);
			_depthS[g] = _depthW[g] * _depthH[g];
			_depthL[g] = (int*)malloc(_depthS[g]*sizeof(int));
			_depthF[g] = (int*)malloc(_depthS[g]*sizeof(int));		
		}
		
		// gray
		for(x = 0; x < _depthW[0]; x++) for(y = 0; y < _depthH[0]; y++)
		{			
			imgbuffer_getpixel(&_tempBuffer, x, y, &tr, &tg, &tb, &talpha);
			_depthL[0][y * _depthW[0] + x] = 0;
			_contrastL[y * _depthW[0] + x] = -999999;
		}	

		// sub
		for(g = 0; g < DEPTH_LEVELS-1; g++)
		{
			for(x = 0; x < _depthW[g+1]; x++) for(y = 0; y < _depthH[g+1]; y++)
			{			
				_depthL[g+1][y * _depthW[g+1] + x] = 0;
			}
		}
			
		imgbuffer_create(&_colorbuffer);
		imgbuffer_new(&_colorbuffer, _w, _h);
		imgbuffer_clearcolor(&_colorbuffer, 0xff000000);
		imgbuffer_clearcolor(&_backbuffer, 0xff000000);
	}
	
	
	
	// gray
	for(x = 0; x < _depthW[0]; x++) for(y = 0; y < _depthH[0]; y++)
	{			
		imgbuffer_getpixel(&_tempBuffer, x, y, &tr, &tg, &tb, &talpha);
		_depthF[0][y * _depthW[0] + x] = tr+tg+tb;//0.1 * tr + 0.6 * tg + 0.3 * tb;
	}	

	// sub
	for(g = 0; g < DEPTH_LEVELS-1; g++)
	{
		for(x = 0; x < _depthW[g+1]; x++) for(y = 0; y < _depthH[g+1]; y++)
		{			
			_depthF[g+1][y * _depthW[g+1] + x] = 0;
		}
		
		for(x = 0; x < _depthW[g]; x++) for(y = 0; y < _depthH[g]; y++)
		{			
			_depthF[g+1][(y/BLOCK) * _depthW[g+1] + (x/BLOCK)] += _depthF[0][y * _depthW[g] + x];
		}
	}
	
	g = DEPTH_LEVELS-1;
	for(x = 0; x < _depthW[g]; x++) for(y = 0; y < _depthH[g]; y++)
	{
		depthRecursive(g-1, x, y);
	}
	
	imgbuffer_copy(&_backbuffer, &_colorbuffer);
	
	imgbuffer_destroy(&_tempBuffer);
	
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
	int g;

	if(!_first)
	{

		imgbuffer_destroy(&_colorbuffer);		
		
		for(g = 0; g < DEPTH_LEVELS; g++)
		{
			free(_depthL[g]);
			free(_depthF[g]);		
		}
		free(_contrastL);		
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
