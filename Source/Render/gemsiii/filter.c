/*
 *		Filtered Image Rescaling
 *
 *		  by Dale Schumacher
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "filter.h"

#ifndef M_PI
#define M_PI  3.14159265358979323846f
#endif

#ifndef EXIT_SUCCESS
#define	EXIT_SUCCESS	(0)
#define	EXIT_FAILURE	(1)
#endif

typedef struct {
	int	xsize;		/* horizontal size of the image in Pixels */
	int	ysize;		/* vertical size of the image in Pixels */
	Pixel *	data;		/* pointer to first scanline of image */
	int	span;		/* byte offset between two scanlines */
} Image;

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)

#define CLAMP(x,y,z) (x<y?y:((x>z)?z:x))


Image *
new_image(xsize, ysize)	/* create a blank image */
int xsize, ysize;
{
	Image *image;

	if((image = (Image *)malloc(sizeof(Image)))
	&& (image->data = (Pixel *)calloc(ysize, xsize))) {
		image->xsize = xsize;
		image->ysize = ysize;
		image->span = xsize;
	}
	return(image);
}

void
free_image(image)
Image *image;
{
	free(image->data);
	free(image);
}


void
get_row(row, image, y)
Pixel *row;
Image *image;
int y;
{
	if((y < 0) || (y >= image->ysize)) {
		return;
	}
	memcpy(row,
		image->data + (y * image->span),
		(sizeof(Pixel) * image->xsize));
}

void
get_column(column, image, x)
Pixel *column;
Image *image;
int x;
{
	int i, d;
	Pixel *p;

	if((x < 0) || (x >= image->xsize)) {
		return;
	}
	d = image->span;
	for(i = image->ysize, p = image->data + x; i-- > 0; p += d) {
		*column++ = *p;
	}
}

Pixel
put_pixel(image, x, y, data)
Image *image;
int x, y;
Pixel data;
{
	static Image *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x] = data);
}


/*
 *	filter function definitions
 */

#define	filter_support		(1.0)

double
filter(t)
double t;
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

#define	box_support		(0.5)

double
box_filter(t)
double t;
{
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

#define	triangle_support	(1.0)

double
triangle_filter(t)
double t;
{
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#define	bell_support		(1.5)

double
bell_filter(t)		/* box (*) box (*) box */
double t;
{
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

#define	B_spline_support	(2.0)

double
B_spline_filter(t)	/* box (*) box (*) box (*) box */
double t;
{
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

double
sinc(x)
double x;
{
	x *= M_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)

double
Lanczos3_filter(t)
double t;
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double
Mitchell_filter(t)
double t;
{
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

void
zoom(dst, src, filterf, fwidth)
Image *dst;				/* destination image structure */
Image *src;				/* source image structure */
double (*filterf)();			/* filter function */
double fwidth;				/* filter width (support) */
{
	Image *tmp;			/* intermediate image */
	double xscale, yscale;		/* zoom scale factors */
	int i, j, k;			/* loop variables */
	int n;				/* pixel number */
	double center;	/* filter calculation variables */
	int left, right;
	double width, fscale, weight;	/* filter calculation variables */
	Pixel *raster;			/* a row or column of pixels */

	/* create intermediate image to hold horizontal zoom */
	tmp = new_image(dst->xsize, src->ysize);
	xscale = (double) dst->xsize / (double) src->xsize;
	yscale = (double) dst->ysize / (double) src->ysize;

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)calloc(dst->xsize, sizeof(CLIST));
	if(xscale < 1.0) {
		width = fwidth / xscale;
		fscale = 1.0 / xscale;
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / xscale;
			left = (int)ceil(center - width);
			right = (int)floor(center + width);
			for(j = left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / xscale;
			left = (int)ceil(center - fwidth);
			right = (int)floor(center + fwidth);
			for(j = left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	raster = (Pixel *)calloc(src->xsize, sizeof(Pixel));
	for(k = 0; k < tmp->ysize; ++k) {
		get_row(raster, src, k);
		for(i = 0; i < tmp->xsize; ++i) {
			weight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
					* contrib[i].p[j].weight;
			}
			put_pixel(tmp, i, k,
				(Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
		}
	}
	free(raster);

	/* free the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp->xsize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)calloc(dst->ysize, sizeof(CLIST));
	if(yscale < 1.0) {
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / yscale;
			left = (int)ceil(center - width);
			right = (int)floor(center + width);
			for(j = left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / yscale;
			left = (int)ceil(center - fwidth);
			right = (int)floor(center + fwidth);
			for(j = left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel *)calloc(tmp->ysize, sizeof(Pixel));
	for(k = 0; k < dst->xsize; ++k) {
		get_column(raster, tmp, k);
		for(i = 0; i < dst->ysize; ++i) {
			weight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
					* contrib[i].p[j].weight;
			}
			put_pixel(dst, k, i,
				(Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
		}
	}
	free(raster);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	free_image(tmp);
}


int resample(image_in,dx_in,dy_in,image_out,dx_out,dy_out,c)
Pixel* image_in;
int dx_in;
int dy_in;
Pixel* image_out;
int dx_out;
int dy_out;
char c;
{
	double (*f)() = filter;
	double s = filter_support;
	Image dst,src;

	switch(c)
	{
	case 'b': f=box_filter; s=box_support; break;
	case 't': f=triangle_filter; s=triangle_support; break;
	case 'q': f=bell_filter; s=bell_support; break;
	case 'B': f=B_spline_filter; s=B_spline_support; break;
	case 'h': f=filter; s=filter_support; break;
	case 'l': f=Lanczos3_filter; s=Lanczos3_support; break;
	case 'm': f=Mitchell_filter; s=Mitchell_support; break;
	default:
		return 0;
	}

	src.xsize=dx_in;
	src.ysize=dy_in;
	src.data=image_in;
	src.span=dx_in;

	dst.xsize=dx_out;
	dst.ysize=dy_out;
	dst.data=image_out;
	dst.span=dx_out;

	zoom(&dst, &src, f, s);
	return 1;
}

