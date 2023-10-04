#version 430

layout (local_size_x=1) in;//工作组大小为1
layout (binding=0, rgba8) uniform image2D output_texture;
layout (binding=1) uniform sampler2D sampMarble;

float camera_pos = 5.0;

uniform float time;

struct Ray//定义光线
{	vec3 start;	// origin of the ray 光线原点
	vec3 dir;	// normalized direction of the ray 归一化后的光线方向
};

//定义球体
float sphere_radius = 1.2;
vec3 sphere_position = vec3(0.7 + sin(time), 0.2, 0.2);
vec3 sphere_color = vec3(1.0, 0.0, 0.0); // red

vec3 box_mins = vec3(-0.25, -0.8, -0.25);
vec3 box_maxs = vec3( 0.25,  0.8,  0.25);
vec3 box_color = vec3(0.0, 1.0, 0.0); // green

vec3 sbox_mins = vec3(-20, -20, -20);
vec3 sbox_maxs = vec3( 20, 20,  20);
vec3 rbox_color = vec3(0.25, 1.0, 1.0); // light aqua

const float PI = 3.14159265358;
const float DEG_TO_RAD = PI / 180.0;

vec3 box_pos = vec3(-0.75, -0.2, 3.4);
float box_xrot = 0.0;
float box_yrot = 70.0;
float box_zrot = 0.0;

//平面的位置
vec3 plane_pos = vec3(0, -1.0, -2.0);
float plane_width = 12.0;
float plane_depth = 16.0;
float plane_xrot = DEG_TO_RAD * 0.0;
float plane_yrot = DEG_TO_RAD * 0.0;
float plane_zrot = DEG_TO_RAD * 0.0;

//ADS光照
vec4 worldAmb_ambient = vec4(0.3, 0.3, 0.3, 1.0);

vec4 objMat_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 objMat_diffuse = vec4(0.9, 0.9, 0.9, 1.0);
vec4 objMat_specular = vec4(1.0, 1.0, 1.0, 1.0);
float objMat_shininess = 50.0;

vec3 pointLight_position = vec3(-3.0+ 5 * sin(time), 4.0 , 3.5);
vec4 pointLight_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 pointLight_diffuse = vec4(0.7, 0.7, 0.7, 1.0);
vec4 pointLight_specular = vec4(1.0, 1.0, 1.0, 1.0);

struct Collision
{	float t;	// value at which this collision occurs for a ray 在光线上距碰撞位置的距离
	vec3 p;		// The world position of the collision 碰撞点的全局坐标
	vec3 n;		// the normal of the collision 碰撞点的表面法向量
	bool inside;	// Whether the ray started inside the object and collided while exiting 碰撞点是否是在穿过物体内表面向外离开物体时发生的
	int object_index;	// The index of the object this collision hit 碰撞到的物体的索引
	vec2 tc;   //物体碰撞点的纹理坐标
	int face_index;   //碰撞到的面的索引（用于有纹理的天空盒）
};

// -------------- RECURSIVE SECTION 递归元素

struct Stack_Element
{	int type;		// The type of ray ( 1 = reflected, 2 = refracted )光线类型（1表示反射，2表示折射。阴影感知射线不会入栈
	int depth;		// The depth of the recursive raytrace光线追踪递归深度
	int phase;	// Keeps track of what phase each recursive call is at (each call is broken down into five phases)当前正在进行5个阶段的递归调用中的哪一个
	vec3 phong_color;		// Contains the Phong ADS model color储存计算好的ADS颜色信息
	vec3 reflected_color;	// Contains the reflected color储存计算好的反射颜色信息
	vec3 refracted_color;	// Contains the refracted color储存计算好的折射颜色信息
	vec3 final_color;		// Contains the final mixed output of the recursive raytrace call本次调用的最终颜色混合结果
	Ray ray;				// The ray for this raytrace invocation本次调用的光线
	Collision collision;	// The collision for this raytrace invocation. Contains null_collision until phase 1本次调用的碰撞，开始时值为null_collision
};

const int RAY_TYPE_REFLECTION = 1;
const int RAY_TYPE_REFRACTION = 2;
//定义各结构的null值以便确定哪个值还未进行赋值（初始化）
Ray null_ray = {vec3(0.0), vec3(0.0)};
Collision null_collision = { -1.0, vec3(0.0), vec3(0.0), false, -1, vec2(0.0, 0.0), -1 };
Stack_Element null_stack_element = {0,-1,-1,vec3(0),vec3(0),vec3(0),vec3(0),null_ray,null_collision};

