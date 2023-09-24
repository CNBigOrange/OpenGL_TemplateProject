#version 430

layout (local_size_x=1) in;//�������СΪ1
layout (binding=0, rgba8) uniform image2D output_texture;
float camera_pos = 5.0;

struct Ray//�������
{	vec3 start;	// origin of the ray ����ԭ��
	vec3 dir;	// normalized direction of the ray ��һ����Ĺ��߷���
};

//��������
float sphere_radius = 2.5;
vec3 sphere_position = vec3(1.0, 0.0, -3.0);
vec3 sphere_color = vec3(1.0, 0.0, 0.0); // red

//����������
vec3 box_mins = vec3(-2.0, -2.0, 0.0);
vec3 box_maxs = vec3(-0.5, 1.0, 2.0);
vec3 box_color = vec3(0.0, 1.0, 0.0); // green

//ADS����
vec4 worldAmb_ambient = vec4(0.3, 0.3, 0.3, 1.0);

vec4 objMat_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 objMat_diffuse = vec4(0.7, 0.7, 0.7, 1.0);
vec4 objMat_specular = vec4(1.0, 1.0, 1.0, 1.0);
float objMat_shininess = 50.0;

vec3 pointLight_position = vec3(-3.0, 2.0 + 5 * sin(time), 4.0);
vec4 pointLight_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 pointLight_diffuse = vec4(0.7, 0.7, 0.7, 1.0);
vec4 pointLight_specular = vec4(1.0, 1.0, 1.0, 1.0);

struct Collision
{	float t;	// value at which this collision occurs for a ray �ڹ����Ͼ���ײλ�õľ���
	vec3 p;		// The world position of the collision ��ײ���ȫ������
	vec3 n;		// the normal of the collision ��ײ��ı��淨����
	bool inside;	// Whether the ray started inside the object and collided while exiting ��ײ���Ƿ����ڴ��������ڱ��������뿪����ʱ������
	int object_index;	// The index of the object this collision hit ��ײ�������������
};

//------------------------------------------------------------------------------
// Checks if Ray r intersects the Box defined by Object o.box
// This implementation is based on the following algorithm:
// http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/basic_algo.pdf
/*
������������Ľ����㷨�� Slabs�㷨����2D�Ƶ���3D
�������ߵ�ԭ��Rs�ͷ���Rd���Լ����ĵ�λ��Sp������뾶Sr�����ߺ����彻��λ��P
���߷��̣�Rs + t.Rd = P ������λ�� = �ӹ������λ�� + ��һ���Ĺ��߷��� * ���룩
ʹ��������Խ�������Ե��������������������壺�� Xmin��Ymin��Zmin���ͣ�Xmax��Ymax��Zmax��
���ǿ���ʹ������������ȷ��������������棬�����������xֵ��ȷ�������������ƽ����yzƽ�����
�������������������С��Ľ��㣺BOXmin = Rs + Tmins*Rd �任���̣�Tmins = ��BOXmins - Rs��/Rd
BOXmax = Rs + Tmax*Rd �任���̣�Tmax = ��BOXmax - Rs��/Rd
BOXmins��x��y��z��BOXmax��x��y��z�ֱ��Ӧ��������ƽ���Զ������ƽ��

����������������������� ƽ���� ��Զ������Ĺ�ϵ���жϹ����Ƿ����������ཻ��
��t_near>t_far��t_far<=0ʱ�����߸��������������ཻ��
��t_near<0��t_far>0ʱ��˵��t_near�ڹ��ߵķ�����t_far�ڹ��ߵ������򣬹����Ǵ��������ڲ�������뿪������ģ���һ����ײ��
������������ײ�㷢���ھ����t_near��t_far��
*/
//------------------------------------------------------------------------------
Collision intersect_box_object(Ray r)
{	// Calculate the box's world mins and maxs:
	vec3 t_min = (box_mins - r.start) / r.dir;
	vec3 t_max = (box_maxs - r.start) / r.dir;
	//GLSL �ж�����vec3����ֵӦ��min���������᷵��һ��vec3����ֵ�����а���ÿ��x��y��zԪ���н�С��һ����max��������ͬ��
	vec3 t_minDist = min(t_min, t_max);
	vec3 t_maxDist = max(t_min, t_max);
	float t_near = max(max(t_minDist.x, t_minDist.y), t_minDist.z);
	float t_far = min(min(t_maxDist.x, t_maxDist.y), t_maxDist.z);

	Collision c;
	c.t = t_near;
	c.inside = false;

	// If the ray is entering the box, t_near contains the farthest boundary of entry
	// If the ray is leaving the box, t_far contains the closest boundary of exit
	// The ray intersects the box if and only if t_near < t_far, and if t_far > 0.0
	
	// If the ray didn't intersect the box, return a negative t value
	if(t_near >= t_far || t_far <= 0.0)
	{	c.t = -1.0;
		return c;
	}

	float intersect_distance = t_near;
	vec3 plane_intersect_distances = t_minDist;

	// if t_near < 0, then the ray started inside the box and left the box
	if( t_near < 0.0)
	{	c.t = t_far;
		intersect_distance = t_far;
		plane_intersect_distances = t_maxDist;
		c.inside = true;
	}

	// Checking which boundary the intersection lies on
	int face_index = 0;

	if(intersect_distance == plane_intersect_distances.y) face_index = 1;
	else if(intersect_distance == plane_intersect_distances.z) face_index = 2;

	// Creating the collision normal
	c.n = vec3(0.0);
	c.n[face_index] = 1.0;

	// If we hit the box from the negative axis, invert the normal
	if(r.dir[face_index] > 0.0) c.n *= -1.0;

	// Calculate the world-position of the intersection:
	c.p = r.start + c.t * r.dir;

	return c;
}

