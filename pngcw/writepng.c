/*
 * Copyright 2002-2008 Guillaume Cottenceau, 2015 Aleksander Denisiuk
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>


#define OUT_FILE "initials.png"
#define WIDTH 600
#define HEIGHT 600
#define COLOR_TYPE PNG_COLOR_TYPE_RGB
#define BIT_DEPTH 8


void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void create_png_file()
{
	width = WIDTH;
	height = HEIGHT;
        bit_depth = BIT_DEPTH;
        color_type = COLOR_TYPE;

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) malloc(width*bit_depth*3);


}


void write_png_file(char* file_name)
{
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height,
		     bit_depth, color_type, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
	for (y=0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);

        fclose(fp);
}

void write_pixel(int x, int y, png_byte cr, png_byte cg, png_byte cb)
{
	png_byte* row = row_pointers[y];
	png_byte* ptr = &(row[x*3]);
	ptr[0] = cr;
	ptr[1] = cg;
	ptr[2] = cb;
}

void bresenham(int i1, int j1, int i2, int j2,
				png_byte cr, png_byte cg, png_byte cb)
{
	// int i, j, m, b, P;
	// if( i2>i1 && j2>=j1 && j2-j1<=i2-i1 ){
	// 	printf("pierwszy przypadek \n");
	// 	m = 2*(j2-j1);
	// 	b = 0;
	// 	write_pixel(i1, j1, cr, cg, cb);
	// 	j = j1;
	// 	P = i2-i1;
	// 	for(i=i1+1; i<=i2; i++){
	// 		b += m;
	// 		if(b>P){
	// 			j += 1;
	// 			b -= 2*P;
	// 		}
	// 		write_pixel(i, j, cr, cg, cb);
	// 	}
	// }
	// else if( i2>i1 && -j2>=-j1 && -j2+j1<=i2-i1 ){
	// 	printf("drugi przypadek \n");
	// 	m = 2*(-j2+j1);
	// 	b = 0;
	// 	write_pixel(i1, j1, cr, cg, cb);
	// 	j = j1;
	// 	P = i2-i1;
	// 	for(i=i1+1; i<=i2; i++){
	// 		b += m;
	// 		if(b>P){
	// 			j += 1;
	// 			b -= 2*P;
	// 		}
	// 		write_pixel(i, j, cr, cg, cb);
	// 	}
	// }
	// else{
	// 	printf("inny przypadek \n");
	// }

	int d, run, rise, ai, bi, ip, jp;
	int i = i1, j = j1;

	if(i1<i2){
		ip = 1;
		run = i2 - i1;
	}
	else{
		ip = -1;
		run = i1 - i2;
	}

	if(j1<j2){
		jp = 1;
		rise = j2 - j1;
	}
	else{
		jp = -1;
		rise = j1 - j2;
	}
	write_pixel(i, j, cr, cg, cb);
	if(run>rise){
		ai = (rise - run)*2;
		bi = rise*2;
		d = bi - run;
		while (i != i2)
		{
			if(d>=0){
				i += ip;
				j += jp;
				d += ai;
			}
			else{
				d += bi;
				i += ip;
			}
			write_pixel(i, j, cr, cg, cb);
		}
	}
	else{
		ai = (run - rise)*2;
		bi = run*2;
		d = bi - rise;
		while (j != j2)
		{
			if(d>=0){
				i += ip;
				j += jp;
				d += ai;
			}
			else{
				d += bi;
				j += jp;
			}
			write_pixel(i, j, cr, cg, cb);
		}
	}
}

void write_pixel4(int x, int y, png_byte cr, png_byte cg, png_byte cb)
{
	write_pixel(x+WIDTH/2, y+HEIGHT/2, cr, cg, cb);
	write_pixel(x+WIDTH/2, -y+HEIGHT/2, cr, cg, cb);
	write_pixel(-x+WIDTH/2, y+HEIGHT/2, cr, cg, cb);
	write_pixel(-x+WIDTH/2, -y+HEIGHT/2, cr, cg, cb);
	/*write_pixel(y+WIDTH/2, x+HEIGHT/2, cr, cg, cb);
	write_pixel(-y+WIDTH/2, x+HEIGHT/2, cr, cg, cb);
	write_pixel(y+WIDTH/2, -x+HEIGHT/2, cr, cg, cb);
	write_pixel(-y+WIDTH/2, -x+HEIGHT/2, cr, cg, cb);*/
}

