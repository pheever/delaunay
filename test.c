#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define RANDF(_min, _max) (_min + ((float)rand() / RAND_MAX) * (_max - _min))

typedef struct _Options
{
    int points, max_x, min_x, max_y, min_y;
    char *out;
} Options;
Options *Config(int argc, char **argv);

int main(int argc, char **argv)
{
    FILE *f = stdout;
    Options *opts;
    if ((argc - 1) % 2 != 0)
    {
        printf("You're probably missing an argument, try again\n");
        return 0;
    }
    opts = Config(argc, argv);

    if (opts->out)
    {
        f = fopen(opts->out, "w+");
        if (!f)
        {
            printf("Could not open file '%s'\n", opts->out);
            return 0;
        }
    }

    srand(time(0));
    for (int i = opts->points; i > 0; i--)
    {
        fprintf(f, "%f,%f\n", RANDF(opts->min_x, opts->max_x), RANDF(opts->min_y, opts->max_y));
    }
    fclose(f);
}

Options *Config(int argc, char **argv)
{
    //char *curr;
    int i;
    Options *opts = (Options *)calloc(sizeof(Options), 1);
    /**
     * Default values
     */
    opts->points = 50;
    opts->max_x = 50;
    opts->min_x = -50;
    opts->max_y = 50;
    opts->min_y = -50;
    opts->out = 0;

    //Nothing was passed
    if (argc < 2)
        return opts;

    for (i = 1; i < argc; i += 2)
    {
        if (strcmp("-points", argv[i]) == 0)
        {
            opts->points = atoi(argv[i + 1]);
        }
        else if (strcmp("-maxx", argv[i]) == 0)
        {
            opts->max_x = atoi(argv[i + 1]);
        }
        else if (strcmp("-minx", argv[i]) == 0)
        {
            opts->min_x = atoi(argv[i + 1]);
        }
        else if (strcmp("-maxy", argv[i]) == 0)
        {
            opts->max_y = atoi(argv[i + 1]);
        }
        else if (strcmp("-miny", argv[i]) == 0)
        {
            opts->min_y = atoi(argv[i + 1]);
        }
        else if (strcmp("-out", argv[i]) == 0)
        {
            opts->out = argv[i + 1];
        }
    }

    return opts;
}