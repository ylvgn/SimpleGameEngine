#version 330

layout(location = 0) in vec4 i_positionOS;
layout(location = 1) in vec4 i_color;
out vec4 color;

void main()
{
    gl_Position = i_positionOS;
    color = i_color;
}

