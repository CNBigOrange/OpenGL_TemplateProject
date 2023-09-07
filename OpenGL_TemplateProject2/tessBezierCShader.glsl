#version 430

in vec2 tc[];
out vec2 tcs_out[];//以标量形式从顶点着色器传来的纹理坐标输出，以数组形式被接受，然后被发送给曲面细分评估着色器

uniform mat4 mvp_matrix;
layout (binding=0) uniform sampler2D tex_color;
layout (vertices = 16) out;//每个顶点有16个控制点

void main(void)
{	int TL = 32;  // tessellation levels 曲面细分级别都被设置为32
//曲面细分级别只需要指定一次，因此在第0次调用时才执行
	if (gl_InvocationID ==0)
	{	gl_TessLevelOuter[0] = TL;
		gl_TessLevelOuter[2] = TL;
		gl_TessLevelOuter[1] = TL;
		gl_TessLevelOuter[3] = TL;
		gl_TessLevelInner[0] = TL;
		gl_TessLevelInner[1] = TL;
	}
	//将纹理和控制点传递给曲面细分评估着色器
	tcs_out[gl_InvocationID] = tc[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}