#version 430

in vec2 tc;
out vec4 fragColor;

uniform mat4 v_matrix;
uniform mat4 p_matrix;
layout (binding = 0) uniform sampler2D s;

void main(void)
{
	fragColor = texture(s,tc);
}
