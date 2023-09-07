#version 430

out vec2 tc;

uniform float currentTime;
uniform mat4 mvp_matrix;
layout (binding = 0) uniform sampler2D tex_color;

void main(void)
{	
//�ɶ�����ɫ��ָ���ͷ��Ϳ��Ƶ�
const vec4 vertices[] =
		vec4[] (vec4(-1.0, 0.5, -1.0, 1.0),
				vec4(-0.5, 0.5, -1.0, 1.0),
				vec4( 0.5, 0.5, -1.0, 1.0),
				vec4( 1.0, 0.5, -1.0, 1.0),
				
				vec4(-1.0, sin(currentTime), -0.5, 1.0),
				vec4(-0.5, sin(currentTime+30), -0.5, 1.0),
				vec4( 0.5, sin(currentTime+20), -0.5, 1.0),
				vec4( 1.0, sin(currentTime+10), -0.5, 1.0),
				
				vec4(-1.0, sin(currentTime),  0.5, 1.0),
				vec4(-0.5, sin(currentTime+30),  0.5, 1.0),
				vec4( 0.5, sin(currentTime+20),  0.5, 1.0),
				vec4( 1.0, sin(currentTime+10),  0.5, 1.0),
				
				vec4(-1.0,-0.5,  1.0, 1.0),
				vec4(-0.5, 0.3,  1.0, 1.0),
				vec4( 0.5, 0.3,  1.0, 1.0),
				vec4( 1.0, 0.3,  1.0, 1.0));
	//Ϊ��ǰ���������ʵ��������꣬��[-1,+1]ת����[0,1]������������ϸ�ֿ�����ɫ��
	tc = vec2((vertices[gl_VertexID].x + 1.0)/2.0, (vertices[gl_VertexID].z + 1.0)/2.0);

	gl_Position = vertices[gl_VertexID];
}