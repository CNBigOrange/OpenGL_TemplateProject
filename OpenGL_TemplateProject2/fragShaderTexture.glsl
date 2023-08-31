#version 430
in vec2 tc;  //输入插值过的纹理坐标
out vec4 color;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout(binding=0) uniform sampler2D samp;

void main(void)
{
    color = texture(samp,tc);
}