const int stack_size = 100;//栈的大小
Stack_Element stack[stack_size];//储存raytrace()函数调用的“递归”栈
const int max_depth = 3;//最多光线序列数，即最大递归深度

int stack_pointer = -1;			// Points to the top of the stack (-1 if empty)指向栈顶（-1表示空栈）
Stack_Element popped_stack_element;	// Holds the last popped element from the stack存储从栈中最后出栈的元素

// --------------- END RECURSIVE SECTION

mat4 buildTranslate(float x, float y, float z)
{	return mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, x, y, z, 1.0);
}
mat4 buildRotateX(float rad)
{	return mat4(1.0,0.0,0.0,0.0,0.0,cos(rad),sin(rad),0.0,0.0,-sin(rad),cos(rad),0.0,0.0,0.0,0.0,1.0);
}
mat4 buildRotateY(float rad)
{	return mat4(cos(rad),0.0,-sin(rad),0.0,0.0,1.0,0.0,0.0,sin(rad),0.0,cos(rad),0.0,0.0,0.0,0.0,1.0);
}
mat4 buildRotateZ(float rad)
{	return mat4(cos(rad),sin(rad),0.0,0.0,-sin(rad),cos(rad),0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0);
}

//------------------------------------------------------------------------------
// Checks if Ray r intersects the Plane
/*
光线与以原点为中心且法向量Np = (0,1,0)的水平面相交时：
平面上一个点到原点的向量与平面法向量点积为0：dot(P,Np) = 0,
光线上一点：P = Rs + t * Rd,合并两式：t = dot(-Rs,Np)/dot(Rd,Np)
*/
//------------------------------------------------------------------------------
Collision intersect_plane_object(Ray r)
{	// Compute the planes's local-space to world-space transform matrices, and their inverse
	//计算平面的局部空间到全局空间的变换矩阵和它们的逆矩阵
	mat4 local_to_worldT = buildTranslate(plane_pos.x, plane_pos.y, plane_pos.z);
	mat4 local_to_worldR = buildRotateY(plane_yrot) * buildRotateX(plane_xrot) * buildRotateZ(plane_zrot);
	mat4 local_to_worldTR = local_to_worldT * local_to_worldR;
	mat4 world_to_localTR = inverse(local_to_worldTR);
	mat4 world_to_localR = inverse(local_to_worldR);

	// Convert the world-space ray to the planes's local space:
	//将光线从全局空间转换到平面的局部空间(用以计算光线交点是否在平面范围内)
	vec3 ray_start = (world_to_localTR * vec4(r.start,1.0)).xyz;
	vec3 ray_dir = (world_to_localR * vec4(r.dir,1.0)).xyz;
	
	Collision c;
	c.inside = false;  // there is no "inside" of a plane
	
	// compute intersection point of ray with plane计算光线与平面的交点
	c.t = dot((vec3(0,0,0) - ray_start),vec3(0,1,0)) / dot(ray_dir, vec3(0,1,0));
	
	// Calculate the world-position of the intersection:计算交点的全局位置和平面局部位置
	c.p = r.start + c.t * r.dir;
	
	// Calculate the position of the intersection in plane space:
	vec3 intersectPoint = ray_start + c.t * ray_dir;
	
	// If the ray didn't intersect the plane object, return a negative t value如果光线没有和平面相交，返回负值
	if ((abs(intersectPoint.x) > (plane_width/2.0)) || (abs(intersectPoint.z) > (plane_depth/2.0)))
	{	c.t = -1.0;
		return c;
	}

	// Create the collision normal
	c.n = vec3(0.0, 1.0, 0.0);

	// If we hit the plane from the negative axis, invert the normal如果光线从平面下方碰撞，则对齐取反
	if(ray_dir.y > 0.0) c.n *= -1.0;
	
	// now convert the normal back into world space
	c.n = transpose(inverse(mat3(local_to_worldR))) * c.n;

	// Compute texture coordinates计算纹理坐标
	float maxDimension = max(plane_width, plane_depth);
	(c.tc).x = (intersectPoint.x + plane_width/2.0)/maxDimension;
	(c.tc).y = (intersectPoint.z + plane_depth/2.0)/maxDimension;
	return c;
}

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
{	
	// Compute the box's local-space to world-space transform matrices, and their inverse
	mat4 local_to_worldT = buildTranslate(box_pos.x, box_pos.y, box_pos.z);
	mat4 local_to_worldR = buildRotateY(box_yrot) * buildRotateX(box_xrot) * buildRotateZ(box_zrot);
	mat4 local_to_worldTR = local_to_worldT * local_to_worldR;
	mat4 world_to_localTR = inverse(local_to_worldTR);
	mat4 world_to_localR = inverse(local_to_worldR);

	// Convert the world-space ray to the box's local space:
	vec3 ray_start = (world_to_localTR * vec4(r.start,1.0)).xyz;
	vec3 ray_dir = (world_to_localR * vec4(r.dir,1.0)).xyz;

	// Calculate the box's world mins and maxs:
	vec3 t_min = (box_mins - ray_start) / ray_dir;
	vec3 t_max = (box_maxs - ray_start) / ray_dir;
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
	if(ray_dir[face_index] > 0.0) c.n *= -1.0;

	// now convert the normal back into world space
	c.n = transpose(inverse(mat3(local_to_worldR))) * c.n;

	// Calculate the world-position of the intersection:
	c.p = r.start + c.t * r.dir;

	// Compute texture coordinates 计算纹理坐标
	// start by computing position in box space that ray collides 从计算立方体局部空间下的光线碰撞点坐标开始
	vec3 cp = (world_to_localTR * vec4(c.p,1.0)).xyz;
	// now compute largest box dimension 接下来计算立方体最大边长
	float totalWidth = box_maxs.x - box_mins.x;
	float totalHeight = box_maxs.y - box_mins.y;
	float totalDepth = box_maxs.z - box_mins.z;
	float maxDimension = max(totalWidth, max(totalHeight, totalDepth));
	// finally, select tex coordinates depending on box face 将x，y，z坐标转换至[0,1]区间，并除以立方体最大边长
	float rayStrikeX = (cp.x + totalWidth/2.0)/maxDimension;
	float rayStrikeY = (cp.y + totalHeight/2.0)/maxDimension;
	float rayStrikeZ = (cp.z + totalDepth/2.0)/maxDimension;
	//最后基于立方体表面索引，选取（x，y）、（x，z）或（y，z）作为纹理坐标
	if (face_index == 0)
		c.tc = vec2(rayStrikeZ, rayStrikeY);
	else if (face_index == 1)
		c.tc = vec2(rayStrikeZ, rayStrikeX);
	else
		c.tc = vec2(rayStrikeY, rayStrikeX);


	return c;
}

