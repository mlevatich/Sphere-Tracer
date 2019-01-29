#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Types of objects
enum object_type
{ SPHERE, TRIANGLE };

// Types of lights
enum light_type
{ L_POINT, L_DIR, L_SPOT };

// Types of texture maps
enum texture_type
{ COLOR_MAP, INTENSITY_MAP, BUMP_MAP };

#define MAX_WALLS 20

// Vector structure
typedef struct
{
    double x;
    double y;
    double z;
}
vec;

// Ray structure
typedef struct
{
    vec pt;
    vec dir;
}
ray;

// Camera
typedef struct
{
    int fov;
    vec eye_pt;
    vec to_pt;
    vec up;
    int r;
}
camera;

// Generic object
typedef struct
{
    int type;
    void* obj;
}
object;

// Sphere
typedef struct
{
    int r;
    double mass;
    vec pt;
    vec color;
    vec acceleration;
    vec velocity;
    double lambert;
    double specular;
    double ambient;
    int tex_index;
    double theta_base;
    double theta_vel;
}
sphere;

// Triangle
typedef struct
{
    vec pt1;
    vec pt2;
    vec pt3;
    vec color;
    double lambert;
    double specular;
    double ambient;
}
triangle;

// Light
typedef struct
{
    int type;
    vec pt;
    vec color;
    vec velocity;
}
light;

// Texture
typedef struct
{
    int w;
    int h;
    uint8_t* img;
    int type;
}
tex;

// Bmp stuff
typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

// Global scene description
camera cam;

int num_objs;
object** objects;

int num_xwalls;
int xwalls[MAX_WALLS];

int num_zwalls;
int zwalls[MAX_WALLS];

int num_ywalls;
int ywalls[MAX_WALLS];

int num_lights;
light** lights;

int num_textures;
tex** textures;

bool anti_aliasing;
double energy_loss;

// Initialize scene from file
int scene_init(const char* path, int* width, int* height);
void scene_destroy();

// Load texture from bmp file
uint8_t* load_texture(const char* path, BITMAPINFOHEADER *bitmapInfoHeader);

// Get color at a pixel
vec trace(ray r, int depth);

// Advance simulation
void take_timestep(double ms, int nframes);

// Vector operations
vec vec_add(vec a, vec b);
vec vec_sub(vec a, vec b);
double vec_dot(vec a, vec b);
double vec_len(vec a);
vec vec_scale(vec a, double s);
vec vec_cscale(vec a, vec b);
vec vec_unit(vec a);
vec vec_cross(vec a, vec b);
vec vec_reflect(vec a, vec b);
void vec_print(vec a);
