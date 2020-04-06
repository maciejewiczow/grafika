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

float h(float x, float period, float peakToValleyRatio) {
    x = mod(x, period);
    
    if (x <= (1- peakToValleyRatio)*period) 
        return 0;
    else 
        return x;
}

void main() {
    vec4 worldStripesDir = model*vec4(stripes_dir, 0);
    float f = dot(pos, worldStripesDir.xyz);
    
    f += time;

    f = clamp(8*h(f, 1., 0.1) , 0, 10);
    f = map(f, 0, 10, 0.06, 1);

	outColor = vec4(Color * f, 1.0f);
}
