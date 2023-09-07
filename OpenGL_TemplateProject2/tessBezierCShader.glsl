#version 430

in vec2 tc[];
out vec2 tcs_out[];//�Ա�����ʽ�Ӷ�����ɫ�����������������������������ʽ�����ܣ�Ȼ�󱻷��͸�����ϸ��������ɫ��

uniform mat4 mvp_matrix;
layout (binding=0) uniform sampler2D tex_color;
layout (vertices = 16) out;//ÿ��������16�����Ƶ�

void main(void)
{	int TL = 32;  // tessellation levels ����ϸ�ּ��𶼱�����Ϊ32
//����ϸ�ּ���ֻ��Ҫָ��һ�Σ�����ڵ�0�ε���ʱ��ִ��
	if (gl_InvocationID ==0)
	{	gl_TessLevelOuter[0] = TL;
		gl_TessLevelOuter[2] = TL;
		gl_TessLevelOuter[1] = TL;
		gl_TessLevelOuter[3] = TL;
		gl_TessLevelInner[0] = TL;
		gl_TessLevelInner[1] = TL;
	}
	//������Ϳ��Ƶ㴫�ݸ�����ϸ��������ɫ��
	tcs_out[gl_InvocationID] = tc[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}