#include <stdio.h>
#include <stdlib.h>
#include <delaunay.h>
#include <utils.h>

int main(int argc, char **argv)
{
    /**
     * This is where it all began
     */
    Point *points = LoadPoints("test.data");
    if (!points)
    {
        printf("no points loaded");
        return 0;
    }

    Point *p = points;
    while (p)
    {
        POINT_PRINT(p);
        p = p->next;
    }

    Point *dist = GetDistPoint(points);
    printf("Most distant point: ");
    POINT_PRINT(dist);
    
    return 1;
}
