#include "../headers/raytrace.h"

#define FILENAME_FORMAT "frame%04d.ppm"

int main(int argc, char** argv)
{
    // Video settings
    int fps = atoi(argv[1]);
    int vid_len_s = atoi(argv[2]);
    if(fps <= 0 || vid_len_s <= 0)
    {
        fprintf(stderr, "Error: frames_per_second and length_in_seconds must be integers > 0\n");
        return 1;
    }

    int vid_len = 1000*vid_len_s;
    int num_frames = vid_len_s * fps;
    double ms_per_frame = (double)vid_len / (double)num_frames;
    int width;
    int height;

    // Scene initialization from scene file
    scene_init(argv[3], &width, &height);

    // Frame creation loop
    FILE* img;
    char filename[14] = {0};
    char header[50] = {0};
    double* img_array = malloc(sizeof(double) * 3 * width * height);
    for(int x = 0; x < num_frames; x++)
    {
        // Viewport initialization
        vec eye_vec = vec_unit(vec_sub(cam.to_pt, cam.eye_pt));
        vec right_vec = vec_unit(vec_cross(eye_vec, cam.up));
        vec up_vec = vec_unit(vec_cross(right_vec, eye_vec));

        double fovRadians = M_PI * ((double)cam.fov / 2.0) / (double)180.0;
        double halfWidth = tan(fovRadians);
        double halfHeight = ((double)height / (double)width) * halfWidth;
        double pixelWidth = (double)(halfWidth * 2) / (double)(width - 1);
        double pixelHeight = (double)(halfHeight * 2) / (double)(height - 1);

        // Open next frame
        sprintf(filename, FILENAME_FORMAT, x);
        sprintf(header, "P6 %d %d 255\n", width, height);
        img = fopen(filename, "w");
        fwrite(header, sizeof(char), strlen(header), img);

        // Trace!
        int index = 0;
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
            {
                vec color = (vec){ 0x00, 0x00, 0x00 };
                if(anti_aliasing)
                {
                    // Anti-aliasing
                    for(double k = 0; k < 1; k += 0.25)
                    {
                        vec x_comp = vec_scale(right_vec, (i * pixelWidth) - halfWidth);
                        vec y_comp = vec_scale(up_vec, (j * pixelHeight) - halfHeight);
                        x_comp.x += pixelWidth * k;
                        y_comp.y += pixelWidth * k;
                        ray primary = (ray){ cam.eye_pt, vec_unit(vec_add(eye_vec, vec_add(x_comp, y_comp))) };
                        color = vec_add(color, trace(primary, 0));
                    }
                    color = vec_scale(color, 0.25);
                }
                else
                {
                    // No anti-aliasing
                    vec x_comp = vec_scale(right_vec, (i * pixelWidth) - halfWidth);
                    vec y_comp = vec_scale(up_vec, (j * pixelHeight) - halfHeight);
                    ray primary = (ray){ cam.eye_pt, vec_unit(vec_add(eye_vec, vec_add(x_comp, y_comp))) };
                    color = vec_add(color, trace(primary, 0));
                }

                img_array[index+0] = color.x;
                img_array[index+1] = color.y;
                img_array[index+2] = color.z;
                index += 3;
            }
        }

        // Get max intensity
        int maxI = 0;
    	for(int j = 0; j < height; j++)
    	{
    		for(int i = 0; i < width; i++)
    		{
    	        index = (i * 3) + (j * width * 3);
    			int intensity = 0.3*img_array[index] + 0.5*img_array[index+1] + 0.2*img_array[index+2];
    			if (intensity > maxI) maxI = intensity;
    		}
    	}

        // Scale pixels by max intensity and clamp to 255
    	for(int j = 0; j < height; j++)
    	{
    		for(int i = 0; i < width; i++)
    		{
    	        index = (i * 3) + (j * width * 3);
    			for(int k = 0; k<3; k++)
    			{
    				  img_array[index+k] *= ((double)255.0 / (double)maxI);
    	              if(img_array[index+k] > 255) img_array[index+k] = 255;
    			}
    		}
    	}

        // Write to frame
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
            {
                index = (i * 3) + (j * width * 3);
                for(int k = 0; k < 3; k++)
                {
                    uint8_t rgb = (uint8_t)((int)(img_array[index+k]) & 0xFF);
                    fwrite(&rgb, 1, 1, img);
                }
            }
        }

        // Close frame
        fclose(img);

        // Advance simulation
        take_timestep(ms_per_frame, num_frames);

        // Logging
        printf("%03d/%d\n", x+1, num_frames);
    }

    // Clean up and exit
    scene_destroy();
    free(img_array);
    return 0;
}
