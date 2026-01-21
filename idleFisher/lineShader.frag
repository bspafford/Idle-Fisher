#version 330 core

out vec4 FragColor;  // Output color of the fragment

uniform vec4 color;

void main()
{
    FragColor = color;  // Set color to red
}