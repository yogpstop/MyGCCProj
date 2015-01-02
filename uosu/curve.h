#ifndef _UOSU_CURVE_H
#define _UOSU_CURVE_H
#define SLIDER_DETAIL_LEVEL 50
typedef struct {
	double x;
	double y;
} point;
typedef struct {
	unsigned int /***/alen/*s*/;
	point *a;
	unsigned int /***/blen/*s*/;
	point *b;
} slider_side;
point *pass   (unsigned int, point*, unsigned int*);
point *bezier (unsigned int, point*, unsigned int*);
point *linear (unsigned int, point*, unsigned int*);
point *catmull(unsigned int, point*, unsigned int*);
void polygon(unsigned int, point*, double, unsigned int, slider_side*);
void uniform(unsigned int, point*, unsigned int, point*);
#endif
