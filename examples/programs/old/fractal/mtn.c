/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/
 
#include "mtn.h"
#include "dore.h"

/*  Compute the # of nodes in generation N.  Watch for numerical wrap around
 *  and return 0 if detected.
 */

unsigned long
gencount(N)
	int N;
{
	unsigned long x, y;

	if (N >= 0 && (x = 1 << N)) {
		++x;

		if ((y = x * x) >= x + x && (x += y) > y)
			return (x >> 1);
	}

	return (0);
}

/*  Generate an index list representing the triangle mesh for an Nth
 *  generation triangle-based mountain fractal.  Store the list in mp
 */

void
genmesh(N, mp)
	int N, mp[][3];
{
	int n, i, j, m;

#ifdef	DEBUG
	for (m = (1 << N) - 1, i = n = 0; i < m; ++i)
		for (j = 0; j <= i; ++n, ++j) {
			printf("%d, %d, %d\n", n, n + i + 1, n + i + 2);
			printf("%d, %d, %d\n",
					n + i + 1, n + i + i + 4, n + i + 2);
		}

	for (j = 0; j <= i; ++n, ++j)
		printf("%d, %d, %d\n", n, n + i + 1, n + i + 2);
#else	!DEBUG
	for (m = (1 << N) - 1, i = n = 0; i < m; ++i)
		for (j = 0; j <= i; ++n, ++j, mp += 2) {
			mp[0][0] = n;
			mp[0][1] = n + i + 1;
			mp[0][2] = n + i + 2;
			mp[1][0] = n + i + 1;
			mp[1][1] = n + i + i + 4;
			mp[1][2] = n + i + 2;
		}

	for (j = 0; j <= i; ++n, ++j, ++mp) {
		mp[0][0] = n;
		mp[0][1] = n + i + 1;
		mp[0][2] = n + i + 2;
	}
#endif	!DEBUG
}

dist_xy(h, z, xyz)
	FractalHeader *h;
	double *z;
	DtReal *xyz;
{
	int i, j, m = 1L << h->gen;

	double x0, y0, dx, dy, sx, sy;
	int color = h->flags & F_COLOR;

	dx = (h->x0 - h->x1) / m;
	dy = (h->y0 - h->y1) / m;

	sx = (h->x2 - h->x1) / m;
	sy = (h->y2 - h->y1) / m;

	x0 = h->x0;
	y0 = h->y0;

	for (i = 0; i <= m; ++i) {
		double x, y;

		for (x = x0, y = y0, j = 0; j <= i; ++j, ++z, xyz += 3) {
			xyz[0] = x;
			xyz[1] = y;
			xyz[2] = z[0];

			if (color) {
				xyz += 3;
				xyz[0] = z[1];
				xyz[1] = z[2];
				xyz[2] = z[3];
				z += 3;
			}

			x += sx;
			y += sy;
		}

		x0 -= dx;
		y0 -= dy;
	}
}