//------------------------------------------------------------------------------
// Checks if Ray r intersects a SkyBox defined by Object o.box
// Essentially the same as box_object computation, but with different texture coordinates.
// Add lighting if using as a roombox.
//------------------------------------------------------------------------------
Collision intersect_sky_box_object(Ray r)
{	// Calculate the box's world mins and maxs:
	vec3 t_min = (sbox_mins - r.start) / r.dir;
	vec3 t_max = (sbox_maxs - r.start) / r.dir;
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

	float intersection = t_near;
	vec3 boundary = t_minDist;

	// if t_near < 0, then the ray started inside the box and left the box
	if( t_near < 0.0)
	{	c.t = t_far;
		intersection = t_far;
		boundary = t_maxDist;
		c.inside = true;
	}

	// Checking which boundary the intersection lies on
	int face_index = 0;
	if(intersection == boundary.y) face_index = 1;
	else if(intersection == boundary.z) face_index = 2;
	
	// Creating the collision normal
	c.n = vec3(0.0);
	c.n[face_index] = 1.0;

	// If we hit the box from the negative axis, invert the normal
	if(r.dir[face_index] > 0.0) c.n *= -1.0;
	
	// Calculate the world-position of the intersection:
	c.p = r.start + c.t * r.dir;
	
	// Calculate face index for collision object
	if (c.n == vec3(1,0,0)) c.face_index = 0;
	else if (c.n == vec3(-1,0,0)) c.face_index = 1;
	else if (c.n == vec3(0,1,0)) c.face_index = 2;
	else if (c.n == vec3(0,-1,0)) c.face_index = 3;
	else if (c.n == vec3(0,0,1)) c.face_index = 4;
	else if (c.n == vec3(0,0,-1)) c.face_index = 5;
	
	// Compute texture coordinates
	// compute largest box dimension
	float totalWidth = sbox_maxs.x - sbox_mins.x;
	float totalHeight = sbox_maxs.y - sbox_mins.y;
	float totalDepth = sbox_maxs.z - sbox_mins.z;
	float maxDimension = max(totalWidth, max(totalHeight, totalDepth));
	
	// select tex coordinates depending on box face
	float rayStrikeX = ((c.p).x + totalWidth/2.0)/maxDimension;
	float rayStrikeY = ((c.p).y + totalHeight/2.0)/maxDimension;
	float rayStrikeZ = ((c.p).z + totalDepth/2.0)/maxDimension;
	
	if (c.face_index == 0)
		c.tc = vec2(rayStrikeZ, rayStrikeY);
	else if (c.face_index == 1)
		c.tc = vec2(1.0-rayStrikeZ, rayStrikeY);
	else if (c.face_index == 2)
		c.tc = vec2(rayStrikeX, rayStrikeZ);
	else if (c.face_index == 3)
		c.tc = vec2(rayStrikeX, 1.0-rayStrikeZ);
	else if (c.face_index == 4)
		c.tc = vec2(1.0-rayStrikeX, rayStrikeY);
	else if (c.face_index == 5)
		c.tc = vec2(rayStrikeX, rayStrikeY);
		
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

	// compute texture coordinates based on normal用球体法向量来求纹理坐标
	c.tc.x = 0.5 + atan(-c.n.z, c.n.x)/(2.0*PI);
	c.tc.y = 0.5 - asin(-c.n.y)/PI;

	return c;
}

