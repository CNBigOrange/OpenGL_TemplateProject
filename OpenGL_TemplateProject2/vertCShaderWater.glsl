#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
out vec2 tc;
out float altitude;

uniform mat4 v_matrix;
uniform mat4 p_matrix;
uniform int isAbove;
layout (binding = 0) uniform sampler2D s;

void main(void)
{
	tc = tex_coord;
	gl_Position = p_matrix * v_matrix * vec4(position,1.0);
	altitude = gl_Position.y;
} 
