#pragma once

struct sVertexLine
{
	Vect3f		pos;
	sColor4c	color;
	float		v;//Сдвиг текстуры
	float		width;
};

class cLine3d : public cAnimUnkObj
{
protected:
	vector<sVertexLine>		Vertex;
	sBox6f					Bound;
	MTDECLARE(lock);
public:

	cLine3d();
	virtual ~cLine3d();
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *UClass);
	virtual void Draw(cCamera *UClass);
	void UpdateVertex(int num_vertex, sVertexLine* varray);

	//num_vertex должно совпадать с Vertex.size()
	void UpdateVertexPos(int num_vertex, Vect3f* varray);

	virtual Vect3f GetCenterObject()	{return (Bound.min+Bound.max)*0.5f;}
};

