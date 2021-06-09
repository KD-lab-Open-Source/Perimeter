#include "StdAfxRD.h"
#include "Silicon.h"
#include <xutil.h>

//#include "PrmEdit.h"
#include "Scripts/Silicon.hi"
#include "Scripts/Silicon.cppi"


Vect3f ElasticSphere::unit_sphere[psi_size][theta_size];
Vect3f ElasticSphere::points_map[psi_size][theta_size];
Vect3f ElasticSphere::normals_map[psi_size][theta_size];

////////////////////////////////////////////////////////////////
//		ElasticSphere
////////////////////////////////////////////////////////////////
ElasticSphere::ElasticSphere()
: cIUnkObj(KIND_ELASTICSPHERE)
{
	static int inited;
	if(!inited){
		inited = 1;
		for(int theta = 0; theta < theta_size; theta++)
			for(int psi = 0; psi < psi_size; psi++)
				unit_sphere[psi][theta].setSpherical(psi*M_PI*2/psi_size, theta*M_PI/(theta_size - 1), 1);
		}

	setRadius(100, 0.3f);
	Frame=new cFrame;
	for(int i=0;i<2;i++)
		Texture[i]=0;
	diffuse.set(1,1,1,1);
	blendMode = ALPHA_BLEND;
//	blendMode = ALPHA_ADDBLENDALPHA;
}
ElasticSphere::~ElasticSphere()
{
	for(int i=0;i<2;i++)
		if(Texture[i]) { Texture[i]->Release(); Texture[i]=0; }
	if(Frame) { delete Frame; Frame=0; }
}

void ElasticSphere::SetTexture(int n,cTexture *pTexture)
{
	if(Texture[n]) { Texture[n]->Release(); Texture[n]=0; } 
	Texture[n]=pTexture;
}
void ElasticSphere::SetPosition(const MatXf& Matrix)
{ 
	GlobalMatrix = LocalMatrix = Matrix;
	initial_position = Matrix.trans(); 

	Vect3f dir;
	dir.x = frnd(1);
	dir.y = frnd(1);
	dir.z = frnd(1);
	GlobalMatrix.rot().set(dir, frnd(M_PI));
}

void ElasticSphere::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)) return;
	if(!DrawNode->TestVisible(GetGlobalMatrix().trans(),radius))
		return;
	DrawNode->Attach(SCENENODE_OBJECTSORT,this);
}
void ElasticSphere::Draw(cCamera *DrawNode)
{
	prepare();
	DrawNode->GetRenderDevice()->Draw(this);
}
void ElasticSphere::Animate(float dt)
{
//	evolve();
	GetFrame()->AddPhase(dt);
}
void ElasticSphere::SetColor(const sColor4f *ambient,const sColor4f *diffuse_,const sColor4f *specular)
{
	diffuse=*diffuse_;
}
void ElasticSphere::GetColor(sColor4f *ambient,sColor4f *diffuse_,sColor4f *specular) const	
{ 
	if(ambient) ambient->set(0,0,0,1);
	if(diffuse_) *diffuse_=diffuse;
	if(specular) specular->set(0,0,0,1);
}

void ElasticSphere::setRadius(float radius_, float relative_random_displacement)
{
	radius = radius_;
	for(int psi = 0; psi < psi_size; psi++)
		for(int theta = 0; theta < theta_size; theta++){
			height_map[psi][theta] = radius*(1 + frnd(relative_random_displacement));
			velocity_map[psi][theta] = 0;
			}
	velocity.set(0, 0, 0);
}

static float zero_bound(float x){ return x > 0 ? x : 0; }

void ElasticSphere::evolve()
{
	float dz_factor = elastic_sphere_stiffness*silicon_time_step;
	float spawn_factor = elastic_sphere_spawn_factor*silicon_time_step;
	int psi;
	for(psi = 0; psi < psi_size; psi++)
		for(int theta = 0; theta < theta_size; theta++){
			float v = velocity_map[psi][theta]*spawn_factor;
			float dz = dz_factor*(height_map[psi][theta] - radius);
			velocity_map[psi][theta] -= dz + v + dot(unit_sphere[psi][theta], velocity)*elastic_sphere_velocity_factor;
			v *= 0.25f;
			v += dz*elastic_sphere_spawn_factor_dz;
			velocity_map[(psi - 1) & psi_mask][theta] += v;
			velocity_map[(psi + 1) & psi_mask][theta] += v;
			velocity_map[psi][(theta - 1) & theta_mask] += v;
			velocity_map[psi][(theta + 1) & theta_mask] += v;
			}

	for(psi = 0; psi < psi_size; psi++)
		for(int theta = 0; theta < theta_size; theta++){
			velocity_map[psi][theta] *= zero_bound(1.f - (elastic_sphere_damping + sqr(velocity_map[psi][theta])*elastic_sphere_damping2)*silicon_time_step);
			height_map[psi][theta] += velocity_map[psi][theta]*silicon_time_step;
			}

	float r0 = 0, v0 = 0;
	float r1 = 0, v1 = 0;
	for(psi = 0; psi < psi_size; psi++){
		r0 += height_map[psi][0];
		v0 += velocity_map[psi][0];
		r1 += height_map[psi][theta_size - 1];
		v1 += velocity_map[psi][theta_size - 1];
		}
	r0 /= psi_size;
	v0 /= psi_size;
	r1 /= psi_size;
	v1 /= psi_size;
	for(psi = 0; psi < psi_size; psi++){
		height_map[psi][0] = r0;
		velocity_map[psi][0] = v0;
		height_map[psi][theta_size - 1] = r1;
		velocity_map[psi][theta_size - 1] = v1;
		}

	position() += velocity*silicon_time_step;
	velocity -= (position() - initial_position)*(elastic_sphere_CM_stiffness*silicon_time_step);
	velocity *= zero_bound(1.f - (elastic_sphere_CM_damping + velocity.norm2()*elastic_sphere_CM_damping2)*silicon_time_step);
	CheckMatrix(GlobalMatrix);
}

