# Set of OpenGL demos

## Parallax Occlusion Mapping
(with soft shadows)

According to http://sunandblackcat.com/tipFullView.php?topicid=28

![parallax occlusion mapping](./screenshots/parallax_1.png)
![parallax occlusion mapping shadows](./screenshots/parallax_2.png)

## Screen Space Ambient Occlusion

According to https://learnopengl.com/Advanced-Lighting/SSAO

![screen space ambient occlusion](./screenshots/ssao.png)

## Physically Based Rendering

According to https://learnopengl.com/PBR/Theory

![screen space ambient occlusion](./screenshots/pbr.png)

## Reflective Shadow Maps
According to this paper http://www.klayge.org/material/3_12/GI/rsm.pdf

Generally it works, but I was not able to get rid of some of the artifacts.

![screen space ambient occlusion](./screenshots/rsm.png)

Environment cubemaps were taken from [sIBL archive](http://www.hdrlabs.com/sibl/archive.html).
PBR materials were taken from [FreePBR.com](https://freepbr.com/).

# Setting up

This project requires OpenGL, SDL2, GLEW, OpenGL Mathematics
and Cmake to be installed in your system. Once there use cmake to generate a project
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
