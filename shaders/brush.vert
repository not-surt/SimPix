#version 330

uniform mat4 matrix;
uniform mat4 projectionMatrix;
uniform ivec2 tilesStart;
uniform ivec2 tilesSize;
uniform ivec2 imageSize;

in vec2 position;
out vec2 texturePosition;

void main(void)
{
    gl_Position = projectionMatrix * ((matrix * vec4(position, 0, 1)) - vec4(vec2(tilesStart.x + (gl_InstanceID % tilesSize.x), tilesStart.y + (gl_InstanceID / tilesSize.x)) * imageSize, 0, 0));
    texturePosition = position;
}
