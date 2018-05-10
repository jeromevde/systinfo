#include <stdlib.h>
#include <memory.h>
#include "fractal.h"
#include <string.h>


struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    fractal_t *allocatedFractal = (fractal_t *) malloc(sizeof(fractal_t));

    if (allocatedFractal == NULL) {
        return NULL;
    }

    allocatedFractal->name = malloc(strlen(name) + sizeof(char));

    if (allocatedFractal->name == NULL) {
        free(allocatedFractal);
        return NULL;
    }

    strncpy(allocatedFractal->name, name, strlen(name) + sizeof(char));

    allocatedFractal->width = width;
    allocatedFractal->height = height;

    allocatedFractal->a = a;
    allocatedFractal->b = b;

    allocatedFractal->values = calloc(width * height, sizeof(int));
    if (allocatedFractal->values == NULL) {
        free(allocatedFractal->name);
        free(allocatedFractal);
        return NULL;
    }
    return allocatedFractal;
}

void fractal_free(struct fractal *f)
{
    //free(f->values);
    free(f->name);
    //free(f);
}

const char *fractal_get_name(const struct fractal *f)
{
    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    return (f->values)[y*f->width + x];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
    (f->values)[y*f->width + x] = val;
}

int fractal_get_width(const struct fractal *f)
{
    return f->width;
}

int fractal_get_height(const struct fractal *f)
{
    return f->height;
}

double fractal_get_a(const struct fractal *f)
{
    return f->a;
}

double fractal_get_b(const struct fractal *f)
{
    return f->b;
}
