#include "../headers/raytrace.h"

// Add two vectors
vec vec_add(vec a, vec b)
{
    return (vec){ a.x + b.x, a.y + b.y, a.z + b.z };
}

// Subtract two vectors
vec vec_sub(vec a, vec b)
{
    return (vec){ a.x - b.x, a.y - b.y, a.z - b.z };
}

// Get dot product of two vectors
double vec_dot(vec a, vec b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Get length of a vector
double vec_len(vec a)
{
    return sqrt(vec_dot(a, a));
}

// Scale a vector by s
vec vec_scale(vec a, double s)
{
    return (vec){ a.x * s, a.y * s, a.z * s };
}

// Scale a vector by another vector
vec vec_cscale(vec a, vec b)
{
    return (vec){ a.x * b.x, a.y * b.y, a.z * b.z };
}

// Normalize a vector
vec vec_unit(vec a)
{
    return vec_scale(a, 1.0/vec_len(a));
}

// Get cross product of two vectors
vec vec_cross(vec a, vec b)
{
    return (vec){ (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
}

// Reflect a vector across another
vec vec_reflect(vec a, vec b)
{
    vec c = vec_scale(b, vec_dot(a, b));
    return vec_sub(vec_scale(c, 2), a);
}

// Print a vector (for debugging)
void vec_print(vec a)
{
    printf("{ %f, %f, %f }\n", a.x , a.y, a.z);
}
