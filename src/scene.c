#include "../headers/raytrace.h"

enum wall_type
{ XWALL, YWALL, ZWALL };

// Add a sphere with the given parameters to the scene
void add_sphere(int r, vec point, vec vel, vec color, double lambert, double specular, double ambient, int tex_id)
{
    objects[num_objs] = malloc(sizeof(object));
    objects[num_objs]->type = SPHERE;
    objects[num_objs]->obj = malloc(sizeof(sphere));
    sphere* s = (sphere*)(objects[num_objs]->obj);
    s->r = r;
    s->pt = point;
    s->color = color;
    s->velocity = vel;
    s->acceleration = (vec){ 0, 9.8, 0 };
    s->lambert = lambert;
    s->specular = specular;
    s->ambient = ambient;
    s->tex_index = tex_id;
    s->theta_base = 0;
    s->theta_vel = 0.5;
    s->mass = (4.0*M_PI/3.0)*(s->r*s->r*s->r);
    num_objs++;
}

// Add a wall with the given parameters to the scene
void add_wall(int wall_type, vec base, int w, int h, vec color, double lambert, double specular, double ambient, bool c, bool invis)
{
    // Getting points from wall type, base, width, height
    vec pts[6];
    pts[0] = base;
    if(wall_type == XWALL)
    {
        xwalls[num_xwalls] = base.x;
        num_xwalls++;
        if(invis) return;
        pts[1] = vec_add(base, (vec){ 0, 0, w });
        pts[2] = vec_add(base, (vec){ 0, h, 0 });
        pts[3] = vec_add(base, (vec){ 0, h, 0 });
        pts[4] = vec_add(base, (vec){ 0, 0, w });
        pts[5] = vec_add(base, (vec){ 0, h, w });
    }
    else if(wall_type == YWALL)
    {
        ywalls[num_ywalls] = base.y;
        num_ywalls++;
        if(invis) return;
        pts[1] = vec_add(base, (vec){ 0, 0, h });
        pts[2] = vec_add(base, (vec){ w, 0, 0 });
        pts[3] = vec_add(base, (vec){ w, 0, 0 });
        pts[4] = vec_add(base, (vec){ 0, 0, h });
        pts[5] = vec_add(base, (vec){ w, 0, h });
    }
    else if(wall_type == ZWALL)
    {
        zwalls[num_zwalls] = base.z;
        num_zwalls++;
        if(invis) return;
        pts[1] = vec_add(base, (vec){ 0, h, 0 });
        pts[2] = vec_add(base, (vec){ w, 0, 0 });
        pts[3] = vec_add(base, (vec){ w, 0, 0 });
        pts[4] = vec_add(base, (vec){ 0, h, 0 });
        pts[5] = vec_add(base, (vec){ w, h, 0 });
    }

    if(c)
    {
        vec swp = pts[0];
        pts[0] = pts[2];
        pts[2] = swp;
        swp = pts[3];
        pts[3] = pts[5];
        pts[5] = swp;
    }

    // First triangle
    objects[num_objs] = malloc(sizeof(object));
    objects[num_objs]->type = TRIANGLE;
    objects[num_objs]->obj = malloc(sizeof(triangle));
    triangle* t1 = (triangle*)(objects[num_objs]->obj);
    t1->pt1 = pts[0];
    t1->pt2 = pts[1];
    t1->pt3 = pts[2];
    t1->color = color;
    t1->lambert = lambert;
    t1->specular = specular;
    t1->ambient = ambient;
    num_objs++;

    // Second triangle
    objects[num_objs] = malloc(sizeof(object));
    objects[num_objs]->type = TRIANGLE;
    objects[num_objs]->obj = malloc(sizeof(triangle));
    triangle* t2 = (triangle*)(objects[num_objs]->obj);
    t2->pt1 = pts[3];
    t2->pt2 = pts[4];
    t2->pt3 = pts[5];
    t2->color = color;
    t2->lambert = lambert;
    t2->specular = specular;
    t2->ambient = ambient;
    num_objs++;
}

// Add a light with the given parameters to the scene
void add_light(int light_type, vec point, vec vel, vec color)
{
    lights[num_lights] = malloc(sizeof(light));
    lights[num_lights]->type = light_type;
    lights[num_lights]->pt = point;
    lights[num_lights]->velocity = vel;
    lights[num_lights]->color = color;
    num_lights++;
}

// Add a texture with the given parameters to the scene
void add_texture(int tex_type, const char* path)
{
    BITMAPINFOHEADER header;
    textures[num_textures] = malloc(sizeof(tex));
    textures[num_textures]->img = load_texture(path, &header);
    textures[num_textures]->w = header.biWidth;
    textures[num_textures]->h = header.biHeight;
    textures[num_textures]->type = tex_type;
    num_textures++;
}

char skip_to_slash(FILE* scene)
{
    char c;
    while((c = fgetc(scene)) != '/' && c != EOF);
    return c;
}

char get_word(FILE* scene, char* buf)
{
    int ibuf = 0;
    char c;
    while((c = fgetc(scene)) != ' ' && c != '\n' && c != EOF)
    {
        buf[ibuf] = c;
        ibuf++;
    }
    buf[ibuf] = '\0';
    return c;
}

