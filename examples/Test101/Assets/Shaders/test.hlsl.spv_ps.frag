#version 330

in vec4 color;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = color;
}