void circle(int R, png_byte cr, png_byte cg, png_byte cb)
{
	int i, j, f;
	i = 0;
	j = R;
	f = 5-4*R;
	write_pixel4(i, j, cr, cg, cb);
	while (i<j)
	{
		if (f>0)
		{
			f = f+8*i-8*j+20;
			j = j-1;
		}
		else{
			f = f+8*i+12;
		}
		i = i+1;
		write_pixel4(i, j, cr, cg, cb);
	}
}

void ellipse(int a, int b, png_byte cr, png_byte cg, png_byte cb)
{
	int i, j, f, g;
	i = 0;
	j = b;
	f = 4*(b*b)-4*(a*a)*b+(a*a);
	write_pixel4(i, j, cr, cg, cb);
	while (b*b*i<a*a*j)
	{
		if (f>0)
		{
			f = f+8*(b*b)*i-8*(a*a)*j+12*(b*b)+8*(a*a);
			j = j-1;
		}
		else{
			f = f+8*(b*b)*i+12*b*b;
		}
		i = i+1;
		write_pixel4(i, j, cr, cg, cb);
	}
	g = f-4*(b*b)*i-3*(b*b)-4*(a*a)*j+3*a*a;
	while (j>0)
	{
		if (g<=0)
		{
			g = g+8*(b*b)*i-8*(a*a)*j+8*(b*b)+12*a*a;
			i = i+1;
		}
		else{
			g = g-8*(a*a)*j+12*a*a;
		}
		j = j-1;
		write_pixel4(i, j, cr, cg, cb);
	}
}

/* void colorfill(int i, int j, png_byte cr1, png_byte cg1,
png_byte cb1, png_byte cr2, png_byte cg2, png_byte cb2)
{
    if (!write_pixel(i, j, cr2, cg2, cb2)){
    write_pixel(i, j, cr1, cg1, cb1);
    colorfill(i-1, j, cr1, cg1, cb1, cr2, cg2, cb2);
    colorfill(i, j-1, cr1, cg1, cb1, cr2, cg2, cb2);
    colorfill(i+1, j, cr1, cg1, cb1, cr2, cg2, cb2);
    colorfill(i, j+1, cr1, cg1, cb1, cr2, cg2, cb2);
    }
} */

void process_file(void)
{
	for (y=0; y<height; y++) {
		png_byte* row = row_pointers[y];
		for (x=0; x<width; x++) {
			png_byte* ptr = &(row[x*3]);
			ptr[0] = 0;
			ptr[1] = ptr[2] = 255;
		}
	}/* 165, 230 */

	bresenham(175, 240, 235, 280, 0, 0, 0);
	bresenham(235, 280, 295, 240, 0, 0, 0);
	bresenham(295, 240, 295, 360, 0, 0, 0);
	bresenham(295, 360, 275, 360, 0, 0, 0);
	bresenham(275, 360, 275, 280, 0, 0, 0);
	bresenham(275, 280, 235, 300, 0, 0, 0);
	bresenham(235, 300, 195, 280, 0, 0, 0);
	bresenham(195, 280, 195, 360, 0, 0, 0);
	bresenham(195, 360, 175, 360, 0, 0, 0);
	bresenham(175, 360, 175, 240, 0, 0, 0);

	bresenham(305, 240, 405, 240, 0, 0, 0);
	bresenham(405, 240, 405, 270, 0, 0, 0);
	bresenham(405, 270, 385, 270, 0, 0, 0);
	bresenham(385, 270, 385, 260, 0, 0, 0);
	bresenham(385, 260, 325, 260, 0, 0, 0);
	bresenham(325, 260, 325, 340, 0, 0, 0);
	bresenham(325, 340, 385, 340, 0, 0, 0);
	bresenham(385, 340, 385, 320, 0, 0, 0);
	bresenham(385, 320, 365, 320, 0, 0, 0);
	bresenham(365, 320, 365, 300, 0, 0, 0);
	bresenham(365, 300, 405, 300, 0, 0, 0);
	bresenham(405, 300, 405, 360, 0, 0, 0);
	bresenham(405, 360, 305, 360, 0, 0, 0);
	bresenham(305, 360, 305, 240, 0, 0, 0);
	ellipse(180, 120, 243, 233, 216);
	// colorfill(1, 1, 255, 0, 255, 243, 233, 216);
	// colorfill(90, 60, 243, 233, 216, 243, 233, 216);
	// colorfill(11, 11, 5, 255, 25, 243, 233, 216);
	// colorfill(141, 11, 5, 255, 25, 243, 233, 216);
}


int main(int argc, char **argv)
{
	create_png_file();
	process_file();
	write_png_file(OUT_FILE);

        return 0;
}
