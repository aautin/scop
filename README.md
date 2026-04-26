# scop

Scop is a project of 42 school. It is a 3D graphic project using OpenGL and C++. The goal of the project is to create a 3D model viewer that can load and display 3D objects from .obj files (and .mtl).

The viewer should also be able to:
- apply textures to the objects
- allow the user to manipulate objects by rotating them on their 3 axis
- zoom in and out

## Features
- Parsing of .obj vertices and faces
- No optimization with index buffers, each face is drawn as a separate triangle
- Escape key to exit the program
- Each vertex has is own color, so each triangle is a shading of the its vertices colors
- Vertex shader and fragment shader to handle the rendering of the models