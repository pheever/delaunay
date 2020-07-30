#include <delaunay.h>

/**
 * Initialises delaunay triangulation
 * From the point furthest from (0,0) create a triangle to cover all points
 */
Triangle *Delaunay(Point *points)
{
    Triangle *root_tri;

    /**
     * Init root triangle using the point furthest from (0,0)
     */
    Point *p = GetDistPoint(points);
    root_tri = TRIANGLE_MAKE();
    root_tri->neighbors = NULL;
    root_tri->point = (Point **)calloc(sizeof(Point *), 3);
    root_tri->point[0] = POINT_MAKE();
    root_tri->point[0]->x = (int)p->d * -3;
    root_tri->point[0]->y = (int)p->d * -3;
    root_tri->point[1] = POINT_MAKE();
    root_tri->point[1]->x = 0;
    root_tri->point[1]->y = (int)p->d * 3;
    root_tri->point[2] = POINT_MAKE();
    root_tri->point[2]->x = (int)p->d * 3;
    root_tri->point[2]->y = 0;

    

    return root_tri;
}

Circle *CircumCircle(Triangle *t)
{
    Circle *circle;
    Point *b, *c;
    //translate points to (0,0)
    b = POINT_MAKE();
    b->x = t->point[1]->x - t->point[0]->x;
    b->y = t->point[1]->y - t->point[0]->y;
    c = POINT_MAKE();
    c->x = t->point[2]->x - t->point[0]->x;
    c->y = t->point[2]->y - t->point[0]->y;

    //2 x determinant
    float d = 2 * (b->x * c->y - b->y * c->x);

    //initialize circle
    circle = CIRCLE_MAKE();
    circle->center = POINT_MAKE();

    //calculate center
    circle->center->x = (c->y * POINT_SQR(b) - b->y * POINT_SQR(c)) / d + t->point[0]->x;
    circle->center->y = (b->x * POINT_SQR(c) - c->x * POINT_SQR(b)) / d + t->point[0]->y;

    //calculate radius
    circle->radius = DISTANCE(circle->center, t->point[0]);

    //free excess points
    free(b);
    free(c);

    return circle;
}
Point *GetDistPoint(Point *points)
{
    Point *p, *f;
    f = p = points;
    while ((p = p->next))
    {
        f = p->d > f->d ? p : f;
    }
    return f;
}