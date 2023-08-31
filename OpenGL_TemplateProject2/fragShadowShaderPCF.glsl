#version 430

in vec3 vNormal, vLightDir, vVertPos, vHalfVec;
in vec4 shadow_coord;
in vec2 tc; 
out vec4 fragColor;
 
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
layout (binding=0) uniform sampler2DShadow shadowTex;
layout (binding=1) uniform sampler2D samp; //������

float lookup(float ox,float oy)
{
	float t = textureProj(shadowTex,shadow_coord +
	vec4(ox*0.001*shadow_coord.w,oy*0.001*shadow_coord.w,-0.01,0.0));//����������(-0.01)����������Ӱ�������ƫ����
	return t;
}

void main(void)
{	float shadowFactor = 0.0f;
	vec3 L = normalize(vLightDir);
	vec3 N = normalize(vNormal);
	vec3 V = normalize(-vVertPos);
	vec3 H = normalize(vHalfVec);

	//�˲�������һ��4���������������Ӱ
	float swidth = 2.5;//�ɵ�������Ӱ��ɢ��
	//����glFragCoord mod 2����4�ֲ���ģʽ�е�һ��
	vec2 offset = mod(floor(gl_FragCoord.xy),2.0)*swidth;
	shadowFactor += lookup(-1.5*swidth + offset.x,1.5*swidth-offset.y);
	shadowFactor += lookup(-1.5*swidth + offset.x,-0.5*swidth-offset.y);
	shadowFactor += lookup(0.5*swidth + offset.x,1.5*swidth-offset.y);
	shadowFactor += lookup(0.5*swidth + offset.x,-0.5*swidth-offset.y);
	shadowFactor = shadowFactor / 4.0;//shadowFactor ��4�������ƽ��ֵ

	//ȫ�ֻ�����+���ʻ�������
	vec4 color = texture(samp,tc);
	fragColor = color*globalAmbient * material.ambient
				+ color*light.ambient * material.ambient;
	
	vec4 lightedColor = color*light.diffuse * material.diffuse * max(dot(L,N),0.0)
			+ color*light.specular * material.specular
			* pow(max(dot(H,N),0.0),material.shininess*3.0);

	fragColor = vec4((fragColor.xyz + shadowFactor*(lightedColor.xyz)),1.0);
}
