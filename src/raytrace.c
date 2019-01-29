#include "../headers/raytrace.h"

// Figure out if a ray intersects the given triangle
double triangle_intersect(ray r, triangle* t)
{
    vec norm = vec_unit(vec_cross(vec_sub(t->pt2, t->pt1), vec_sub(t->pt3, t->pt1)));
    double d = vec_dot(norm, t->pt1);
    double ndotray = vec_dot(r.dir, norm);
    if(fabs(ndotray) < 0.00001) return 0;
    double ti = -1*(vec_dot(r.pt, norm) - d) / ndotray;
    if(ti < 0) return 0;
    vec intersection = vec_add(r.pt, vec_scale(r.dir, ti));
    vec l = vec_sub(intersection, r.pt);
    double distance = vec_len(l);

    double area = vec_len(vec_cross(vec_sub(t->pt2, t->pt1), vec_sub(t->pt3, t->pt1))) * 0.5;
    double a = (vec_len(vec_cross(vec_sub(t->pt2, intersection), vec_sub(t->pt3, intersection))) * 0.5) / area;
    double b = (vec_len(vec_cross(vec_sub(t->pt3, intersection), vec_sub(t->pt1, intersection))) * 0.5) / area;
    double c = (vec_len(vec_cross(vec_sub(t->pt1, intersection), vec_sub(t->pt2, intersection))) * 0.5) / area;
    double q = a + b + c;
    if(a < 0 || a > 1 || b < 0 || b > 1 || c < 0 || c > 1 || q < 0 || q > 1.01) return 0;
    return distance - 1;
}

// Figure out if a ray intersects the given sphere
double sphere_intersect(ray r, sphere* s)
{
    vec eye_to_center = vec_sub(s->pt, r.pt);
    double v = vec_dot(eye_to_center, r.dir);
    double eoDot = vec_dot(eye_to_center, eye_to_center);
    double d = (s->r * s->r) - eoDot + (v * v);

    double dist = 0;
    if(!(fabs((vec_len(eye_to_center) - s->r) / s->r) < .001) && d >= 0) dist = v - sqrt(d);
    return dist;
}

// Check if a ray intersects any objects in the scene
double intersect_scene(ray r, object** o)
{
    double dist = 0;
    double min_dist = 100000000;
    bool hit_something = false;
    for(int i = 0; i < num_objs; i++)
    {
        if((objects[i]->type == SPHERE   && (dist = sphere_intersect(r, (sphere*)(objects[i]->obj)))) ||
           (objects[i]->type == TRIANGLE && (dist = triangle_intersect(r, (triangle*)(objects[i]->obj)))))
        {
            hit_something = true;
            if(dist >= 100000000) printf("wtf\n");
            if(dist < min_dist)
            {
                min_dist = dist;
                *o = objects[i];
            }
        }
    }
    return hit_something ? min_dist : 0;
}

// Return the normal of the given object
vec object_normal(object* o, vec pt)
{
    if(o->type == SPHERE)
    {
        sphere* s = (sphere*)o->obj;
        return vec_unit(vec_sub(pt, s->pt));
    }
    else
    {
        triangle* t = (triangle*)o->obj;
        return vec_unit(vec_cross(vec_sub(t->pt3, t->pt1), vec_sub(t->pt2, t->pt1)));
    }
}

// Helper function to compute the intensity at an image index and scale it to [-1, 1]
double scaleIntensity(tex* t, int index)
{
    double intensity = 0.3*t->img[index+0] + 0.55*t->img[index+1] + 0.15*t->img[index+2];
    return (-1.0 + intensity / 128.0);
}

// Helper function to compute the intensity vector D for this location in the bump map
vec getIntensityVector(tex* t, int i, int j)
{
    // Check if at the edges of image
    if(i == 0 || j == 0 || i == t->w - 1 || j == t->h - 1) return (vec){ 0, 0, 0 };

    // Get index
    int index = 3*(i + j*t->w);

    // Compute intensity delta in the x direction
    double ix1 = scaleIntensity(t, index+3);
    double ix0 = scaleIntensity(t, index-3);
    double x_delta = (ix1 - ix0) / 2;

    // Compute intensity delta in the y direction
    double iy1 = scaleIntensity(t, index + t->w*3);
    double iy0 = scaleIntensity(t, index - t->w*3);
    double y_delta = (iy1 - iy0) / 2;

    return (vec){ x_delta, y_delta, 0 };
}

