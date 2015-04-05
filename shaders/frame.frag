#version 130

flat in ivec4 colour;

void main(void)
{
    gl_FragColor = vec4(colour / 255.);
}
