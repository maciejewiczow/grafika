#version 150 core

in vec3 position;
in vec3 color;
out vec3 Color;
out vec2 pos;

void main(){
    Color = color;
    pos = position.xy;
    gl_Position = vec4(position, 1.0);
}
