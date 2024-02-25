#version 330 core

// input from the vertex shader
in vec3 ourColor;
// output to the buffer
out vec4 FragColor;

void main()
{
   FragColor = vec4(ourColor, 1.0); // RGB + alpha=1.0
}