#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Power of 2 
 */
#define POW2(_x) (_x * _x)
/*
 * Distance between 2 points squared
 */
#define DISTSQR(_a, _b) POW2(((Point *)_a)->x - (_b ? ((Point *)_b)->x : 0)) + POW2(((Point *)_a)->y - (_b ? ((Point *)_b)->y : 0))
/*
 * Distance between 2 points
 */
#define DISTANCE(_a, _b) (sqrtf(powf(((Point *)_a)->x - (_b ? ((Point *)_b)->x : 0), 2) + powf(((Point *)_a)->y - (_b ? ((Point *)_b)->y : 0), 2)))
/*
 * Angle orientation
 * 0: points are colinear
 */
#define CCW(_a, _b, _c) (_b->x - _a->x) * (_c->y - _a->y) - (_b->y - _a->y) * (_c->x - _a->x)

typedef struct _Point Point;
struct _Point
{
    float x, y, d;
    Point *next;
};

#define POINT_SQR(_p) (powf(_p->x, 2) + powf(_p->y, 2))
#define POINT_MAKE() (Point *)calloc(sizeof(Point), 1)
#define POINT_PRINT(_p) printf("%p -> x: %5.3f\ty: %5.3f\td: %5.3f\n", (void *)_p, _p->x, _p->y, _p->d)

typedef struct _Triangle Triangle;
struct _Triangle
{
    Point **point;
    Triangle *next;
    Triangle **neighbors;
};

#define TRIANGLE_MAKE() (Triangle *)calloc(sizeof(Triangle), 1)
#define TRIANGLE_PRINT(_t) printf("%p -> (x: %5.3f\ty: %5.3f), (x: %5.3f\ty: %5.3f), (x: %5.3f\ty: %5.3f)\n", \
                                  (void *)_t, _t->point[0]->x, _t->point[0]->y, _t->point[1]->x, _t->point[1]->y, _t->point[2]->x, _t->point[2]->y)

#define POINT_IN_TRIANGLE(_t, _p) ((CCW(_t->point[0], _p, _t->point[1]) > 0) ^ (CCW(_t->point[1], _p, _t->point[2]) > 0) ^ (CCW(_t->point[2], _p, _t->point[0]) > 0))
#define POINT_ON_TRIANGLE(_t, _p) (CCW(_t->point[0], _p, _t->point[1]) == 0 || CCW(_t->point[1], _p, _t->point[2]) == 0 || CCW(_t->point[2], _p, _t->point[0]) == 0)

typedef struct _Circle
{
    Point *center;
    float radius;
} Circle;

#define CIRCLE_MAKE() (Circle *)calloc(sizeof(Circle), 1)
#define CIRCLE_PRINT(_c) printf("%p -> r: %5.3f (x: %5.3f\ty: %5.3f)\n", (void *)_c, _c->radius, _c->center->x, _c->center->y)

Circle *CircumCircle(Triangle *t);
Point *GetDistPoint(Point *points);
Triangle *Delaunay(Point *furthest);