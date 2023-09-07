#version 430

layout (vertices = 4) out;

in vec2 tc[];
out vec2 tcs_out[];

uniform mat4 mvp_matrix;
layout (binding = 0) uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_height;
layout (binding = 2) uniform sampler2D tex_normal;

/*--- light stuff----*/
struct PositionalLight
{	vec4 ambient; vec4 diffuse; vec4 specular; vec3 position; };
struct Material
{	vec4 ambient; vec4 diffuse; vec4 specular; float shininess; };
uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
/*-----------------*/

void main(void)
{	
	float subdivisions = 32.0;//���ڸ߶�ͼ��ϸ���ܶȵĿɵ����ĳ���
	if (gl_InvocationID == 0)
	{ 	vec4 p0 = mvp_matrix * gl_in[0].gl_Position;  //��Ļ�ռ��п��Ƶ��λ��
	p0 = p0 / p0.w;
		vec4 p1 = mvp_matrix * gl_in[1].gl_Position;  
		p1 = p1 / p1.w;
		vec4 p2 = mvp_matrix * gl_in[2].gl_Position;  
		p2 = p2 / p2.w;
		float width  = length(p1-p0) * subdivisions + 1.0;//����ϸ������ĸ�֪����ȡ�
		float height = length(p2-p0) * subdivisions + 1.0;//����ϸ������ĸ�֪���߶ȡ�
		gl_TessLevelOuter[0] = height;//���ڸ�֪�ı߳���������ϸ�ּ���
		gl_TessLevelOuter[1] = width;
		gl_TessLevelOuter[2] = height;
		gl_TessLevelOuter[3] = width;
		gl_TessLevelInner[0] = width;
		gl_TessLevelInner[1] = height;
	}

	tcs_out[gl_InvocationID] = tc[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}