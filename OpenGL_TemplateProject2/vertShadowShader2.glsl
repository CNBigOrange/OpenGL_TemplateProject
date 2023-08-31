#version 430

layout (location=0) in vec3 vertPos;
layout(location=1) in vec2 texCoord;
layout (location=2) in vec3 vertNormal;
out vec2 tc;  //纹理坐标输出到光栅着色器用于插值

out vec3 vNormal, vLightDir, vVertPos, vHalfVec; 
out vec4 shadow_coord;

struct PositionalLight
{	vec4 ambient, diffuse, specular;
	vec3 position;
};
struct Material
{	vec4 ambient, diffuse, specular;   
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP;
//阴影纹理的采样器
layout (binding=0) uniform sampler2DShadow shadowTex;
//常规纹理采样
layout (binding=1) uniform sampler2D samp; //采样器

void main(void)
{	//output the vertex position to the rasterizer for interpolation
	//输出顶点到光栅着色器进行插值
	vVertPos = (mv_matrix * vec4(vertPos,1.0)).xyz;
        
	//get a vector from the vertex to the light and output it to the rasterizer for interpolation
	//获得一个顶点到灯光的向量，再输出到光栅进行插值
	vLightDir = light.position - vVertPos;

	//get a vertex normal vector in eye space and output it to the rasterizer for interpolation
	vNormal = (norm_matrix * vec4(vertNormal,1.0)).xyz;
	
	// calculate the half vector (L+V)
	vHalfVec = (vLightDir-vVertPos).xyz;
	
	shadow_coord = shadowMVP * vec4(vertPos,1.0);
	
	gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);

	tc = texCoord;
}
