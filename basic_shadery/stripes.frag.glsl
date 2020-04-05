#version 150 core

in vec3 Color;
in vec3 pos;
out vec4 outColor;

uniform mat4 model;
uniform vec3 stripes_dir;
uniform float time;

#define PI 3.151492

float map(float x, float min_s, float max_s, float min_d, float max_d) {
    return ((x - min_s)/(max_s - min_s))*(max_d - min_d) + min_d;
}

void main()
{
    vec4 worldStripesDir = model*vec4(stripes_dir,0);
    float f = dot(pos, worldStripesDir.xyz);
    
    f += time;

    f = map(cos(10*f), -1, 1, 0, 1);

   // f = clamp(cos(f * PI * 4.5)*2 + map(cos(time*1.4), -1, 1, -1.5, 2.45), 0, 1);
   //
   // f *= sin(length(pos) - .1);
	outColor = vec4(Color * f, 1.0f);
}
