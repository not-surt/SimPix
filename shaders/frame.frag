#version 130

uniform ivec4 colour;

void main(void)
{
    gl_FragColor = vec4(colour / 255.);
}
