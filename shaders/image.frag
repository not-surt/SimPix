#version 330

uniform usampler2D textureUnit;
uniform bool isIndexed;
uniform bool hasPalette;
uniform usampler2D paletteTextureUnit;

in vec2 texturePosition;

out vec4 fragment;

void main(void)
{
//    const float edgeOffset = 0.00390625;
    const float edgeOffset = 0.;
    ivec2 texelPosition = ivec2(floor(texturePosition + edgeOffset));
    if (isIndexed) {
        uint texel = texelFetch(textureUnit, texelPosition, 0).r;
        if (hasPalette) {
            uvec4 paletteTexel = texelFetch(paletteTextureUnit, ivec2(texel, 0), 0);
            fragment = vec4(paletteTexel / 255.);
        }
        else {
            float value = texel / 255.;
            fragment = vec4(value);
        }
    }
    else {
        uvec4 texel = texelFetch(textureUnit, texelPosition, 0);
        fragment = vec4(texel / 255.);
    }
}
