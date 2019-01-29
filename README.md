# Sphere-Tracer

Turn a plaintext scene description into a beautifully ray-traced sphere-filled movie!

# Install

~~~~
# Install ffmpeg (brew is easiest for mac)
brew install ffmpeg

# Clone the repo
git clone https://github.com/mlevatich/Sphere-Tracer.git
cd Sphere-Tracer
~~~~

# Usage

First, create a scene description (see below). Then generate the simulation with:

~~~~
# Edit makefile first if on linux
make
chmod +x create_simulation
./create_simulation frames_per_second length_in_seconds scene_desc.txt
~~~~

On linux, you need to manually link the math library, so add the '-lm' flag to the commands in the makefile 
before making.

The simulation comes out as the file sim.avi. The ppm frame files are deleted when the program finishes.

# Examples

A sample scene description is given in examples/sphere_party.txt. The movie which that description generates is 
examples/sphere_party.avi. While it is only three seconds long due to the limitations of file sizes on github, 
sphere_party.avi is a timeless work of art which showcases the many awesome features of my silly simulator.

Some bmp files are provided for texture mapping in the textures directory, but users can provide their own 
if they so desire.

# Features

Highly customizable scene description – scenes include:

    Spheres
    
        -spheres come equipped with an initial position and velocity, and are affected by gravity.
         a RGB color is specified for each sphere, in addition to the degree of diffuse, specular, 
         and ambient reflectance.
        -spheres can have textures mapped onto them (see 'textures').
        -spheres collide with both walls and other spheres in realistic ways, conserving momentum 
         upon collision with spheres and losing a portion of kinetic energy upon collision with 
         walls. A sphere's mass is determined by its radius, which is specified by the user.
        -spheres rotate pleasingly around their y axes.
        
    Walls
    
        -walls are built from ray traced triangles and have rectangular dimensions.
        -walls (or floors, as the case may be) are specified by their axis (a wall on the y axis 
         is a floor/ceiling, etc), a base coordinate, and a width and height.
        -spheres collide with all types of walls.
        -walls can be shaded like spheres, with diffuse, specular, and ambient reflectance, but 
         cannot have textures mapped onto them. Like spheres, they have a base color.
        -walls can be made invisible.
        
    Lights
    
        -lights are specified by a point and a color. While the code has the infrastructure for 
         spot lights and directional
        -lights, only point lights are supported.
        -like spheres, lights can be given a velocity, so that they move across the screen in 
         cool ways.
        
    Textures
    
        -textures (bmp files, most conveniently 512x512), can be mapped onto spheres in one of 
         three ways. a color-mapped texture will replace the color of a sphere completely with 
         the texture, like wrapping paper.
        -an intensity-mapped texture will modify the brightness of the sphere's color by the 
         intensity of the texture at a given point, preserving the sphere's color but giving 
         it the texture.
        -a bump-mapped texture will modify the surface normal to give the sphere the appearance 
         of bumpiness.
        -textures rotate with each frame, so that the spheres appear to be spinning!
        
    Camera
    
        -a field of view, eye point and to point can be specified for the camera.
        -while it is currently commented out in the code and the functionality can't be accessed 
         from the scene description, the camera can move! uncommenting the lines in sim.c will 
         cause the camera to move in a circle. It's cool, but a bit much.

# The Scene Description

The scene file needs to obey very specific syntax in order to work.

Every relevant line in the scene file should start with a forward slash '/'. Lines without this will 
be ignored, so you can put comments there and other nonsense. Lines take one of the following forms:

        /identifier value value ...

        /identifier
            /field value value ...
            /field value ...

Identifiers are expected in the following order: resolution, camera, texture, sphere, light, wall.  
You must include the resolution and camera fields. All of the others can be omitted or repeated any 
number of times, so long as they are in order.

The fields inside an identifier must also be in a SPECIFIC order, and if you include an identifier 
(sphere, for example), you must have ALL of its fields under it.

Here is a list of each identifier's fields and their descriptions, in the order they should appear.
Refer to examples/sphere_party.txt to see this in action:

    resolution
    
        -width and height (in pixels) follow immediately after the resolution, on the same line.
         They should be integers.

    camera
    
        -fov is the field of view, in degrees. It should be an integer.
        -eye is a vector representing the eye point where the rays are shot from. It should be 
         followed by three integers (x, y, z)
        -to is where the eye is looking to. It is also a vector.

    texture
    
        -type is one of 'bump', 'intensity', or 'color', corresponding to a bump map, intensity 
         map, or color map (described above).
        -file is the bmp file from which to load the texture into memory. The pathname can have 
         forward slashes (e.g. examples/slime.bmp) and still work, but not spaces.

    sphere
    
        -radius is the sphere's radius.  It also determines mass. It should be an integer.
        -point is the starting location for the center of the sphere. It should be a vector of 
         three integers (x, y, z).
        -velocity is the starting velocity, also an integer vector.
        -color is the base color of the sphere, in RGB.  It may be overwritten by a color 
         mapped texture.
        -shading represents the coefficients of lambertian, specular, and ambient reflectance 
         for the sphere, in that order. They are floats, and should not add up to more than 1.
        -texture represents an 'index' of a texture – i.e. texture 0 refers to the first 
         texture written in the scene description, etc. A -1 here means no texture.

    light
    
        -type is the type of light, point, dir, or spot. It doesn't matter what you choose, 
         they're all point lights.
        -point is the location of the light source in the scene. It should be an integer vector.
        -velocity a vector representing the light's movement over time. Use at your own risk.
        -color is an RGB value again.

    wall
    
        -axis is one of 'x', 'y', or 'z', and represents the plane the wall is on. Y means a 
         floor or ceiling, x or z means a wall facing a certain way.
        -base is the bottom left coordinate of the wall, where 'bottom' and 'left' mean the 
         smallest numbers. Keep in mind that negative y is up.
        -width and height are the width and height of the wall. Walls extend infinitely across 
         the plane in terms of their stopping power (we'll call this a feature and not a bug), 
         but they are only displayed up to the given width and height. These should be integers.
        -Like spheres, walls have colors and shading. They work the same way.
        -invisible determines whether or not the wall is visible. Sometimes transparent walls 
         can be cool/useful. 'yes' means the wall will be invisible. Anything else means it 
         will be visible.

