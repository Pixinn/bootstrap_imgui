#version 330 core

// input from the vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// output to the fragment shader
out vec3 ourColor;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   ourColor = aColor;
}