#version 330

uniform uvec4 colour;
uniform bool isIndexed;

in vec2 texturePosition;

out uvec4 fragment;

void main(void)
{
    if (length(texturePosition) > 1.) discard;
    else fragment = colour;
}
