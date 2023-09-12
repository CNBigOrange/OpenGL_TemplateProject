#version 430

layout (triangles) in;

in vec3 varyingNormal[];
in vec3 varyingLightDir[];
in vec3 varyingHalfVector[];
in vec3 varyingVertPos[];

out vec3 varyingNormalG;
out vec3 varyingLightDirG;
out vec3 varyingHalfVectorG;

layout (triangle_strip, max_vertices=3) out;

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

void main (void)
{	
	vec4 triangleNormal = vec4(((varyingNormal[0]+varyingNormal[1]+varyingNormal[2])/3.0),1.0);
	//如果被三整除则忽略图元
	//if(mod(gl_PrimitiveIDIn,3)!=0){
		for (int i=0; i<3; i++)
		{	
			//if(gl_in[i].gl_Position.y>0 && gl_in[i].gl_Position.x>0){
			//gl_Position = gl_in[i].gl_Position + normalize(vec4(varyingNormal[i],1.0))*100.4;
			//}
			//else{
			//gl_Position = gl_in[i].gl_Position;
			//}
		
			gl_Position = gl_in[i].gl_Position + normalize(triangleNormal)*100.0;
		
			varyingNormalG = varyingNormal[i];
			varyingLightDirG = varyingLightDir[i];
			varyingHalfVectorG = varyingHalfVector[i];
			EmitVertex();
		}
	//}
	EndPrimitive();
}
