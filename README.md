# 3D

3D rendering/game engine using OpenGL by Przemek Malon.

![Engine Image](https://github.com/przemektmalon/3D/blob/master/gitimg/engineimg.png)

To build:

Ensure you have all dependancies built on your system (listed at the bottom of this file).

Clone the repo. 

Either open the MSVC solution and amend the usual project properties (include directories, library directories) to point to paths on your system.

Through cmake, launch cmake-gui, open the LIB properties group, input the correct library paths, configure, generate, and build.

CMake process tested with MSVC and GCC. GCC compiles and links, however, the app crashes. Most likely because the GCC compiled 64bit code while libs are 32bit. I might fix this in the future, for the time being though, MSVC works.

# Current features

-Deferred HDR rendering using half-z tile light culling through OGL compute shader. Supports normal and tone mapping. Uses metallic and roughness maps for PBR calculations.

-Realistic PBR lighting model

-Frustum culling on GPU compute shader

-Shader hot-loading & preprocessing

-Partial bullet physics library integration

-Ambient Occlusion

-LOD system

-Blended multi-texturing (splatting) using alpha map, up to four textures.

-Multi material models (any number of triangle lists with different materials in one mesh)

-Logarithmic depth buffer (large precision allows for very large scenes with no z clipping)

-Text console (define commands that take numbers, vectors and strings)

-Scene graph/hierarchical tranformations

-Dynamic lighting (spot, point, directional)

-Shadows for point, spot and directional lights.

-Tweak files for updating engine variables at runtime automatically from a file

-Some basic UI elements (window, labels, buttons, sliders, tabs, input fields)

-Asset manager (models, fonts, textures)

-Text rendering

-Modern OpenGL approach

# Planned features

## Render Engine

-Heightmap tesselation

-Light rays

-Global illumination and lightmap baking

-Translucent shadow maps

-Anti-aliasing

-Bloom

-Image based lighting

-Emissive and parallax mapping

-Depth of field

-Fog

-Particle system

-Water

-Reflections

-Skeletal animation and inverse kinematics

## Game Engine

-Scripting language (or at least support for one)

-Markup language for UI design

-Terrain system

-Day/night cycles

-Character system

## General

-Networking

-Platform independence

# Dependencies

-Assimp

-Bullet Physics

-FreeType

-Boost

-rapidxml

-GLM

-GLEW

-SOIL

-DevIL
