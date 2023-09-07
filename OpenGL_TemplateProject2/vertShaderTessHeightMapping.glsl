#version 430

out vec2 tc;

uniform mat4 mvp_matrix;
layout (binding = 0) uniform sampler2D tex_color;

void main(void)
{	
	//ָ��4����������
	vec2 patchTexCoords[] =
		vec2[] (vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));
	//���ڵ�ǰ���ĸ�ʵ�����������ƫ����
	// compute an offset for coordinates based on which instance this is
	int x = gl_InstanceID % 64;
	int y = gl_InstanceID / 64;
	//�������걻�����64�������У�����һ����[0,1]����תy������
	// texture coordinates are distributed across 64 patches	
	tc = vec2( (x+patchTexCoords[gl_VertexID].x)/64.0,
			   (63-y+patchTexCoords[gl_VertexID].y)/64.0 );
	//����λ�ú�����������ͬ��ֻ������ȡֵ��ΧΪ��-0.5��+0.5
	// vertex locations range from -0.5 to +0.5
	gl_Position = vec4(tc.x-0.5, 0.0, (1.0-tc.y)-0.5, 1.0);
}