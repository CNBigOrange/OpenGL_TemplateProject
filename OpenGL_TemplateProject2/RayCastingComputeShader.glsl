#version 430

layout (local_size_x=1) in;//工作组大小为1
layout (binding=0, rgba8) uniform image2D output_texture;
float camera_pos = 5.0;

struct Ray//定义光线
{	vec3 start;	// origin of the ray 光线原点
	vec3 dir;	// normalized direction of the ray 归一化后的光线方向
};

//定义球体
float sphere_radius = 2.5;
vec3 sphere_position = vec3(1.0, 0.0, -3.0);
vec3 sphere_color = vec3(1.0, 0.0, 0.0); // red

//定义立方体
vec3 box_mins = vec3(-2.0, -2.0, 0.0);
vec3 box_maxs = vec3(-0.5, 1.0, 2.0);
vec3 box_color = vec3(0.0, 1.0, 0.0); // green

struct Collision
{	float t;	// value at which this collision occurs for a ray 在光线上距碰撞位置的距离
	vec3 p;		// The world position of the collision 碰撞点的全局坐标
	vec3 n;		// the normal of the collision 碰撞点的表面法向量
	bool inside;	// Whether the ray started inside the object and collided while exiting 碰撞点是否是在穿过物体内表面向外离开物体时发生的
	int object_index;	// The index of the object this collision hit 碰撞到的物体的索引
};

//------------------------------------------------------------------------------
// Checks if Ray r intersects the Box defined by Object o.box
// This implementation is based on the following algorithm:
// http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/basic_algo.pdf
/*
光线与立方体的交点算法： Slabs算法，由2D推到至3D
给定射线的原点Rs和方向Rd，以及球心的位置Sp和球面半径Sr，射线和球体交点位置P
光线方程：Rs + t.Rd = P （交点位置 = 从光线起点位置 + 归一化的光线方向 * 距离）
使用立方体对角线上相对的两个顶点来定义立方体：（ Xmin，Ymin，Zmin）和（Xmax，Ymax，Zmax）
我们可以使用者两个点来确定立方体的留个面，例如采用两个x值来确定长方体的两个平行于yz平面的面
射线与立方体最大点和最小点的交点：BOXmin = Rs + Tmins*Rd 变换方程：Tmins = （BOXmins - Rs）/Rd
BOXmax = Rs + Tmax*Rd 变换方程：Tmax = （BOXmax - Rs）/Rd
BOXmins的x，y，z和BOXmax的x，y，z分别对应近的三个平面和远的三个平面

求光线与立方体所处的三组 平行面 的远近交点的关系，判断光线是否与立方体相交：
当t_near>t_far或t_far<=0时，光线根本不与立方体相交；
当t_near<0且t_far>0时，说明t_near在光线的反方向，t_far在光线的正方向，光线是从立方体内部射出并离开立方体的，有一个碰撞点
否则，有两个碰撞点发生在距起点t_near和t_far处
*/
//------------------------------------------------------------------------------
Collision intersect_box_object(Ray r)
{	// Calculate the box's world mins and maxs:
	vec3 t_min = (box_mins - r.start) / r.dir;
	vec3 t_max = (box_maxs - r.start) / r.dir;
	//GLSL 中对两个vec3类型值应用min（）操作会返回一个vec3类型值，其中包含每队x，y，z元素中较小的一个。max（）操作同理
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
光线与球面的交点算法：
给定射线的原点Rs和方向Rd，以及球心的位置Sp和球面半径Sr，射线和球体交点位置P
三维空间圆上的点的方程： ||P -Sp||^2 = Sr^2 （点的位置到球心位置等于半径）
光线方程：Rs + t.Rd = P （交点位置 = 从光线起点位置 + 归一化的光线方向 * 距离）
合并方程： Rs^2 + 2*t*Rd*Rs - 2*Rs*Sp + t^2*Rd^2 - 2*t*Rd*Sp + Sp^2 - Sr^2 = 0 
进一步简化：  (Rd*Rd)t^2 + (2Rd*(Rs-Sp))t+(Rs-Sp)^2-Sr^2=0 (是一个关于t的一元二次方程)
计算它的判别式： delt = (2Rd*(Rs - Sp))^2 - 4|Rd|^2((Rs-Sp)^2 - Sr^2)
距离： t = ((-2Rd*(Rs-Sp))+-sqrt(delt)) /2(Rd*Rd)^2
delt小于零时，无实根，delt等于零时，有两个相等根，delt大于零时，有两个不相等的根
delt大于0时，t的两个值中较小和较大的分别表示为Tnear和Tfar，此时：
当Tnear和Tfar都为负时，整个球体都在光线的背面，没有交点
当Tnear为负且Tfar为正时，光线从球体内部开始，第一个交点距起点Tfar
当两者都为正时，第一个交点距起点Tnear
*/
//------------------------------------------------------------------------------
Collision intersect_sphere_object(Ray r)
{	float qa = dot(r.dir, r.dir);//出于性能考虑对同一个向量计算他的点乘来计算向量长度的平方 dot(a, b) = |a|*|b|*cos(ceta)
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
// This function casts a ray into the scene and returns the final color for a pixel
//------------------------------------------------------------------------------
vec3 raytrace(Ray r)
{	Collision c = get_closest_collision(r);
	if (c.object_index == -1) return vec3(0.0);	// no collision
	if (c.object_index == 1) return sphere_color;
	if (c.object_index == 2) return box_color;
}

void main()
{	int width = int(gl_NumWorkGroups.x);
	int height = int(gl_NumWorkGroups.y);
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

	// convert this pixels screen space location to world space 将像素从屏幕空间转换到全局空间
	float x_pixel = 2.0 * pixel.x/width - 1.0;
	float y_pixel = 2.0 * pixel.y/height - 1.0;
	
	// Get this pixels world-space ray获取像素的全局光线
	Ray world_ray;
	world_ray.start = vec3(0.0, 0.0, camera_pos);
	vec4 world_ray_end = vec4(x_pixel, y_pixel, camera_pos-1.0, 1.0);
	world_ray.dir = normalize(world_ray_end.xyz - world_ray.start);

	// Cast the ray out into the world and intersect the ray with objects 投射光线并使其与物体相交
	vec3 color = raytrace(world_ray);
	imageStore(output_texture, pixel, vec4(color,1.0));
}