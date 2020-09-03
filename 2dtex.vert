#version 130
#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 UV;
out vec4 Color;

void main()
{
    UV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
