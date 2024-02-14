#include "pngformat.h"

CRESULT pngformat_create(PNGFORMAT *v) {
     v->row_pointers = NULL;
     v->image_data = NULL;
     v->argb = NULL;

     v->default_display_exponent = 0.0;

     return CSUCCESS;
}

CRESULT pngformat_destroy(PNGFORMAT *v) {
    if(v->row_pointers) free(v->row_pointers);
    if(v->image_data) free(v->image_data);
    if(v->argb) free(v->argb);
    return CSUCCESS;
}

CRESULT pngformat_read(PNGFORMAT *v, char* filename) {
    png_byte header[8];
    int i;
    size_t s;

    FILE *fp = fopen(filename, "rb");
    if (fp == 0)
    {
        perror(filename);
        return CFAILED;
    }

    // read the header
    s = fread(header, 1, 8, fp);
    if(s != 8)
    {
        fprintf(stderr, "error: %s is not a PNG.\n", filename);
        fclose(fp);
        return CFAILED;
    }

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", filename);
        fclose(fp);
        return CFAILED;
    }

    v->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!v->png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return CFAILED;
    }

    // create png info struct
    v->info_ptr = png_create_info_struct(v->png_ptr);
    if (!v->info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&v->png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return CFAILED;
    }

    // create png info struct
    v->end_info = png_create_info_struct(v->png_ptr);
    if (!v->end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&v->png_ptr, &v->info_ptr, (png_infopp) NULL);
        fclose(fp);
        return CFAILED;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(v->png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&v->png_ptr, &v->info_ptr, &v->end_info);
        fclose(fp);
        return CFAILED;
    }

    // init png reading
    png_init_io(v->png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(v->png_ptr, 8);

    // read all the info up to the image data
    png_read_info(v->png_ptr, v->info_ptr);


    // get info about png
    png_get_IHDR(v->png_ptr, v->info_ptr, (unsigned int*)&v->width, (unsigned int*)&v->height, &v->bit_depth, &v->color_type,
        NULL, NULL, NULL);

    // Update the png info struct.
    png_read_update_info(v->png_ptr, v->info_ptr);

    // Row size in bytes.
    v->rowbytes = png_get_rowbytes(v->png_ptr, v->info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    v->rowbytes += 3 - ((v->rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    v->image_data = malloc(v->rowbytes * v->height * sizeof(png_byte)+15);
    if (!v->image_data)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&v->png_ptr, &v->info_ptr, &v->end_info);
        fclose(fp);
        return CFAILED;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    v->row_pointers = malloc(v->height * sizeof(png_bytep));
    if (v->row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&v->png_ptr, &v->info_ptr, &v->end_info);
        free(v->image_data);
        fclose(fp);
        return CFAILED;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (i = 0; i < v->height; i++)
    {
        v->row_pointers[i] = v->image_data + (i * v->rowbytes);
    }



    // read the png into image_data through row_pointers
    png_read_image(v->png_ptr, v->row_pointers);

    s = v->width * v->height * 4;
    v->argb = (unsigned char*)malloc(s);

    if(png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_RGB && v->bit_depth == 8)
    {
/*        int i;
        for(i=0; i < s; i++)
        {
            v->argb[i*4] = 0xff;
            v->argb[i*4+1] = (unsigned char)v->image_data[i*3+0];
            v->argb[i*4+2] = (unsigned char)v->image_data[i*3+1];
            v->argb[i*4+3] = (unsigned char)v->image_data[i*3+2];
        }
*/

        for(int i=0; i < s; i+=4)
        {
            int j = i/4*3;
            v->argb[i] = 0xff;
            v->argb[i+1] = v->image_data[j+1];
            v->argb[i+2] = v->image_data[j];
            v->argb[i+3] = v->image_data[j+2];
        }
    }
    else if(png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_RGBA && v->bit_depth == 8)
    {
        /*int i;
        for(i=0; i < s; i++)
        {
            v->argb[i*4] = (unsigned char)v->image_data[i*4+3];
            v->argb[i*4+1] = (unsigned char)v->image_data[i*4+0];
            v->argb[i*4+2] = (unsigned char)v->image_data[i*4+1];
            v->argb[i*4+3] = (unsigned char)v->image_data[i*4+2];
        }
        */
        for(int i=0; i < s; i+=4)
        {
            v->argb[i] = v->image_data[i+3];
            v->argb[i+1] = v->image_data[i+1];
            v->argb[i+2] = v->image_data[i];
            v->argb[i+3] = v->image_data[i+2];
        }
    }
    else
        return CFAILED;
    // Generate the OpenGL texture object
    return CSUCCESS;
}

unsigned char *pngformat_getdata(PNGFORMAT *v) {
    return v->argb;
}

int pngformat_getwidth(PNGFORMAT *v) {
    return v->width;
}

int pngformat_getheight(PNGFORMAT *v) {
    return v->height;
}
