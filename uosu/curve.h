#ifndef _UOSU_CURVE_H
#define _UOSU_CURVE_H
#define SLIDER_DETAIL_LEVEL 50
typedef struct {
	double x;
	double y;
} point;
point *pass   (unsigned int, point*, unsigned int*);
point *bezier (unsigned int, point*, unsigned int*);
point *linear (unsigned int, point*, unsigned int*);
point *catmull(unsigned int, point*, unsigned int*);
void uniform(unsigned int, point*, unsigned int, point*);
#endif
