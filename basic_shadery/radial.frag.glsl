#version 150 core

in vec3 Color;
in vec2 pos;
out vec4 outColor;

uniform float time;

#define PI 3.151492

float map(float x, float min_s, float max_s, float min_d, float max_d) {
    return ((x - min_s)/(max_s - min_s))*(max_d - min_d) + min_d;
}

void main()
{
    float f = 1/length(pos*2);

    f += atan(pos.x, pos.y)/(PI/2);

    f += time;

    f = clamp(cos(f * PI * 4.5)*2 + map(cos(time*1.4), -1, 1, -1.5, 2.45), 0, 1);

    f *= sin(length(pos) - .1);
	outColor = vec4(Color * f, 1.0f);
}
