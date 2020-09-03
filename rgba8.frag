#version 130
#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0) out vec4 FragColor;

in vec2 UV;

uniform sampler2D Texture0;

void main()
{
    FragColor = texture(Texture0, UV);
}