//------------------------------------------------------------------------------
// Checks if Ray r intersects the Sphere defined by Object o.sphere
// This implementation is based on the following algorithm:
// http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/basic_algo.pdf
/*
����������Ľ����㷨��
�������ߵ�ԭ��Rs�ͷ���Rd���Լ����ĵ�λ��Sp������뾶Sr�����ߺ����彻��λ��P
��ά�ռ�Բ�ϵĵ�ķ��̣� ||P -Sp||^2 = Sr^2 �����λ�õ�����λ�õ��ڰ뾶��
���߷��̣�Rs + t.Rd = P ������λ�� = �ӹ������λ�� + ��һ���Ĺ��߷��� * ���룩
�ϲ����̣� Rs^2 + 2*t*Rd*Rs - 2*Rs*Sp + t^2*Rd^2 - 2*t*Rd*Sp + Sp^2 - Sr^2 = 0 
��һ���򻯣�  (Rd*Rd)t^2 + (2Rd*(Rs-Sp))t+(Rs-Sp)^2-Sr^2=0 (��һ������t��һԪ���η���)
���������б�ʽ�� delt = (2Rd*(Rs - Sp))^2 - 4|Rd|^2((Rs-Sp)^2 - Sr^2)
���룺 t = ((-2Rd*(Rs-Sp))+-sqrt(delt)) /2(Rd*Rd)^2
deltС����ʱ����ʵ����delt������ʱ����������ȸ���delt������ʱ������������ȵĸ�
delt����0ʱ��t������ֵ�н�С�ͽϴ�ķֱ��ʾΪTnear��Tfar����ʱ��
��Tnear��Tfar��Ϊ��ʱ���������嶼�ڹ��ߵı��棬û�н���
��TnearΪ����TfarΪ��ʱ�����ߴ������ڲ���ʼ����һ����������Tfar
�����߶�Ϊ��ʱ����һ����������Tnear
*/
//------------------------------------------------------------------------------
Collision intersect_sphere_object(Ray r)
{	float qa = dot(r.dir, r.dir);//�������ܿ��Ƕ�ͬһ�������������ĵ���������������ȵ�ƽ�� dot(a, b) = |a|*|b|*cos(ceta)
	float qb = dot(2*r.dir, r.start-sphere_position);
	float qc = dot(r.start-sphere_position, r.start-sphere_position) - sphere_radius*sphere_radius;

	// Solving for qa * t^2 + qb * t + qc = 0
	float qd = qb * qb - 4 * qa * qc;

	Collision c;
	c.inside = false;

	if(qd < 0.0)	// no solution in this case
	{	c.t = -1.0;
		return c;
	}

	float t1 = (-qb + sqrt(qd)) / (2.0 * qa);
	float t2 = (-qb - sqrt(qd)) / (2.0 * qa);

	float t_near = min(t1, t2);
	float t_far = max(t1, t2);

	c.t = t_near;

	if(t_far < 0.0)		// sphere is behind the ray, no intersection
	{	c.t = -1.0;
		return c;
	}

	if(t_near < 0.0)	// the ray started inside the sphere
	{	c.t = t_far;
		c.inside = true;
	}

	c.p = r.start + c.t * r.dir;	// world position of the collision
	c.n = normalize(c.p - sphere_position);	// use the world position to compute the surface normal

	if(c.inside)	// if collision is leaving the sphere, flip the normal
	{	c.n *= -1.0;
	}
	return c;
}

