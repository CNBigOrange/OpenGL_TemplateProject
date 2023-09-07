#version 430

out vec2 tc;

uniform mat4 mvp_matrix;
layout (binding = 0) uniform sampler2D tex_color;

void main(void)
{	
	//指定4个纹理坐标
	vec2 patchTexCoords[] =
		vec2[] (vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));
	//基于当前是哪个实例计算出坐标偏移量
	// compute an offset for coordinates based on which instance this is
	int x = gl_InstanceID % 64;
	int y = gl_InstanceID / 64;
	//纹理坐标被分配进64个补丁中，并归一化到[0,1]。翻转y轴坐标
	// texture coordinates are distributed across 64 patches	
	tc = vec2( (x+patchTexCoords[gl_VertexID].x)/64.0,
			   (63-y+patchTexCoords[gl_VertexID].y)/64.0 );
	//顶点位置和纹理坐标相同，只是它的取值范围为从-0.5到+0.5
	// vertex locations range from -0.5 to +0.5
	gl_Position = vec4(tc.x-0.5, 0.0, (1.0-tc.y)-0.5, 1.0);
}