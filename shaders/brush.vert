#version 330

uniform mat4 matrix;

in vec2 position;

void main(void)
{
    gl_Position = matrix * vec4(position, 0, 1.);
}
