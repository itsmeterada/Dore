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
 
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "mtn.h"

FractalHeader Init = {
	 5.00, 8.660254,
	 0.00, 0.000000,
	10.00, 0.000000,
	0, 0, FRACTALMAGIC, 0
};

double InitZ[] = { 0.0, 0.0, 0.0 };

double
drand()
{
	unsigned long random();

	return ((random() & 0xffff) / (double) 0xffff - 0.5);
}

double Max;

double *
color(z)
	double z;
{
	static double green[] = { 0.0, 1.0, 0.0 };
	static double white[] = { 1.0, 1.0, 1.0 };
	static double brown[] = { 0.6, 0.4, 0.1 };

	if (z >= 0.06 * Max)
		return (white);
	if (z <= 0.0)
		return (green);

	return (brown);
}

void
writez(f, z, n)
	FILE *f;
	double *z;
	int n;
{
	Init.zcount  = n;
	Init.flags  &= ~F_COLOR;

	fwrite((char *) &Init, sizeof(Init), 1, f);
	fwrite((char *) z, sizeof(double), n, f);
}

void
writezc(f, z, n)
	FILE *f;
	double *z;
	int n;
{
	Init.zcount  = n;
	Init.flags  |= F_COLOR;

	fwrite((char *) &Init, sizeof(Init), 1, f);

	while (n--) {
		fwrite((char *) z, sizeof(double), 1, f);
		fwrite((char *) color(*z), sizeof(double), 3, f);
		++z;
	}
}

/*  Write out the lattice so it can be read back using -g.
 *  Each z value is written in plain text so it can be edited.
 */

void
writev(f, v, n)
	FILE *f;
	double *v;
	int n;
{
	int i;

	fprintf(f, "%d\n", Init.gen);
	fprintf(f, "%0.10e,%0.10e\n", Init.x0, Init.y0);
	fprintf(f, "%0.10e,%0.10e\n", Init.x1, Init.y1);
	fprintf(f, "%0.10e,%0.10e\n", Init.x2, Init.y2);

	for (i = 0; i < n; ++i, ++v)
		fprintf(f, "%d:%0.10e\n", i, *v);
}

/*  Read in the header and z-list information from a specified file.  If
 *  successful, return the generation count of the file; otherwise, return
 *  -1 to indicate failure
 */

int
readv(nm, f, dp, g)
	char *nm, *f;
	double *dp;
	int g;
{
	FILE *fp;
	struct stat sb;
	FractalHeader tmp;
	unsigned long nz;

	if ((fp = fopen(f, "r")) == NULL) {
		fprintf(stderr, "%s: Cannot open '%s'\n", nm, f);
		return (-1);
	}

	if (fstat(fileno(fp), &sb) != 0)
		fprintf(stderr, "%s: Cannot stat '%s'\n", nm, f);
	else if ((sb.st_mode & S_IFMT) != S_IFREG)
		fprintf(stderr, "%s: '%s' must be a regular file\n", nm, f);
	else if (fscanf(fp, "%d\n%le,%le\n %le,%le\n%le,%le\n", &tmp.gen,
		&tmp.x0, &tmp.y0, &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2) != 7)
		fprintf(stderr, "%s: header read error\n", nm);
	else if ((nz = gencount(tmp.gen)) == 0)
		fprintf(stderr, "%s: invalid generation count\n", nm);
	else if (tmp.gen > g)
		fprintf(stderr,
			"%s: initial generation count (%d) > total (%d)\n",
							nm, tmp.gen, g);
	else {
		int i, n;

		for (i = 0; i < nz; ++i, ++dp)
			if (fscanf(fp, "%d:%le\n", &n, dp) != 2 || i != n) {
				fprintf(stderr, "%s: data read error\n", nm);
				break;
			}

		if (i == nz) {
			fclose(fp);
			tmp.magic = FRACTALMAGIC;
			tmp.zcount = 0;
			tmp.flags  = 0;
			Init = tmp;
			return (tmp.gen);
		}
	}

	fclose(fp);
	return (-1);
}

double
dist(x0, y0, z0, x1, y1, z1)
	double x0, y0, z0, x1, y1, z1;
{
	double sqrt();

	x0 -= x1;
	y0 -= y1;
	z0 -= z1;

	return (sqrt(x0 * x0 + y0 * y0 + z0 * z0));
}

double
interp(z0, z1, len, sc)
	double z0, z1, len, sc;
{
#ifdef	DEBUG
	printf("interp(%0.4e, %0.4e, %0.4e, %0.4e)\n", z0, z1, len, sc);
	return (0.0);
#else	!DEBUG
	return ((z0 + z1) * 0.5 + drand() * len * sc);
#endif	!DEBUG
}

