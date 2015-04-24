#version 330

uniform bool isIndexed;
uniform uint index;
uniform uvec4 colour;

in vec2 texturePosition;

layout(location = 0) out uint indexFragment;
layout(location = 1) out uvec4 fragment;

void main(void)
{
    if (max(abs(texturePosition.x), abs(texturePosition.y)) > 1.) discard;
    else if (isIndexed) {
        indexFragment = index;
    }
    else {
        fragment = colour;
    }
}
