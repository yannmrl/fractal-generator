#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <png.h>
#include <string.h>
#include <stdbool.h>




/* A coloured pixel. */

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}
pixel_t;

/* A picture. */

typedef struct
{
	pixel_t *pixels;
	size_t width;
	size_t height;
}
bitmap_t;

/* Given "bitmap", this returns the pixel of bitmap at the point
	("x", "y").
*/

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
	return bitmap->pixels + bitmap->width * y + x;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
	success, non-zero on error.
*/

static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
	FILE * fp;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_byte ** row_pointers = NULL;
	/* "status" contains the return value of this function. At first
		it is set to a value which means 'failure'. When the routine
		has finished its work, it is set to a value which means
		'success'.
	*/
	int status = -1;
	/* The following number is set by trial and error only. I cannot
		see where it it is documented in the libpng manual.
	*/
	int pixel_size = 3;
	int depth = 8;

	fp = fopen (path, "wb");
	if (! fp) {
		goto fopen_failed;
	}

	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		goto png_create_write_struct_failed;
	}

	info_ptr = png_create_info_struct (png_ptr);
	if (info_ptr == NULL) {
		goto png_create_info_struct_failed;
	}

	/* Set up error handling. */

	if (setjmp (png_jmpbuf (png_ptr))) {
		goto png_failure;
	}

	/* Set image attributes. */

	png_set_IHDR (png_ptr,info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

	/* Initialize rows of PNG. */

	row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
	for (y = 0; y < bitmap->height; y++) {
		png_byte *row = png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
		row_pointers[y] = row;
		for (x = 0; x < bitmap->width; x++) {
			pixel_t * pixel = pixel_at (bitmap, x, y);
			*row++ = pixel->red;
			*row++ = pixel->green;
			*row++ = pixel->blue;
		}
	}

	/* Write the image data to "fp". */

	png_init_io (png_ptr, fp);
	png_set_rows (png_ptr, info_ptr, row_pointers);
	png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* The routine has successfully written the file, so we set
		"status" to a value which indicates success.
	*/

	status = 0;

	for (y = 0; y < bitmap->height; y++) {
		png_free (png_ptr, row_pointers[y]);
	}
	png_free (png_ptr, row_pointers);

	png_failure:
	png_create_info_struct_failed:
		png_destroy_write_struct (&png_ptr, &info_ptr);
	png_create_write_struct_failed:
		fclose (fp);
	fopen_failed:
		return status;
}

/* Given "value" and "max", the maximum value which we expect "value"
	to take, this returns an integer between 0 and 255 proportional to
	"value" divided by "max".
*/

static int pix (int value, int max)
{
	if (value < 0) {
		return 0;
	}
	return (int) (256.0 *((double) (value)/(double) max));
}



///////////////////////////////////////////////////////////////////////////////



double complex f(double complex z, double complex c, int p)
{
	double complex z2=z;
	for (int i = 0; i < p; i++)
	{
		z2 = z2*z;
	}
	return z2+c;
}

int amp(int i)
{
	return i;
}

double module(double complex z)
{
	return creall(z)*creall(z)+cimagl(z)*cimagl(z);
}

void make_fractal(int* fractal, int n, int iteration_max, int p, double complex c)
{
	double A, B;
	double x, y, x0, y0;
	double complex z, z0;
	int i = 0;
	int j = 0;


	for (B = 0; B <= 4*n; B++)
	{
		y0 = 2-B/n;

		x = 0;
		y = 0;
		for (A = 0; A <= 4*n; A++)
		{
			x0 = -2+A/n;

			z0 = x0+y0*I;
			z = z0;

			while (module(z) < 4 && i < iteration_max)
			{
				z = f(z, c, p);
				i++;
			}

			fractal[j] = amp(i);
//			printf("%d", i);
			j++;
			i = 0;
		}

//		printf("\n");
	}

}

int* make_color(int val, int max)
{
	static int c[3];

//	max = amp(max);
	int r, g, b;
	int ic = max/6;
	int sixth = val/ic;
	int m = 255;

	switch(sixth)
	{
		case 0: r=m; g=val*m/ic; b=0; break;
		case 1: r=m-(val-ic)*m/ic; g=m; b=0; break;
		case 2: r=0; g=m; b=(val-2*ic)*m/ic; break;
		case 3: r=0; g=m-(val-3*ic)*m/ic; b=m; break;
		case 4: r=(val-4*ic)*m/ic; g=0; b=m; break;
		case 5: r=255; g=0; b=m-(val-5*ic)*m/ic; break;
		case 6: r=255; g=0; b=0; break;
	}

//	printf("%d/%d, (%d, %d, %d) s%d ic%d\n", val, max, r, g, b, sixth, ic);
	c[0] = r; c[1] = g; c[2] = b;
	return c;
}

void make_png(int n, int iteration_max, int p, double complex c, int no)
{
	int* fractal = malloc((4*n+1)*(4*n+1)*sizeof(int));
	make_fractal(fractal, n, iteration_max, p, c);

	int i = 0;

	bitmap_t fract;
	int x, y;
	int status = 0;

	int* color;

	fract.width = 4*n+1;
	fract.height = 4*n+1;

	fract.pixels = calloc(fract.width * fract.height, sizeof(pixel_t));

	for (y = 0; y < fract.height; y++)
	{
		for (x = 0; x < fract.width; x++)
		{
			pixel_t * pixel = pixel_at (&fract, x, y);

			i = fractal[y*fract.height+x];
			color = make_color(i, iteration_max);
//			printf("%d/%d, (%d, %d, %d)\n", i, iteration_max, color[0], color[1], color[2]);
			pixel->blue = color[0]/2;
			pixel->green = color[1];
			pixel->red = color[2];
		}
	}

	/* Write the image to a file 'fract.png'. */

	char str[8];
	sprintf(str, "f%d.png", no);
	if (save_png_to_file(&fract, str));
	{
//		fprintf(stderr, "Error writing file.\n");
		status = -1;
	}


	free(fract.pixels);
	free(fractal);
}

void main(int argc, char* argv[])
{
	if (argc != 6)
		printf("Usage : <n> <iteration max> <puissance> <Re(c)> <Im(c)>\n");
	else
	{
		int n = atoi(argv[1]);
		int iteration_max = atoi(argv[2]);
		iteration_max = iteration_max+(6-iteration_max%6);
		int p = atoi(argv[3]);
		double complex c = atof(argv[4])+I*atof(argv[5]);

		make_png(n, iteration_max, p, c, 0);
		system("display f0.png");


//		GENERATING SEVERAL IMAGES WITH A DIFFERENT VALUE OF i
/*		for (int i = 10; i < iteration_max; i+=10)
		{
			make_png(n, i, p, c, i);
		}
*/

//		GENERATING A VIDEO FROM THE IMAGES GENERATED
/*		if (nombre_d_images == 1)
			system("display f0.png");
		else
		{
			system("rm fractal.mp4");
			system("ffmpeg -framerate 30 -pattern_type glob -i 'f*.png' fractal.mp4");
//			system("rm f*.png");
			printf("video de %d images générée\n", nombre_d_images);
			system("ffplay fractal.mp4");
		}
*/	}
}
