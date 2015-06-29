#include <stdio.h>
#include <windows.h>
static int cmp_li(const void *v1, const void *v2) {
	return ((LARGE_INTEGER*)v1)->QuadPart - ((LARGE_INTEGER*)v2)->QuadPart;
}
static LONGLONG avg(const LARGE_INTEGER * const i, const int m) {
	int n = m;
	LONGLONG r = 0;
	while (n--) {
		r += i[n].QuadPart;
	}
	return r / m;
}
static void extract(const LARGE_INTEGER * const t1, const int row, const LARGE_INTEGER * const base, const int max) {
	LARGE_INTEGER *t = malloc(sizeof(LARGE_INTEGER) * max);
	int i, from, to, ti;
	for (i = 0; i < row * row; i++) {
		from = 0;
		ti = 0;
		while (from < max) {
			if (!t1[from].QuadPart) { from++; continue; }
			to = from + 1;
			while (to < max) {
				if (!t1[to].QuadPart) { to++; continue; }
				if (((from % row) * row + (to % row)) != i) break;
				t[ti++].QuadPart = t1[to].QuadPart - t1[from].QuadPart;
				break;
			}
			from = to;
		}
		if (!ti) continue;
		qsort(t, ti, sizeof(LARGE_INTEGER), cmp_li);
		printf("%2d to %2d %8d %12f %12f %12f %12f\n", i / row, i % row, ti,
			(double) t[0].QuadPart / base->QuadPart,
			(double) t[ti - 1].QuadPart / base->QuadPart,
			(double) avg(t, ti) / base->QuadPart,
			(double) t[ti / 2].QuadPart / base->QuadPart);
	}
	printf("\n");
	for (i = 0; i < row; i++) {
		from = i;
		ti = 0;
		while (from < max) {
			if (!t1[from].QuadPart) { from += row; continue; }
			to = from + row;
			while (to < max) {
				if (!t1[to].QuadPart) { to += row; continue; }
				t[ti++].QuadPart = t1[to].QuadPart - t1[from].QuadPart;
				break;
			}
			from = to;
		}
		if (!ti) continue;
		qsort(t, ti, sizeof(LARGE_INTEGER), cmp_li);
		printf("p2p   %2d %8d %12f %12f %12f %12f\n", i, ti,
			(double) t[0].QuadPart / base->QuadPart,
			(double) t[ti - 1].QuadPart / base->QuadPart,
			(double) avg(t, ti) / base->QuadPart,
			(double) t[ti / 2].QuadPart / base->QuadPart);
	}
	free(t);
}
int main(int argc, char **argv) {
	if (argc != 2) return 1;
	LARGE_INTEGER base;
	LARGE_INTEGER *t1;
	FILE *f = fopen(argv[1], "rb");
	int row = 0, max = 0;
	fread(&base, 1, sizeof(LARGE_INTEGER), f);
	fread(&row, 1, 1, f);
	fread(&max, 1, 4, f);
	t1 = malloc(max);
	fread(t1, 1, max, f);
	base.QuadPart /= 1000;
	fclose(f);
	extract(t1, row, &base, max / sizeof(LARGE_INTEGER) / row);
	free(t1);
	return 0;
}
