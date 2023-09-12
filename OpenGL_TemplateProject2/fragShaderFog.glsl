#version 430

in vec3 vertEyeSpacePos;
in vec2 tc;
out vec4 fragColor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D t;	// for texture
layout (binding=1) uniform sampler2D h;	// for height map

void main(void)
{	vec4 fogColor = vec4(0.7, 0.8, 0.9, 1.0);	// bluish gray蓝灰色，雾色
	//输出颜色过渡到雾色的范围
	float fogStart = 6.5;
	float fogEnd = 8.5;

	// the distance from the camera to the vertex in eye space is simply the length of a
    // vector to that vertex, because the camera is at (0,0,0) in eye space.
	//在视觉空间中从相机到顶点的距离就是到这个顶点的向量的长度，因为相机在视觉空间中的(0,0,0)位置
	float dist = length(vertEyeSpacePos.xyz - vec3(0.0,0.0,0.0));
	//钳制在0~1之间
	float fogFactor = clamp(((fogEnd-dist)/(fogEnd-fogStart)), 0.0, 1.0);
	//根据fogFactor返回加权平均值
	fragColor = mix(fogColor,(texture(t,tc)),fogFactor);
}
