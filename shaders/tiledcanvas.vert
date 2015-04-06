#version 330

uniform mat4 matrix;
uniform vec2 offset;
uniform ivec2 tilesStart;
uniform ivec2 tilesSize;
uniform usampler2D textureUnit;

in vec3 position;

out vec2 texturePosition;

void main(void)
{
    int tileX = tilesStart.x + (gl_InstanceID % tilesSize.x);
    int tileY = tilesStart.y + (gl_InstanceID / tilesSize.x);
    ivec2 size = textureSize(textureUnit, 0);
    gl_Position = matrix * (vec4(position, 1.) + vec4(tileX * size.x, tileY * size.y, 0, 0));
    texturePosition = vec4(position, 1.).xy;
}
