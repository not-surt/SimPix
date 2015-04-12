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
    vec2 offset = fwidth(texturePosition);
    ivec2 texelPosition = ivec2(floor(texturePosition + (offset / 2) + edgeOffset));
    ivec2 samples = ivec2(1, 1);
    vec4 colour = vec4(0, 0, 0, 0);
    for (int y = 0; y < samples.y; y++) {
        for (int x = 0; x < samples.x; x++) {
            ivec2 texelPosition = ivec2(floor(mod(texturePosition + ((offset / vec2(samples)) * vec2(x + 0.5, y + 0.5)), textureSize(textureUnit, 0))));
            uvec4 texel;
            if (isIndexed) {
                uint index = texelFetch(textureUnit, texelPosition, 0).r;
                if (hasPalette) {
                    texel = texelFetch(paletteTextureUnit, ivec2(index, 0), 0);
                }
                else {
                    texel = uvec4(index, index, index, 255);
                }
            }
            else {
                texel = texelFetch(textureUnit, texelPosition, 0);
            }
            colour += vec4(texel / 255.);
        }
    }
    fragment = colour / float(samples.x * samples.y);
}
//void main(void)
//{
////    const float edgeOffset = 0.00390625;
//    const float edgeOffset = 0.;
//    ivec2 texelPosition = ivec2(floor(texturePosition + edgeOffset));
//    if (isIndexed) {
//        uint texel = texelFetch(textureUnit, texelPosition, 0).r;
//        if (hasPalette) {
//            uvec4 paletteTexel = texelFetch(paletteTextureUnit, ivec2(texel, 0), 0);
//            fragment = vec4(paletteTexel / 255.);
//        }
//        else {
//            float value = texel / 255.;
//            fragment = vec4(value);
//        }
//    }
//    else {
//        uvec4 texel = texelFetch(textureUnit, texelPosition, 0);
//        fragment = vec4(texel / 255.);
//}
