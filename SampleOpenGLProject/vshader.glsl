#version 150

in  vec4 vPosition;
in  vec4 vColor;
flat out vec4 color;

void main() 
{
    color = vColor;
    gl_Position = vPosition;
} 