// Return true if the point is not in shadow
bool is_light_visible(vec pt, vec light)
{
    ray from_object = (ray){ pt, vec_unit(vec_sub(light, pt)) };
    object* dummy;
    return (intersect_scene(from_object, &dummy) < 0.5);
}

// Return the color given by the ray's intersection with an object
vec surface(ray r, object* o, double d, int depth)
{
    // Intersection point and normal
    vec intersectPt = vec_add(r.pt, vec_scale(r.dir, d));
    vec normal = object_normal(o, intersectPt);

    // Extract object properties from pointer
    vec obj_color = (vec){ 0, 0, 0 };
    double lambert = 0;
    double specular = 0;
    double ambient = 0;
    int tex_id = -1;
    if(o->type == SPHERE)
    {
        obj_color = ((sphere*)(o->obj))->color;
        lambert = ((sphere*)(o->obj))->lambert;
        specular = ((sphere*)(o->obj))->specular;
        ambient = ((sphere*)(o->obj))->ambient;
        tex_id = ((sphere*)(o->obj))->tex_index;
    }
    else
    {
        obj_color = ((triangle*)(o->obj))->color;
        lambert = ((triangle*)(o->obj))->lambert;
        specular = ((triangle*)(o->obj))->specular;
        ambient = ((triangle*)(o->obj))->ambient;
    }

    // Texture mapping
    if(tex_id >= 0)
    {
        sphere* s = (sphere*)o->obj;
        tex* t = textures[tex_id];
        vec sphere_coords = vec_unit(vec_sub(intersectPt, s->pt));
        double theta_rad = s->theta_base + atan2(sphere_coords.z, sphere_coords.x) + M_PI;
        if(theta_rad >= 2*M_PI) theta_rad -= 2*M_PI;
        double fi_rad = acos(sphere_coords.y);
        int tex_i = (theta_rad / (2*M_PI)) * t->w;
        int tex_j = (fi_rad / M_PI) * t->h;
        int index = 3*(tex_i + tex_j*t->w);
        if(t->type == COLOR_MAP)
        {
            vec tex_color = (vec){t->img[index + 0], t->img[index + 1], t->img[index + 2]};
            obj_color = tex_color;
        }
        else if(t->type == INTENSITY_MAP)
        {
            double intensity = 0.3*t->img[index + 0] + 0.55*t->img[index + 1] + 0.15*t->img[index + 2];
            obj_color = vec_scale(obj_color, intensity/255.0);
        }
        else if(t->type == BUMP_MAP)
        {
            vec xy_vector = getIntensityVector(t, tex_i, tex_j);
            vec i_vector = vec_add(vec_scale(vec_unit(vec_cross(normal, (vec){ 0, 1, 0 })), xy_vector.x*5),
                                   vec_scale(vec_unit(vec_cross(normal, (vec){ 0, 0, 1 })), xy_vector.y*5));
            normal = vec_unit(vec_add(i_vector, normal));
        }
    }

    // Diffuse
    vec lambertVec = (vec){ 0, 0, 0 };
    if(lambert > 0.001)
    {
        for(int i = 0; i < num_lights; i++)
        {
            if(is_light_visible(intersectPt, lights[i]->pt))
            {
                double contribution = vec_dot(vec_unit(vec_sub(lights[i]->pt, intersectPt)), normal);
               	if(contribution > 0)
                {
                    lambertVec = vec_add(lambertVec, vec_scale(lights[i]->color, contribution));
                }
            }
        }

        lambertVec = vec_cscale(lambertVec, obj_color);
        lambertVec = vec_scale(lambertVec, lambert);
        lambertVec = vec_scale(lambertVec, (double)1.0/(double)255.0);
    }

    // Specular
    vec specVec = (vec){ 0, 0, 0 };
    if(specular > 0.001)
    {
        ray reflectedRay = (ray){ intersectPt, vec_reflect(vec_scale(r.dir, -1), normal) };
        vec reflectedColor = trace(reflectedRay, depth + 1);
        if(reflectedColor.x >= 0)
        {
              specVec = vec_add(specVec, vec_scale(reflectedColor, specular));
        }
    }

    // Ambient
    vec ambientVec = vec_scale(obj_color, ambient);

    return vec_add(lambertVec, vec_add(specVec, ambientVec));
}

// Shoot ray to given depth
vec trace(ray r, int depth)
{
    if (depth > 3) return (vec){ -1, -1, -1 };

    object* o;
    double distance = intersect_scene(r, &o);
    if(distance) return surface(r, o, distance, depth);
    return (vec){ 0x00, 0x00, 0x00 };
}