void
fillin(Z, x, y, size, ll, lr, li, sc, skip)
	double *Z;
	int x, y, size;
	double ll, lr, li, sc;
	int skip;
{
	int hsize = size >> 1;
	int xnode = XNODE(x);
	int xsnode = XNODE(x + size);
	int xhnode = XNODE(x + hsize);

	double sqrt();

	if (skip <= 0 && size > 1) {
		double z0, z1, z2;

		z0 = Z[xnode + y];
		z1 = Z[xsnode + y];
		z2 = Z[xsnode + y + size];

		Z[xhnode + y] = interp(z0, z1, ll, sc);
		Z[xhnode + y + hsize] = interp(z0, z2, lr, sc);
		Z[xsnode + y + hsize] = interp(z1, z2, li, sc);
	}

	if (abs(size) > 2) {
		--skip;

		ll *= 0.5;
		lr *= 0.5;
		li *= 0.5;

		if (size < -2)
			fillin(Z, x + size,  y + hsize, -hsize,
						ll, lr, li, sc, skip);

		fillin(Z, x,         y,         hsize,  ll, lr, li, sc, skip);
		fillin(Z, x + hsize, y,         hsize,  ll, lr, li, sc, skip);
		fillin(Z, x + hsize, y + hsize, hsize,  ll, lr, li, sc, skip);

		if (size > 2)
			fillin(Z, x + size,  y + hsize, -hsize,
							ll, lr, li, sc, skip);
	}
}

void
distribute(ip, g0, g1)
	double *ip;
	int g0, g1;
{
	if (g0 < g1) {
		int c0 = 1 << g0;
		int x0 = XY2NODE(c0, c0);
		int d  = 1 << (g1 - g0);
		int c1;

		for (c1 = 1 << g1; c1 > 0; c1 -= d) {
			int l = XNODE(c1);
			int x1;

			for (x1 = XY2NODE(c1, c1); x1 >= l; --x0, x1 -= d)
				ip[x1] = ip[x0];
		}
	}
}

double LLength;
double RLength;
double ILength;
double Scale;

void
initlens()
{
	LLength = dist(Init.x0, Init.y0, 0.0, Init.x1, Init.y1, 0.0);
	RLength = dist(Init.x0, Init.y0, 0.0, Init.x2, Init.y2, 0.0);
	ILength = dist(Init.x1, Init.y1, 0.0, Init.x2, Init.y2, 0.0);

	Max = (LLength + RLength + ILength) / 3.0;
}

usage(s)
	char *s;
{
	char *m = "usage: %s [-i #] [-s #] [-g file] [-n] [-v | -m] count\n";

	fprintf(stderr, m, s);
	exit(1);
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int i, g, Colored, VertOnly;
	char *nm, *cp;
	long Seed, time();
	double *ZList, *sbrk(), atof();

	Scale    = 1.0;
	Colored  = 1;
	VertOnly = 0;
	Seed     = time(0);

	/*  Process arguments for controlling parameters:
	 *
	 *	-i #		Use the integer # as the initial seed
	 *			instead of the time() function
	 *
	 *	-s #		Set the global scale to # (default = 1.0)
	 *
	 *	-g file		Read the initial geometry from file.
	 *			The file in the same format as a non-color
	 *			file generated by this program with -v
	 *
	 *	-n		Suppress color verticies
	 *
	 *	-v		Write the resulting lattice in left-right
	 *			format so it can be read using -g
	 */

	for (nm = cp = argv[0]; i = *cp; ++cp)
		if (i == '/')
			nm = cp + 1;

	if (*nm == '\0')
		nm = argv[0];

	cp = NULL;

	for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv)
		switch (argv[0][1]) {
		case 'i':
			if (argc < 2)
				usage(nm);

			Seed = atoi(argv[1]);
			--argc; ++argv;
			break;

		case 's':
			if (argc < 2)
				usage(nm);

			Scale = atof(argv[1]);
			--argc; ++argv;
			break;

		case 'g':
			if (argc < 2 || cp)
				usage(nm);

			cp = argv[1];
			--argc; ++argv;
			break;

		case 'v':
			VertOnly = 1;
			break;

		case 'n':
			Colored = 0;
			break;

		default:
			usage(nm);
		}

	if (argc != 1)
		usage(nm);

	if ((g = atoi(argv[0])) < 0 || gencount(g) == 0) {
		fprintf(stderr, "%s: invalid generation count (%d)\n", nm, g);
		return (2);
	}

	if ((ZList = sbrk(gencount(g) * sizeof(double))) == (double *) -1) {
		fprintf(stderr, "%s: Cannot allocate %u bytes for z list\n",
							nm, gencount(g));
		return (2);
	}

	if (cp == NULL)
		bcopy(InitZ, ZList, sizeof(InitZ));
	else if (readv(nm, cp, ZList, g) < 0)
		return (2);

	if (g < Init.gen) {
		fprintf(stderr,
			"%s: initial generation count (%d) > total (%d)\n",
							nm, Init.gen, g);
		return (2);
	}

	distribute(ZList, Init.gen, g);
	srandom(Seed);
	initlens();

	fprintf(stderr, "Fractal Parameters:\n");
	fprintf(stderr, "\t%d Generations (Initial = %d)\n", g, Init.gen);
	fprintf(stderr, "\tRandom seed = %u\n", Seed);
	fprintf(stderr, "\tPerturbation Scale = %0.2f\n", Scale);
	fprintf(stderr, "\tInitial Lengths = <%0.2f, %0.2f, %0.2f>\n",
						LLength, RLength, ILength);

	fillin(ZList, 0, 0, 1 << g,
			LLength, RLength, ILength, Scale, Init.gen);

	Init.gen = g;

	if (VertOnly)
		writev(stdout, ZList, gencount(g));
	else if (Colored)
		writezc(stdout, ZList, gencount(g));
	else
		writez(stdout, ZList, gencount(g));

	return (0);
}
