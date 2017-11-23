# 3D

3D rendering/game engine using OpenGL by Przemek Malon.

![Engine Image](https://github.com/przemektmalon/3D/blob/master/gitimg/engine.png)

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

-Some basic UI elements (window, labels, buttons)

-Asset manager (models, fonts, textures)

-Text rendering

-Modern OpenGL approach

# Planned features

-Heightmap tesselation

-Depth of field

-Emissive and parallax mapping

-Fog

-Particle system

-Water

-Reflections

-Day/night cycles

-Scripting language (or at least support for one)

-Markup language for UI design

-Skeletal animation and inverse kinematics

# Dependencies

-Assimp

-Bullet Physics

-FreeType

-Boost

-GLM

-GLEW

