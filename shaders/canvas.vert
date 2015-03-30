#version 130

uniform mat4 matrix;
uniform mat4 textureMatrix;

attribute vec3 position;

varying vec2 texturePosition;

void main(void)
{
    gl_Position = matrix * vec4(position, 1.);
    texturePosition = (textureMatrix * vec4(position, 1.)).xy;
}