// Read the scene from a file
int scene_init(const char* path, int* width, int* height)
{
    // Open scene description
    FILE* scene = fopen(path, "r");
    if(!scene)
    {
        fprintf(stderr, "Error: scene file %s does not exist\n", path);
        return -1;
    }

    // Setup
    num_objs = 0;
    num_zwalls = 0;
    num_xwalls = 0;
    num_ywalls = 0;
    num_lights = 0;
    num_textures = 0;
    objects = malloc(sizeof(object*) * 100);
    lights = malloc(sizeof(light*) * 100);
    textures = malloc(sizeof(tex*) * 100);

    // Read scene description (probably the worst code I've ever written in my life)
    char word[100];
    skip_to_slash(scene);
    get_word(scene, word);
    get_word(scene, word);
    *width = atoi(word);
    get_word(scene, word);
    *height = atoi(word);
    skip_to_slash(scene);
    get_word(scene, word);
    skip_to_slash(scene);
    get_word(scene, word);
    get_word(scene, word);
    cam.fov = atoi(word);
    skip_to_slash(scene);
    get_word(scene, word);
    get_word(scene, word);
    cam.eye_pt.x = atoi(word);
    get_word(scene, word);
    cam.eye_pt.y = atoi(word);
    get_word(scene, word);
    cam.eye_pt.z = atoi(word);
    skip_to_slash(scene);
    get_word(scene, word);
    get_word(scene, word);
    cam.to_pt.x = atoi(word);
    get_word(scene, word);
    cam.to_pt.y = atoi(word);
    get_word(scene, word);
    cam.to_pt.z = atoi(word);
    skip_to_slash(scene);
    get_word(scene, word);

    if(!strcmp(word, "texture"))
    {
        int tex_type = 0;
        while(!strcmp(word, "texture"))
        {
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            if(!strcmp(word, "bump"))
            {
                tex_type = BUMP_MAP;
            }
            else if(!strcmp(word, "intensity"))
            {
                tex_type = INTENSITY_MAP;
            }
            else if(!strcmp(word, "color"))
            {
                tex_type = COLOR_MAP;
            }
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            add_texture(tex_type, word);
            skip_to_slash(scene);
            get_word(scene, word);
        }
    }

    if(!strcmp(word, "sphere"))
    {
        int radius;
        vec pt;
        vec vel;
        vec color;
        double lambert;
        double specular;
        double ambient;
        int tex_id;
        while(!strcmp(word, "sphere"))
        {
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            radius = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            pt.x = atoi(word);
            get_word(scene, word);
            pt.y = atoi(word);
            get_word(scene, word);
            pt.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            vel.x = atoi(word);
            get_word(scene, word);
            vel.y = atoi(word);
            get_word(scene, word);
            vel.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            color.x = atoi(word);
            get_word(scene, word);
            color.y = atoi(word);
            get_word(scene, word);
            color.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            lambert = atof(word);
            get_word(scene, word);
            specular = atof(word);
            get_word(scene, word);
            ambient = atof(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            tex_id = atoi(word);
            add_sphere(radius, pt, vel, color, lambert, specular, ambient, tex_id);
            skip_to_slash(scene);
            get_word(scene, word);
        }
    }

    if(!strcmp(word, "light"))
    {
        int light_type;
        vec pt;
        vec vel;
        vec color;
        while(!strcmp(word, "light"))
        {
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            if(!strcmp(word, "point"))
            {
                light_type = L_POINT;
            }
            else if(!strcmp(word, "dir"))
            {
                light_type = L_DIR;
            }
            else if(!strcmp(word, "spot"))
            {
                light_type = L_SPOT;
            }
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            pt.x = atoi(word);
            get_word(scene, word);
            pt.y = atoi(word);
            get_word(scene, word);
            pt.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            vel.x = atoi(word);
            get_word(scene, word);
            vel.y = atoi(word);
            get_word(scene, word);
            vel.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            color.x = atoi(word);
            get_word(scene, word);
            color.y = atoi(word);
            get_word(scene, word);
            color.z = atoi(word);
            add_light(light_type, pt, vel, color);
            skip_to_slash(scene);
            get_word(scene, word);
        }
    }

    if(!strcmp(word, "wall"))
    {
        int axis;
        vec base;
        int w;
        int h;
        vec color;
        double lambert;
        double specular;
        double ambient;
        bool invis;
        while(!strcmp(word, "wall"))
        {
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            if(!strcmp(word, "x"))
            {
                axis = XWALL;
            }
            else if(!strcmp(word, "y"))
            {
                axis = YWALL;
            }
            else if(!strcmp(word, "z"))
            {
                axis = ZWALL;
            }
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            base.x = atoi(word);
            get_word(scene, word);
            base.y = atoi(word);
            get_word(scene, word);
            base.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            w = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            h = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            color.x = atoi(word);
            get_word(scene, word);
            color.y = atoi(word);
            get_word(scene, word);
            color.z = atoi(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            lambert = atof(word);
            get_word(scene, word);
            specular = atof(word);
            get_word(scene, word);
            ambient = atof(word);
            skip_to_slash(scene);
            get_word(scene, word);
            get_word(scene, word);
            if(!strcmp(word, "yes"))
            {
                invis = true;
            }
            else
            {
                invis = false;
            }
            bool mess = false;
            if(axis == XWALL && base.x > 0) mess = true;
            add_wall(axis, base, w, h, color, lambert, specular, ambient, mess, invis);
            skip_to_slash(scene);
            get_word(scene, word);
        }
    }

    // Other parameters
    anti_aliasing = true;
    energy_loss = 0.9;
    cam.up = (vec){ 0, 1, 0 };
    cam.r = cam.eye_pt.z - cam.to_pt.z;
    fclose(scene);
    return 0;
}

void scene_destroy()
{
    // Free objects
    for(int i = 0; i < num_objs; i++)
    {
        free(objects[i]->obj);
        free(objects[i]);
    }
    free(objects);

    // Free lights
    for(int i = 0; i < num_lights; i++)
    {
        free(lights[i]);
    }
    free(lights);

    // Free textures
    for(int i = 0; i < num_textures; i++)
    {
        free(textures[i]->img);
        free(textures[i]);
    }
    free(textures);
}
