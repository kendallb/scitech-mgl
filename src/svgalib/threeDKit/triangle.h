
#define max(x,y)     (((x) > (y)) ? (x) : (y))
#define min(x,y)     (((x) < (y)) ? (x) : (y))

#define TRIANGLE_COLOR_LOOKUP_TABLE_SIZE 4096

/* triangle interpolation definitions: */
typedef struct {
	unsigned char *bitmap1;
	unsigned char *bitmap2;
	int bf;
} TD_tridata;

void gl_triangle (int x0, int y0, int z0, int x1, int y1, int z1, int x2, int y2, int z2, int bf);
void gl_wtriangle (int x0, int y0, int xd0, int yd0, int z0, \
		   int x1, int y1, int xd1, int yd1, int z1, \
		   int x2, int y2, int xd2, int yd2, int z2, \
		   TD_tridata * tri);	/* This does not alter tri structure. */
void gl_swtriangle (int x0, int y0, int xd0, int yd0, \
		    int x1, int y1, int xd1, int yd1, \
		    int x2, int y2, int xd2, int yd2, int c, \
		    TD_tridata * tri);	/* This does not alter tri structure. */

void gl_striangle (int x0, int y0, int x1, int y1, int x2, int y2, int color, int bf);

void gl_trisetcolorlookup (int i, long c);
long gl_trigetcolorlookup (int i);

void gl_trisetdrawpoint (void (setpixelfunc) (int, int, int));
