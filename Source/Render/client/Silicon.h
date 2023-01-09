#ifndef __SILICON_H__
#define __SILICON_H__

#pragma once
//#include <list.h>

class ElasticSphere : public cIUnkObj
{
public:
	enum { 
		theta_size_len = 4,
		psi_size_len = theta_size_len + 1,
		psi_size = 1 << psi_size_len,
		theta_size = 1 << theta_size_len,
		psi_mask = psi_size - 1,
		theta_mask = theta_size - 1
		};

	ElasticSphere();
	~ElasticSphere();
	void setRadius(float radius, float relative_random_displacement);
	void evolve();
	void prepare(); // call before point() & normal()

	const Vect3f& point(int psi, int theta) { return points_map[psi & psi_mask][theta & theta_mask]; } 
	const Vect3f& normal(int psi, int theta) { return normals_map[psi & psi_mask][theta & theta_mask]; } 
	Vect3f& position(){ return GlobalMatrix.trans(); } 
	void getAngles(const Vect3f& point, int& psi, int& theta);
	void getPoint(int psi, int theta, Vect3f& point);

	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual void SetPosition(const MatXf& Matrix);
	virtual void Animate(float dt);
	virtual void GetColor(sColor4f *ambient,sColor4f *diffuse,sColor4f *specular=0) const;
	virtual void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);

	void SetTexture(int n,class cTexture *pTexture);
	inline const sColor4f& GetDiffuse()	const				{ return diffuse; }
	inline class cTexture* GetTexture(int n)				{ return Texture[n]; }
	inline class cFrame* GetFrame()							{ return Frame; }

	inline const Vect3f& getVelocity() const				{ return velocity; }
	inline float getVelocityMap(int psi, int theta) const	{ return velocity_map[psi][theta]; }

	inline void changeVelocity( const Vect3f& delta )				{ velocity += delta; }
	inline void changeVelocityMap(int psi, int theta, float delta) 	{ velocity_map[psi][theta] += delta; }

	float radius;
	eBlendMode blendMode;
private:
	Vect3f velocity, initial_position;

	float height_map[psi_size][theta_size];
	float velocity_map[psi_size][theta_size];
	static Vect3f unit_sphere[psi_size][theta_size];
	static Vect3f points_map[psi_size][theta_size];
	static Vect3f normals_map[psi_size][theta_size];

	class cTexture	*Texture[2];
	class cFrame	*Frame;
	sColor4f		diffuse;
};

class ElasticLink : public cIUnkObj
{
public:

	ElasticSphere *sphere1, *sphere2;
	int psi1, theta1;
	int psi2, theta2;
	float initial_length;

	int line_size;
	float* x_height;
	float* x_velocity;

	Vect3f x_axis;
	Vect3f z_axis;

	float length;
	float thickness_factor;
	Vect3f point1, point2;

	eBlendMode blendMode;

	ElasticLink(ElasticSphere* sphere1, ElasticSphere* sphere2);
	~ElasticLink();
	void Animate(float dt);
	void PreDraw(cCamera *UCamera);
	void Draw(cCamera *UCamera);
	void evolve();

	void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	void SetTexture(class cTexture *pTexture);
	void SetTexture2(class cTexture *pTexture);
	inline const sColor4f& GetDiffuse() const				{ return diffuse; }
	inline class cTexture* GetTexture()						{ return Texture; }
	inline class cTexture* GetTexture2()					{ return Texture2; }
	inline class cFrame* GetFrame()							{ return Frame; }

private:
	class cTexture	*Texture;
	class cTexture	*Texture2;
	class cFrame	*Frame;
	sColor4f		diffuse;
};	


#endif //__SILICON_H__
