#version 330

//uniform vec2 size;
uniform ivec4 colour;
uniform bool isIndexed;
uniform uint index;

in vec2 position;
in vec2 texturePosition;

void main(void)
{
    float value = clamp(1. - abs(max(position.x, position.y)), 0., 1.);
    gl_FragColor = vec4(vec3(colour.rgb) / 255., (float(colour.a) / 255.) * value);
}