//------------------------------------------------------------------------------
// Returns the closest collision of a ray
// object_index == -1 if no collision
// object_index == 1 if collision with sphere
// object_index == 2 if collision with box
//------------------------------------------------------------------------------
Collision get_closest_collision(Ray r)
{	Collision closest_collision, cSph, cBox;
	closest_collision.object_index = -1;

	cSph = intersect_sphere_object(r);
	cBox = intersect_box_object(r);
	
	if ((cSph.t > 0) && ((cSph.t < cBox.t) || (cBox.t < 0)))
	{	closest_collision = cSph;
		closest_collision.object_index = 1;
	}
	if ((cBox.t > 0) && ((cBox.t < cSph.t) || (cSph.t < 0)))
	{	closest_collision = cBox;
		closest_collision.object_index = 2;
	}
	return closest_collision;
}

//------------------------------------------------------------------------------
// Computes the Ambient Diffuse Specular (ADS) Phong lighting for an
// incident Ray r at the surface of the object.  Returns the color.
//------------------------------------------------------------------------------
//*
vec3 ads_phong_lighting(Ray r, Collision c)
{	// add the contribution from the ambient and positional lights
	vec4 ambient = worldAmb_ambient + pointLight_ambient * objMat_ambient;
	
	// initialize diffuse and specular contributions��ʼ��������;��淴�����
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);

	// Check to see if any object is casting a shadow on this surface����Ƿ����κ��������������Ͷ����Ӱ
	Ray light_ray;
	light_ray.start = c.p + c.n * 0.01;
	light_ray.dir = normalize(pointLight_position - c.p);
	bool in_shadow = false;

	// Cast the ray against the scene����Ӱ��֪��Ͷ�������
	Collision c_shadow = get_closest_collision(light_ray);

	// If the ray hit an object and if the hit occurred between the surface and the light
	//�����Ӱ��֪�����������壬������ײλ���ڹ�Դ�����֮��
	if(c_shadow.object_index != -1 && c_shadow.t < length(pointLight_position - c.p))
	{	in_shadow = true;
	}

	// If this surface is in shadow, don't add diffuse and specular components
	if(in_shadow == false)
	{	// Computing the light's reflection on the surface
		vec3 light_dir = normalize(pointLight_position - c.p);
		vec3 light_ref = normalize( reflect(-light_dir, c.n));//reflect�������ڼ���һ������������һ�������ķ���
		float cos_theta = dot(light_dir, c.n);
		float cos_phi = dot( normalize(-r.dir), light_ref);

		diffuse = pointLight_diffuse * objMat_diffuse * max(cos_theta, 0.0);
		specular = pointLight_specular * objMat_specular * pow( max( cos_phi, 0.0), objMat_shininess);
	}
	vec4 phong_color = ambient + diffuse + specular;
	return phong_color.rgb;
}
//*/

//�������Ϊ����Ӱ����
/*
vec3 ads_phong_lighting(Ray r, Collision c)
{	// add the contribution from the ambient and positional lights
	vec4 ambient = worldAmb_ambient + pointLight_ambient * objMat_ambient;

	// Computing the light's reflection on the surface
	vec3 light_dir = normalize(pointLight_position - c.p);
	vec3 light_ref = normalize( reflect(-light_dir, c.n));//reflect�������ڼ���һ������������һ�������ķ���
	float cos_theta = dot(light_dir, c.n);
	float cos_phi = dot( normalize(-r.dir), light_ref);

	vec4 diffuse = pointLight_diffuse * objMat_diffuse * max(cos_theta, 0.0);
	vec4 specular = pointLight_specular * objMat_specular * pow( max( cos_phi, 0.0), objMat_shininess);
	
	vec4 phong_color = ambient + diffuse + specular;
	return phong_color.rgb;
}
*/

//------------------------------------------------------------------------------
// This function casts a ray into the scene and returns the final color for a pixel
//------------------------------------------------------------------------------
vec3 raytrace(Ray r)
{	Collision c = get_closest_collision(r);
	if (c.object_index == -1) return vec3(0.0);	// no collision
	if (c.object_index == 1) return ads_phong_lighting(r,c) * sphere_color;
	if (c.object_index == 2) return ads_phong_lighting(r,c) * box_color;
}

void main()
{	int width = int(gl_NumWorkGroups.x);
	int height = int(gl_NumWorkGroups.y);
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

	// convert this pixels screen space location to world space �����ش���Ļ�ռ�ת����ȫ�ֿռ�
	float x_pixel = 2.0 * pixel.x/width - 1.0;
	float y_pixel = 2.0 * pixel.y/height - 1.0;
	
	// Get this pixels world-space ray��ȡ���ص�ȫ�ֹ���
	Ray world_ray;
	world_ray.start = vec3(0.0, 0.0, camera_pos);
	vec4 world_ray_end = vec4(x_pixel, y_pixel, camera_pos-1.0, 1.0);
	world_ray.dir = normalize(world_ray_end.xyz - world_ray.start);

	// Cast the ray out into the world and intersect the ray with objects Ͷ����߲�ʹ���������ཻ
	vec3 color = raytrace(world_ray);
	imageStore(output_texture, pixel, vec4(color,1.0));
}