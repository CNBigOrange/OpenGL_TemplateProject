#version 430

layout (triangles) in;

in vec3 varyingNormal[];
in vec3 varyingLightDir[];
in vec3 varyingHalfVector[];
in vec3 varyingVertPos[];

out vec3 varyingNormalG;
out vec3 varyingLightDirG;
out vec3 varyingHalfVectorG;
out vec3 varingVertPosG;

layout (triangle_strip, max_vertices=9) out;

struct PositionalLight
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
struct Material
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform int enableLighting;
//���������棬9������
vec3 newPoints[9],lightDir[9];
float sLen = 0.01;//sLen ��С����׶�ĸ߶�

void setOutputValues(int p,vec3 norm)
{
	varyingNormalG = norm;
	varyingLightDirG = lightDir[p];
	varingVertPosG = newPoints[p];
	varyingHalfVectorG = normalize(varyingLightDirG) + normalize(-newPoints[p]);
	gl_Position =  proj_matrix * vec4(newPoints[p],1.0);

}

void makeNewTriangle(int p1,int p2)
{
	//Ϊ������������ɱ��淨����
	vec3 c1 = normalize(newPoints[p1] - newPoints[3]);
	vec3 c2 = normalize(newPoints[p2] - newPoints[3]);
	//����������淨����
	vec3 norm = cross(c1,c2);

	//���ɲ�����3������
	setOutputValues(p1,norm);
	EmitVertex();
	setOutputValues(p2,norm);
	EmitVertex();
	setOutputValues(3,norm);
	EmitVertex();
	EndPrimitive();
}

void main (void)
{	
	//��3�������ζ������ԭʼ���淨����
	vec3 sp0 = gl_in[0].gl_Position.xyz + varyingNormal[0]*sLen;
	vec3 sp1 = gl_in[1].gl_Position.xyz + varyingNormal[1]*sLen;
	vec3 sp2 = gl_in[2].gl_Position.xyz + varyingNormal[2]*sLen;

	//�������С����׶���µ�
	newPoints[0] = gl_in[0].gl_Position.xyz;
	newPoints[1] = gl_in[1].gl_Position.xyz;
	newPoints[2] = gl_in[2].gl_Position.xyz;
	newPoints[3] = (sp0 + sp1 + sp2 )/3.0; //�����Ķ���

	//����Ӷ��㵽���յķ���
	lightDir[0] = light.position - newPoints[0];
	lightDir[1] = light.position - newPoints[1];
	lightDir[2] = light.position - newPoints[2];
	lightDir[3] = light.position - newPoints[3];

	//����3�������Σ������С����׶�ı���
	makeNewTriangle(0,1);     //����������Զ�Ƕ�
	makeNewTriangle(1,2);
	makeNewTriangle(2,0);
}
