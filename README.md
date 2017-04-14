# 3D

3D rendering/game engine using OpenGL by Przemek Malon.
Some screenshots on Google Drive: https://drive.google.com/open?id=0B9uuq4IEinJPTzk5WXRmb3h3Mjg

# Current features

-Deferred HDR rendering using half-z tile light culling through OGL compute shader. Supports normal & specular mapping, and tonemapping. 
-Frustum culling on GPU compute shader
-Shader hot-loading & preprocessing
-Ambient Occlusion
-Blended multi-texturing using alpha map, up to four textures.
-Multi material meshes (any number of triangle lists with different materials in one mesh)
-Logarithmic depth buffer (large precision allows for very large scenes with no z clipping)
-Easily programmable text console (define commands that take numbers, vectors and strings)
-Mouse object selection
-Scene graph/hierarchical tranformations
-Dynamic lighting (spot, point, directional)
-Shadows for spot and directional lights (point light shadows soon)
-Some basic UI elements (window, labels, buttons)
-Asset manager (meshes, fonts, textures)
-Text rendering
-Modern OpenGL approach

# Planned features

-Depth of field
-LOD scaling for meshes
-Emissive and parallax mapping
-Fog
-Particle system
-Water
-Reflections
-Day/night cycles
-Scripting language (or at least support for one)
-Markup language for UI design
-Skeletal animation and inverse kinematics
-Physics engine
