#version 150 core

in vec3 Color;
in vec2 pos;
out vec4 outColor;

#define ITERATION_LIMIT 400

const float PI = 3.1415926535897932384626433832795;

float scale = .27;

void main() {
    vec2 c, z;

    c = (1/scale)*pos + vec2(-.35,.75);
    z = vec2(0.0, 0.0);

    int iterations = 0;

    for(int i = 0; i < ITERATION_LIMIT; i++) {
        iterations = i;
        if ((z.x*z.x + z.y*z.y) > 4.0) break;

        float x = (z.x*z.x - z.y*z.y) + c.x;
        float y = (2.0*z.x*z.y) + c.y;

        z.x = x;
        z.y = y;
    }

    float value = (iterations != ITERATION_LIMIT-1) ? pow(float(iterations)/float(ITERATION_LIMIT), 0.35) : 0.0;

    outColor = normalize(vec4(value*Color.x, value*Color.y, value*Color.z, 1.0));
}
