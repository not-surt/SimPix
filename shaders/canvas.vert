#version 130

uniform mat4 matrix;
uniform vec2 offset;

in vec3 position;

out vec2 texturePosition;

void main(void)
{
    gl_Position = matrix * (vec4(position, 1.) + vec4(offset, 0., 0.));
    texturePosition = vec4(position, 1.).xy;
}
