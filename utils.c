#include <stdlib.h>
#include <delaunay.h>
Point *LoadPoints(char *filepath)
{
    FILE *file = fopen(filepath, "r");
    Point *p, *head = 0;
    if (!file)
    {
        printf("Could not read file '%s'\n", filepath);
        return NULL;
    }
    p = POINT_MAKE();
    while (fscanf(file, "%f,%f\n", &p->x, &p->y) == 2)
    {
        p->d = DISTANCE(p, 0);
        p->next = head;
        head = p;
        p = POINT_MAKE();
    }

    return head;
}