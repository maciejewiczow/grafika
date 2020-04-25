#version 150 core

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D tex1;

void main() {
	outColor = texture(tex1, TexCoord);
}