#version 130
#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0) out vec4 FragColor;

in vec2 UV;

const vec3 _red_coef_709 = vec3( 1.5748, 1.0,  0.0000);
const vec3 _grn_coef_709 = vec3(-0.4681, 1.0, -0.1873);
const vec3 _blu_coef_709 = vec3( 0.0000, 1.0,  1.8556);

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
    vec4 _tmp0;
    _tmp0.yw = texture(Texture0, UV).xy;
    _tmp0.xz = texture(Texture1, UV).yx;
    _tmp0.xz = _tmp0.xz * 1.003922 - 0.503929;
    vec4 c;
    c.r = dot(_tmp0.xyz, _red_coef_709);
    c.g = dot(_tmp0.xyz, _grn_coef_709);
    c.b = dot(_tmp0.xyz, _blu_coef_709);
    c.a = _tmp0.a;
    FragColor = c;
}