//------------------------------------------------------------------------------
// Returns the closest collision of a ray
// object_index == -1 if no collision
// object_index == 1 if collision with sphere
// object_index == 2 if collision with box
//------------------------------------------------------------------------------
Collision get_closest_collision(Ray r)
{	Collision closest_collision, cSph, cBox, cRBox, cPlane;
	closest_collision.object_index = -1;

	cSph = intersect_sphere_object(r);
	cBox = intersect_box_object(r);
	cRBox = intersect_sky_box_object(r);
	cPlane = intersect_plane_object(r);
	
	if ((cSph.t > 0) &&
		((cSph.t < cBox.t) || (cBox.t < 0))
		&& ((cSph.t < cRBox.t) || (cRBox.t < 0))
		&& ((cSph.t < cPlane.t) || (cPlane.t < 0)))
	{	closest_collision = cSph;
		closest_collision.object_index = 1;
	}
	if ((cBox.t > 0) &&
		((cBox.t < cSph.t) || (cSph.t < 0))
		&& ((cBox.t < cRBox.t) || (cRBox.t < 0))
		&& ((cBox.t < cPlane.t) || (cPlane.t < 0)))
	{	closest_collision = cBox;
		closest_collision.object_index = 2;
	}
	if ((cRBox.t > 0) &&
		((cRBox.t < cSph.t) || (cSph.t < 0))
		&& ((cRBox.t < cBox.t) || (cBox.t < 0))
		&& ((cRBox.t < cPlane.t) || (cPlane.t < 0)))
	{	closest_collision = cRBox;
		closest_collision.object_index = 3;
	}
	if ((cPlane.t > 0) &&
		((cPlane.t < cSph.t) || (cSph.t < 0))
		&& ((cPlane.t < cBox.t) || (cBox.t < 0))
		&& ((cPlane.t < cRBox.t) || (cRBox.t < 0)))
	{	closest_collision = cPlane;
		closest_collision.object_index = 4;
	}
	return closest_collision;
}

//------------------------------------------------------------------------------
// Computes the Ambient Diffuse Specular (ADS) Phong lighting for an
// incident Ray r at the surface of the object.  Returns the color.
//------------------------------------------------------------------------------
//*
vec3 ads_phong_lighting(Ray r, Collision c)
{	
	// add the contribution from the ambient and positional lights
	vec4 ambient = worldAmb_ambient + pointLight_ambient * objMat_ambient;
	
	// initialize diffuse and specular contributions初始化漫反射和镜面反射分量
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);

	// Check to see if any object is casting a shadow on this surface检查是否有任何物体在这个表面投射阴影
	Ray light_ray;
	light_ray.start = c.p + c.n * 0.01;
	light_ray.dir = normalize(pointLight_position - c.p);
	bool in_shadow = false;

	// Cast the ray against the scene将阴影感知线投射进场景
	Collision c_shadow = get_closest_collision(light_ray);

	// If the ray hit an object and if the hit occurred between the surface and the light
	//如果阴影感知线碰到了物体，并且碰撞位置在光源与表面之间
	if(c_shadow.object_index != -1 && c_shadow.t < length(pointLight_position - c.p))
	{	in_shadow = true;
	}

	// If this surface is in shadow, don't add diffuse and specular components
	if(in_shadow == false)
	{	// Computing the light's reflection on the surface
		vec3 light_dir = normalize(pointLight_position - c.p);
		vec3 light_ref = normalize( reflect(-light_dir, c.n));//reflect函数用于计算一个向量基于另一个向量的反射
		float cos_theta = dot(light_dir, c.n);
		float cos_phi = dot( normalize(-r.dir), light_ref);

		diffuse = pointLight_diffuse * objMat_diffuse * max(cos_theta, 0.0);
		specular = pointLight_specular * objMat_specular * pow( max( cos_phi, 0.0), objMat_shininess);
	}
	vec4 phong_color = ambient + diffuse + specular;
	return phong_color.rgb;
}
//*/

