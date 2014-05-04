#version 130

uniform usampler2D textureUnit;

varying vec2 texturePosition;

void main(void)
{
    ivec2 texelPosition = ivec2(floor(mod(texturePosition, textureSize(textureUnit, 0))));
    uvec4 texel = texelFetch(textureUnit, texelPosition, 0);
    gl_FragColor = vec4(texel / 255.);
}
