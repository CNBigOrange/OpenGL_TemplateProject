#version 430

in vec2 tc;
in float altitude;
out vec4 fragColor;

uniform mat4 v_matrix;
uniform mat4 p_matrix;
uniform int isAbove;
layout (binding = 0) uniform sampler2D s;

void main(void)
{
	if ((altitude < .47) && (isAbove == 0)) fragColor = vec4(0,0,.2,1);
	else fragColor = texture(s,tc);
}