//下面代码为无阴影代码
/*
vec3 ads_phong_lighting(Ray r, Collision c)
{	// add the contribution from the ambient and positional lights
	vec4 ambient = worldAmb_ambient + pointLight_ambient * objMat_ambient;

	// Computing the light's reflection on the surface
	vec3 light_dir = normalize(pointLight_position - c.p);
	vec3 light_ref = normalize( reflect(-light_dir, c.n));//reflect函数用于计算一个向量基于另一个向量的反射
	float cos_theta = dot(light_dir, c.n);
	float cos_phi = dot( normalize(-r.dir), light_ref);

	vec4 diffuse = pointLight_diffuse * objMat_diffuse * max(cos_theta, 0.0);
	vec4 specular = pointLight_specular * objMat_specular * pow( max( cos_phi, 0.0), objMat_shininess);
	
	vec4 phong_color = ambient + diffuse + specular;
	return phong_color.rgb;
}
*/

// -------------------------------------------------------------
// This function generates a procedural checkerboard texture
// tileScale specifies the number of squares along the longest axis
// -------------------------------------------------------------
vec3 checkerboard(vec2 tc)
{	float tileScale = 24.0;
	float tile = mod(floor(tc.x * tileScale) + floor(tc.y * tileScale), 2.0);
	return tile * vec3(1,1,1);
}

// ==================== RECURSIVE SECTION =============================

//------------------------------------------------------------------------------
// Schedules a new raytrace by adding it to the top of the stack
//“入栈”函数，通过将raytrace（）调用所需信息入栈，将一次新的raytrace（）调用加入调度
//------------------------------------------------------------------------------
void push(Ray r, int depth, int type)
{	if (stack_pointer >= stack_size-1)  return;//如果栈已满，直接返回

	Stack_Element element;//将已知元素初始化，其余元素赋值为0或null值
	element = null_stack_element;
	element.type = type;
	element.depth = depth;
	element.phase = 1;
	element.ray = r;

	stack_pointer++;//指向栈顶
	stack[stack_pointer] = element;
}

//------------------------------------------------------------------------------
// Removes the topmost stack element
//“出栈”函数，移除已经完成的raytrace（）函数操作
//------------------------------------------------------------------------------
Stack_Element pop()
{	// Store the element we're removing in top_stack_element 储存我们将要移除的栈顶元素 
	Stack_Element top_stack_element = stack[stack_pointer];
	
	// Erase the element from the stack 抹去元素
	stack[stack_pointer] = null_stack_element;
	stack_pointer--;
	return top_stack_element;
}

