# Set of OpenGL demos

## Parallax Occlusion Mapping

A relatively simple technique that adds a depth and shadows to a
flat texture without adding additional vertices.

According to http://sunandblackcat.com/tipFullView.php?topicid=28

![parallax occlusion mapping](./screenshots/parallax.png)

## Screen Space Ambient Occlusion

A technique for efficiently approximating the ambient occlusion effect in real time.

According to https://learnopengl.com/Advanced-Lighting/SSAO

![screen space ambient occlusion](./screenshots/ssao.png)

## Physically Based Rendering

Shading model seeks to render graphics in a way that more accurately
models the flow of light in the real world.

According to https://learnopengl.com/PBR/Theory

![Physically Based Rendering](./screenshots/pbr.png)

Environment cubemaps were taken from [sIBL archive](http://www.hdrlabs.com/sibl/archive.html).
PBR materials were taken from [FreePBR.com](https://freepbr.com/).

## Reflective Shadow Maps

An algorithm for interactive rendering of plausible indirect illumination.
It is an extension to a standard shadow map, where every
pixel is considered as an indirect light source.

According to this paper http://www.klayge.org/material/3_12/GI/rsm.pdf

I have not implemented final interpolation step (with recalculation of
global illumination at edges), as described in the paper.
Instead I simply interpolated illumination from low-res texture for
whole screen. This leads to blurry halos around the objects.

![Reflective Shadow Maps](./screenshots/rsm.png)

# Setting up

This project requires OpenGL, SDL2, GLEW, OpenGL Mathematics
and Cmake to be installed in your system. Once there, use cmake to generate a project
for your favorite IDE. In case of unix systems and `make` building should be as simple as

```bash
mkdir build
cd ./build
cmake ..
make
```

This will generate binaries for all subprojects in corresponding subfolders
insinde `bin` folder.

Note that shaders code placed straight inside `bin/<project_name>` for simplicity.

Keep in mind that depending on your version of `cmake` and/or different versions of "find" modules,
libraries path constant name may change a bit. For example, some versions placing found sdl path to 
`SDL2_LIBRARY` and `SDL2_INCLUDE_DIR`, while others use `SDL2_LIBRARIES` and `SDL2_INCLUDE_DIRS`.
Take a look into `Modules/FindSDL2.cmake`(and other used scripts) in your cmake folder to get proper
constant name, and use those in the [base project](https://github.com/daniilsunyaev/shaders/blob/master/src/base/CMakeLists.txt#L11-L15).

# Controls

- use `w,a,s,d` for navigation;
- click anywhere in the window to toggle mouse look;
- use mouse scroll wheel to zoom in / zoom out.