void ElasticSphere::prepare()
{
	int psi;
	for(psi = 0; psi < psi_size; psi++)
		for(int theta = 0; theta < theta_size; theta++)
			points_map[psi][theta].scale(unit_sphere[psi][theta], height_map[psi][theta]);

	for(psi = 0; psi < psi_size; psi++)
		for(int theta = 0; theta < theta_size; theta++){
			normals_map[psi][theta].cross(point(psi - 1, theta) - point(psi + 1, theta), point(psi, theta + 1) - point(psi, theta - 1));
			normals_map[psi][theta].Normalize();
			}
	for(psi = 0; psi < psi_size; psi++){
		normals_map[psi][0] = Vect3f::K;
		normals_map[psi][theta_size - 1] = Vect3f::K_;
		}
}

void ElasticSphere::getAngles(const Vect3f& point, int& psi, int& theta)
{
	Vect3f v;
	GetGlobalMatrix().invXformPoint(point, v);
	psi = (int)round(v.psi()*psi_size/(M_PI*2)) & psi_mask;
	theta = (int)round(v.theta()*theta_size/M_PI) & theta_mask;
}	

void ElasticSphere::getPoint(int psi, int theta, Vect3f& point)
{
	point.setSpherical(psi*M_PI*2/psi_size, theta*M_PI/theta_size, height_map[psi][theta]*elastic_sphere_get_point_radius_factor);
	GetGlobalMatrix().xformPoint(point);
}	

////////////////////////////////////////////////////////////////
//		ElasticLink
////////////////////////////////////////////////////////////////
ElasticLink::ElasticLink(ElasticSphere* sphere1_, ElasticSphere* sphere2_)
: cIUnkObj(KIND_ELASTICLINK)
{
	sphere1 = sphere1_;
	sphere2 = sphere2_;
	sphere1->getAngles(sphere2->position(), psi1, theta1);
	sphere2->getAngles(sphere1->position(), psi2, theta2);
	sphere1->getPoint(psi1, theta1, point1);
	sphere2->getPoint(psi2, theta2, point2);
	initial_length = point1.distance(point2);
	thickness_factor = 1;
	line_size = round(initial_length*oscillating_link_density);
	x_height = new float[2*line_size];
	x_velocity = x_height + line_size;
	memset(x_height, 0, 2*line_size*sizeof(float));
	for(int i = 0; i < line_size; i++)
		x_height[i] = 20*sin(i*0.25);

	z_axis = point2 - point1;
	z_axis.Normalize();
	x_axis = Vect3f::ZERO;

	if(dot(z_axis, sphere2->position() - sphere1->position()) < 0)
		initial_length = -initial_length;

	blendMode = ALPHA_BLEND;
//	blendMode = ALPHA_ADDBLENDALPHA;

	Frame = new cFrame;
	Texture = Texture2 = 0;
	diffuse.set(1,1,1,1);
}

ElasticLink::~ElasticLink()
{
	if(Texture) { Texture->Release(); Texture=0; }
	if(Texture2) { Texture2->Release(); Texture2=0; }
	if(Frame) { delete Frame; Frame=0; }
	delete x_height;
}

