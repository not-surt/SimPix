#version 330

#define M_PI 3.14159265358979323846
#define M_TAU (2 * 3.14159265358979323846)

const float size = 8;
flat in ivec4 colour;

out vec4 fragment;

void main(void)
{
    fragment = vec4(colour / 255.);
//    gl_FragColor = int(floor(gl_FragCoord.x) + floor(gl_FragCoord.y)) % 2 == 0 ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
//    gl_FragColor = int(floor(gl_FragCoord.x) + floor(gl_FragCoord.y)) / 4 % 2 == 0 ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
//    float sinX = sin(gl_FragCoord.x * M_TAU / size);
//    float sinY = sin(gl_FragCoord.y * M_TAU / size);
//    gl_FragColor = (sinX + sinY) >= 0 ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}
