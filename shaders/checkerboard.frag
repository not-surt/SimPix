#version 330

uniform ivec4 colour0, colour1;
uniform vec2 size;

in vec2 texturePosition;

void main(void)
{
    ivec4 colour = !((mod(texturePosition.x, size.x) > (size.x / 2.)) != !(mod(texturePosition.y, size.y) > (size.y / 2.))) ? colour1 : colour0;
    gl_FragColor = colour / 255.;
}
