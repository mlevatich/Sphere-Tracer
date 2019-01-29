#include "../headers/raytrace.h"

// Advance the numerical integration by some timestep
void take_timestep(double ms, int nframes)
{
    // Timestep
    double step = ms/1000.0;

    // Move camera
    // double deg_inc = 360.0 / (double)nframes;
    // double rad_inc = deg_inc * M_PI / (double)180.0;
    // double rad_cur = atan2(cam.eye_pt.z, cam.eye_pt.x);
    // cam.eye_pt.z = sin(rad_cur + rad_inc) * cam.r;
    // cam.eye_pt.x = cos(rad_cur + rad_inc) * cam.r;

    // Move objects
    for(int i = 0; i < num_objs; i++)
    {
        // Nothing for triangles
        if(objects[i]->type == TRIANGLE) continue;
        sphere* s = (sphere*)(objects[i]->obj);

        // Update position due to velocity
        s->pt = vec_add(s->pt, vec_scale(s->velocity, step));

        // Update rotation
        s->theta_base += s->theta_vel * step;
        if(s->theta_base >= 2*M_PI) s->theta_base -= 2*M_PI;

        // Detect collisions with ground/ceiling
        double yv_new = -1 * sqrt(energy_loss * s->velocity.y * s->velocity.y);
        for(int j = 0; j < num_ywalls; j++)
        {
            // Are we in the wall?
            double right_side = s->pt.y + s->r;
            double left_side = s->pt.y - s->r;
            if(right_side > ywalls[j] && left_side < ywalls[j])
            {
                if(right_side - ywalls[j] > ywalls[j] - left_side)
                {
                    s->pt.y = ywalls[j] + 0.5 + s->r;
                }
                else
                {
                    s->pt.y = ywalls[j] - 0.5 - s->r;
                }
                s->velocity.y = yv_new;
            }
        }

        // Detect collisions with walls
        double xv_new = sqrt(energy_loss * s->velocity.x * s->velocity.x) * ((s->velocity.x > 0) ? -1 : 1);
        for(int j = 0; j < num_xwalls; j++)
        {
            // Are we in the wall?
            double right_side = s->pt.x + s->r;
            double left_side = s->pt.x - s->r;
            if(right_side > xwalls[j] && left_side < xwalls[j])
            {
                if(right_side - xwalls[j] > xwalls[j] - left_side)
                {
                    s->pt.x = xwalls[j] + 0.5 + s->r;
                }
                else
                {
                    s->pt.x = xwalls[j] - 0.5 - s->r;
                }
                s->velocity.x = xv_new;
            }
        }
        double zv_new = sqrt(energy_loss * s->velocity.z * s->velocity.z) * ((s->velocity.z > 0) ? -1 : 1);
        for(int j = 0; j < num_zwalls; j++)
        {
            // Are we in the wall?
            double right_side = s->pt.z + s->r;
            double left_side = s->pt.z - s->r;
            if(right_side > zwalls[j] && left_side < zwalls[j])
            {
                if(right_side - zwalls[j] > zwalls[j] - left_side)
                {
                    s->pt.z = zwalls[j] + 0.5 + s->r;
                }
                else
                {
                    s->pt.z = zwalls[j] - 0.5 - s->r;
                }
                s->velocity.z = zv_new;
            }
        }

        // Collisions with other spheres
        for(int j = 0; j < num_objs; j++)
        {
            if(objects[j]->type == TRIANGLE || objects[j] == objects[i]) continue;
            sphere* s2 = (sphere*)(objects[j]->obj);
            vec center_line = vec_sub(s2->pt, s->pt);
            vec x = vec_unit(center_line);
            if(s->r + s2->r > vec_len(center_line))
            {
                s->pt = vec_add(s->pt, vec_scale(x, -3));
                s2->pt = vec_add(s2->pt, vec_scale(x, 3));

                vec v1 = s->velocity;
                double x1 = vec_dot(v1, x);
                vec v1x = vec_scale(x, x1);
                vec v1y = vec_sub(v1, v1x);
                double m1 = s->mass;

                x = vec_scale(x, -1);
                vec v2 = s2->velocity;
                double x2 = vec_dot(v2, x);
                vec v2x = vec_scale(x, x2);
                vec v2y = vec_sub(v2, v2x);
                double m2 = s2->mass;

                s->velocity =  vec_add(vec_add(vec_scale(v1x, (m1-m2)/(m1+m2)), vec_scale(v2x, (2*m2)/(m1+m2))), v1y);
                s2->velocity = vec_add(vec_add(vec_scale(v1x, (2*m1)/(m1+m2)),  vec_scale(v2x, (m2-m1)/(m1+m2))), v2y);
            }
        }

        // Update velocity due to acceleration
        s->velocity = vec_add(s->velocity, vec_scale(s->acceleration, step));

        // Update acceleration due to forces
        s->acceleration.y += 9.8*step;
    }

    // Move lights
    for(int i = 0; i < num_lights; i++)
    {
        lights[i]->pt = vec_add(lights[i]->pt, vec_scale(lights[i]->velocity, step));
    }

    // Move textures
}