void ElasticLink::evolve()
{
	sphere1->getPoint(psi1, theta1, point1);
	sphere2->getPoint(psi2, theta2, point2);

	LocalMatrix.trans() = GlobalMatrix.trans() = 0.5f*(point1 + point2);
	z_axis = point2 - point1;
	length = max(z_axis.norm(), 0.1f);
	GetRadius() = 0.5f*length;
	z_axis /= length;
	thickness_factor = pow(fabs(initial_length/length), elastic_link_thickness_pow);

	if(dot(z_axis, sphere2->position() - sphere1->position()) < 0)
		length = -length;

	float dl = length - initial_length;
	float vz = dot(sphere2->getVelocity() - sphere1->getVelocity(), z_axis) 
		- sphere1->getVelocityMap(psi1, theta1) - sphere2->getVelocityMap(psi2, theta2);
	Vect3f dvCM = z_axis*((dl*elastic_link_CM_stiffness + (elastic_link_CM_damping + sqr(vz)*elastic_link_CM_damping2)*vz)*silicon_time_step);
	float dv = (dl*elastic_link_stiffness + (elastic_link_damping + sqr(vz)*elastic_link_damping2)*vz)*silicon_time_step;

	sphere1->changeVelocity(dvCM);
	sphere2->changeVelocity(-dvCM);

	sphere1->changeVelocityMap(psi1, theta1, dv);
	sphere2->changeVelocityMap(psi2, theta2, dv);

//	x_velocity[1] += dot(sphere1->velocity, x_axis)*oscillating_link_velocity_factor;
//	x_velocity[line_size - 2] += dot(sphere2->velocity, x_axis)*oscillating_link_velocity_factor;

	float dz_factor = oscillating_link_stiffness*silicon_time_step;
	float spawn_factor = oscillating_link_spawn_factor*silicon_time_step;
	int i;
	for(i = 1; i < line_size - 1; i++){
		float v = x_velocity[i]*spawn_factor;
		float dz = dz_factor*x_height[i];
		x_velocity[i] -= dz + v;
		v *= 0.5f;
		v += dz*elastic_sphere_spawn_factor_dz;
		x_velocity[i - 1] += v;
		x_velocity[i + 1] += v;
		}

	x_velocity[0] = x_velocity[line_size - 1] = 0;

	for(i = 0; i < line_size; i++){
		x_velocity[i] *= zero_bound(1.f - (oscillating_link_damping + sqr(x_velocity[i])*oscillating_link_damping2)*silicon_time_step);
		x_height[i] += x_velocity[i]*silicon_time_step;
		}
}
void ElasticLink::SetColor(const sColor4f *ambient,const sColor4f *diffuse_,const sColor4f *specular)
{
	diffuse=*diffuse_;
}
void ElasticLink::SetTexture(cTexture *pTexture)
{
	if(Texture) { Texture->Release(); Texture=0; } 
	Texture=pTexture;
}
void ElasticLink::SetTexture2(cTexture *pTexture)
{
	if(Texture2) { Texture2->Release(); Texture2=0; } 
	Texture2=pTexture;
}
void ElasticLink::Animate(float dt)
{
//	evolve();
	GetFrame()->AddPhase(dt);
}
void ElasticLink::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)) return;
	if(!DrawNode->TestVisible(GetGlobalMatrix().trans(),GetRadius()))
		return;
	DrawNode->Attach(SCENENODE_OBJECTSORT,this);
}

void ElasticLink::Draw(cCamera *DrawNode)
{
	Vect3f p1, p2;
	DrawNode->ConvertorWorldToViewPort(&point1, 0, &p1);
	DrawNode->ConvertorWorldToViewPort(&point2, 0, &p2);
	Vect3f axis = p2 - p1;
	axis.z = 0;
	swap(axis.x, axis.y);
	axis.x = -axis.x;
	axis.Normalize();
	MatXf X;
	DrawNode->GetPosition(&X);
	X.invXformVect(axis);
	if(dot(axis, x_axis) < 0)
		axis.negate();
	x_axis = axis;
	if(line_size<2)
		return;
	float dz = length/line_size;

	int nVertex=0;

	DrawStripT2 strip;

	DWORD old_cull;
	old_cull=gb_RenderDevice3D->GetRenderState(D3DRS_CULLMODE);
	gb_RenderDevice3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	gb_RenderDevice->SetNoMaterial(blendMode,GetFrame()->GetPhase(),GetTexture(),GetTexture2(),COLOR_MOD);
	
	strip.Begin(MatXf(Mat3f::ID,point1));

	sColor4c Diffuse(diffuse);
	Vect3f Weight = 10*x_axis;
	float u1=0.f, du1=0.01f;
	float u2=2*GetFrame()->GetPhase(), du2=0.02f;
	float v2=0, dv2=0.01f;

	sVertexXYZDT2 vtx0,vtx1;
	for(int i = 0; i < line_size; i++)
	{
		float z = dz*i;
		float x = thickness_factor*x_height[i];
		if(z < oscillating_link_tail_length)
			x *= z/oscillating_link_tail_length;
		if(length - z < oscillating_link_tail_length)
			x *= (length - z)/oscillating_link_tail_length;
		Vect3f v = x*x_axis + z*z_axis;
		vtx0.pos = v-Weight;
		vtx1.pos = v+Weight;
		vtx0.diffuse=vtx1.diffuse=Diffuse;
		vtx0.u1()=   vtx1.u1()=(u1+=du1);
		vtx0.v1()=0; vtx1.v1()=1;
		vtx0.u2()=   vtx1.u2()=(u2+=du2);
		vtx0.v2()=(v2+=dv2);vtx1.v2()=v2+1;
		nVertex+=2;
		strip.Set(vtx0,vtx1);
	}

	strip.End();
	gb_RenderDevice3D->SetRenderState(D3DRS_CULLMODE, old_cull);
}


