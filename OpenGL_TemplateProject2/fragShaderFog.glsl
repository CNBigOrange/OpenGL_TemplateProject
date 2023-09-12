#version 430

in vec3 vertEyeSpacePos;
in vec2 tc;
out vec4 fragColor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D t;	// for texture
layout (binding=1) uniform sampler2D h;	// for height map

void main(void)
{	vec4 fogColor = vec4(0.7, 0.8, 0.9, 1.0);	// bluish gray����ɫ����ɫ
	//�����ɫ���ɵ���ɫ�ķ�Χ
	float fogStart = 6.5;
	float fogEnd = 8.5;

	// the distance from the camera to the vertex in eye space is simply the length of a
    // vector to that vertex, because the camera is at (0,0,0) in eye space.
	//���Ӿ��ռ��д����������ľ�����ǵ��������������ĳ��ȣ���Ϊ������Ӿ��ռ��е�(0,0,0)λ��
	float dist = length(vertEyeSpacePos.xyz - vec3(0.0,0.0,0.0));
	//ǯ����0~1֮��
	float fogFactor = clamp(((fogEnd-dist)/(fogEnd-fogStart)), 0.0, 1.0);
	//����fogFactor���ؼ�Ȩƽ��ֵ
	fragColor = mix(fogColor,(texture(t,tc)),fogFactor);
}
