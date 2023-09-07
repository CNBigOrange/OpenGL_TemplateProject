#version 430

out vec2 tc;

uniform float currentTime;
uniform mat4 mvp_matrix;
layout (binding = 0) uniform sampler2D tex_color;

void main(void)
{	
//由顶点着色器指定和发送控制点
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
	//为当前顶点计算合适的纹理坐标，从[-1,+1]转换到[0,1]，并传入曲面细分控制着色器
	tc = vec2((vertices[gl_VertexID].x + 1.0)/2.0, (vertices[gl_VertexID].z + 1.0)/2.0);

	gl_Position = vertices[gl_VertexID];
}