//------------------------------------------------------------------------------
// This function processes the stack element at a given index
// This function is guaranteed to be ran on the topmost stack element
//光线处理的5个阶段：1、碰撞；2、光照；3、反射；4、折射；5、混色
//------------------------------------------------------------------------------
void process_stack_element(int index)
{
	// If there is a popped_stack_element that just ran, it holds one of our values
	//如果存在之前处理过的popped_stack_element，那么它有当前栈元素的反射/折射信息。存储该信息并清除popped_stack_element
	// Store it and delete it
	if (popped_stack_element != null_stack_element)//GLSL元素的结构体比较
	{	if (popped_stack_element.type == RAY_TYPE_REFLECTION)//反射
			stack[index].reflected_color = popped_stack_element.final_color;
		else if (popped_stack_element.type == RAY_TYPE_REFRACTION)//折射
			stack[index].refracted_color = popped_stack_element.final_color;
		popped_stack_element = null_stack_element;
	}

	Ray r = stack[index].ray;//初始化时使用最初的光线
	Collision c = stack[index].collision;//从null开始，在阶段1中赋值

	// Iterate through the raytrace phases (explained below)
	switch (stack[index].phase)
	{	//=================================================
		// PHASE 1 - Raytrace Collision Detection
		//第一阶段——光线追踪碰撞检测
		//=================================================
		case 1:
			c = get_closest_collision(r);	// Cast ray against the scene, store the collision result将光线投射进场景，存储碰撞结果
			if (c.object_index != -1)		// If the ray didn't hit anything, stop.如果光线没有碰撞到任何物体，则停止
				stack[index].collision = c;	// otherwise, store the collision result否则，存入碰撞结果
			break;
		//=================================================
		// PHASE 2 - Phong ADS Lighting Computation
		//第二阶段——Phong ADS光照计算
		//=================================================
		case 2:
			stack[index].phong_color = ads_phong_lighting(r, c);
			break;
		//=================================================
		// PHASE 3 - Reflection Bounce Pass Computation
		//第三阶段——生成反射光线
		//=================================================
		case 3:
			// Only make recursive raytrace passes if we're not at max depth如果达到递归深度上限，则停止
			if (stack[index].depth < max_depth)
			{	// only the sphere and box are reflective
				if ((c.object_index == 1) || (c.object_index == 2))//只有球体和立方体有反射
				{	Ray reflected_ray;
					reflected_ray.start = c.p + c.n * 0.001;
					reflected_ray.dir = reflect(r.dir, c.n);
				
					// Add a raytrace for that ray to the stack将一组raytrace（）调用参数入栈，并设置其类型为反射
					push(reflected_ray, stack[index].depth+1, RAY_TYPE_REFLECTION);
			}	}
			break;
		//=================================================
		// PHASE 4 - Refraction Transparency Pass Computation
		//第四阶段——生成折射光线
		//=================================================
		case 4:
			// Only make recursive raytrace passes if we're not at max depth如果达到递归深度上限，则停止
			if (stack[index].depth < max_depth)
			{	// only the sphere is transparent
				if (c.object_index == 1)
				{	Ray refracted_ray;
					refracted_ray.start = c.p - c.n * 0.001;
					float refraction_ratio = 0.66667;
					if (c.inside) refraction_ratio = 1.0 / refraction_ratio;//当光线离开球体时，为1.0 / refraction_ratio
					refracted_ray.dir = refract(r.dir, c.n, refraction_ratio);
			
					// Add a raytrace for that ray to the stack将一组raytrace（）调用参数入栈，并设为折射
					push(refracted_ray, stack[index].depth+1, RAY_TYPE_REFRACTION);
			}	}
			break;
		//=================================================
		// PHASE 5 - Mixing to produce the final color
		//第五阶段——颜色混合
		//=================================================
		case 5:
			if (c.object_index == 1)//对于球体，将折射、反射和光照的颜色进行混合
			{	stack[index].final_color = stack[index].phong_color *
					((0.3 * stack[index].reflected_color) + (2.0 * stack[index].refracted_color));
			}
			if (c.object_index == 2)//对于立方体，将反射、光照和纹理的颜色进行混合
			{	stack[index].final_color = stack[index].phong_color *
					((0.5 * stack[index].reflected_color) + (1.0 * (texture(sampMarble, c.tc)).xyz));
			}
			if (c.object_index == 3) stack[index].final_color = stack[index].phong_color * rbox_color;
			if (c.object_index == 4) stack[index].final_color = stack[index].phong_color * (checkerboard(c.tc)).xyz;
			break;
		//=================================================
		// when all five phases are complete, end the recursion
		//当五个阶段都结束时，结束递归
		//=================================================
		case 6: { popped_stack_element = pop(); return; }
	}
	stack[index].phase++;
	return;	// Only process one phase per process_stack_element() invocation
	//每次调用process_stack_element()时只处理一个阶段
}

//------------------------------------------------------------------------------
// This function emulates recursive calls to raytrace for any desired depth
//这是驱动函数，它发出第一束光，并处理次级光线，直到结束
//------------------------------------------------------------------------------
vec3 raytrace(Ray r)
{	// Add a raytrace to the stack
	push(r, 0, RAY_TYPE_REFLECTION);

	// Process the stack until it's empty处理栈，直到他为空
	while (stack_pointer >= 0)
	{	int element_index = stack_pointer;		// Peek at the topmost stack element指向栈顶元素
		process_stack_element(element_index);	// Process this stack element处理当前元素的下一阶段
	}

	// Return the final_color value of the last-popped stack element返回最后出栈元素的颜色
	return popped_stack_element.final_color;
}
// ==========  END RECURSIVE SECTION ==================


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