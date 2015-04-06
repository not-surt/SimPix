#version 330

uniform mat4 matrix;
uniform vec2 offset;
uniform ivec2 tilesStart;
uniform ivec2 tilesSize;
uniform usampler2D textureUnit;

in vec2 position;
out vec2 texturePosition;

void main(void)
{
    ivec2 size = textureSize(textureUnit, 0);
    gl_Position = matrix * vec4(position + vec2((tilesStart.x + (gl_InstanceID % tilesSize.x)) * size.x, (tilesStart.y + (gl_InstanceID / tilesSize.x)) * size.y), 0, 1);
    texturePosition = position;
}
