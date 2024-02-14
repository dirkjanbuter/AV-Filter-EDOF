CC = gcc
SRCS = main.c imgbuffer.c matrix2d.c matrix3d.c vector2d.c vector3d.c pngformat.c log.c
FILES = $(addprefix src/, $(SRCS))
OBJS = $(FILES:.c=.o)
LIBS = -lpng -lm 
CFLAGS =  

%.o: %.c $(FILES)
	$(CC) -c -o $@ $< $(CFLAGS) -fPIC 

build: $(OBJS)
	$(CC) $(OBJS) $(LIBS) -fPIC  --shared -o ./../build/filter_edof.so

dist: build
		rm $(OBJS)

clean:
		rm $(OBJS)
