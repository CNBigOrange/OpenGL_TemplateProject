#version 430
layout(location=0) in vec3 position;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf; //用于动画和放置立方体的时间因子

out vec4 varyingColor;

mat4 buildRotateY(float rad);
mat4 buildRotateX(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main(void)
{
    float i = gl_InstanceID + tf;
    float a = sin(203.0*i/3000)*203.0;
    float b = sin(301.5*i/2001)*201.0;
    float c = sin(400.0*i/4003)*205.0;

    //构建旋转和平移矩阵，将会应用于当前立方体的模型矩阵
    mat4 localRotX = buildRotateX(5*i);
    mat4 localRotY = buildRotateY(5*i);
    mat4 localRotZ = buildRotateZ(5*i);
    mat4 localTrans = buildTranslate(a,b,c);

    //构建模型矩阵，然后构建MV矩阵
    mat4 newM_matrix = localTrans*localRotX*localRotY*localRotZ;
    mat4 mv_matrix = v_matrix*newM_matrix;

    gl_Position = proj_matrix*mv_matrix*vec4(position,1.0);
    varyingColor = vec4(position,1.0)*0.5+vec4(0.5,0.5,0.5,0.5);
}

//构建并返回平移矩阵
mat4 buildTranslate(float x, float y, float z)
{
	mat4 trans = mat4(1.0, 0.0, 0.0, 0.0,//注意：这是最左列，而非第一行
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					x, y, z, 1.0);
	return trans;
}
//构建并返回绕x轴的旋转矩阵
mat4 buildRotateX(float rad)
{
	mat4 xrot = mat4(1.0, 0.0, 0.0, 0.0,//注意：这是最左列，而非第一行
		0.0, cos(rad), -sin(rad), 0.0,
		0.0, sin(rad), cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0);
	return xrot;
}
//构建并返回y轴的旋转举证
mat4 buildRotateY(float rad)
{
	mat4 yrot = mat4(cos(rad), 0.0, sin(rad), 0.0,
					0.0, 1.0, 0.0, 0.0,
					-sin(rad), 0.0, cos(rad), 0.0,
					0.0, 0.0, 0.0, 1.0);
	return yrot;
}
//构建并返回绕z轴的旋转矩阵
mat4 buildRotateZ(float rad)
{
	mat4 zrot = mat4(cos(rad), -sin(rad), 0.0, 0.0,
				sin(rad), cos(rad), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0);
	return zrot;
}