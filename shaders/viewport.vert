#version 330

uniform mat4 matrix;
uniform vec2 offset;

in vec2 position;
out vec2 texturePosition;

void main(void)
{
    gl_Position = matrix * vec4(position, 0., 1.);
    texturePosition = position;
}
