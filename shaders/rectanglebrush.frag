#version 330

uniform uvec4 colour;
uniform bool isIndexed;

in vec2 position;
in vec2 texturePosition;

out uvec4 fragment;

void main(void)
{
    float len = max(abs(position.x), abs(position.y));
    float weight = 1. - clamp(len, 0., 1.);
    fragment = uvec4(colour.rgb, uint(round(colour.a * (1. - step(1., len)))));
}
