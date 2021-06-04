#pragma once

class cVertexBufferInternal
{
protected:
	sPtrVertexBuffer vb;
	int numvertex;
	int fmt;
	int cur_min_vertex,start_vertex;
public:
	cVertexBufferInternal();
	~cVertexBufferInternal();
	void Destroy();

	BYTE* Lock(int minvertex);
	void Unlock(int num_write_vertex);
	inline int GetSize(){return numvertex-cur_min_vertex;};

	void Create(int bytesize,int vertexsize,int fmt);
	void DrawPrimitive(PRIMITIVETYPE Type,UINT Count,const MatXf &m);
	void DrawPrimitive(PRIMITIVETYPE Type,UINT Count);
	void DrawIndexedPrimitive(UINT Count);
};

template <class vertex>
class cVertexBuffer
{
	cVertexBufferInternal buf;
public:

	inline void Destroy(){buf.Destroy();};
	inline vertex* Lock(int minvertex=40){return (vertex*)buf.Lock(minvertex);};
	inline void Unlock(int num_write_vertex){buf.Unlock(num_write_vertex);};
	inline int GetSize(){return buf.GetSize();};

	inline void Create(int bytesize)
	{
		buf.Create(bytesize,sizeof(vertex),vertex::fmt);
	}

	inline void DrawPrimitive(PRIMITIVETYPE Type,UINT Count,const MatXf &m)
	{
		buf.DrawPrimitive(Type,Count,m);
	}

	inline void DrawPrimitive(PRIMITIVETYPE Type,UINT Count)
	{
		buf.DrawPrimitive(Type,Count);
	}

};

class cQuadBufferInternal:protected cVertexBufferInternal
{
	BYTE* start_vertex;
	int vertex_index;
public:
	cQuadBufferInternal();
	~cQuadBufferInternal();

	void Destroy();
	void Create(int vertexsize,int fmt);

	void SetMatrix(const MatXf &m);
	void BeginDraw();
	void* Get();
	void EndDraw();
};

template <class vertex>
class cQuadBuffer
{
	cQuadBufferInternal buf;
public:

	inline void Destroy(){buf.Destroy();}
	inline void Create()
	{
		buf.Create(sizeof(vertex),vertex::fmt);
	}

	inline void BeginDraw(const MatXf &m=MatXf::ID){buf.SetMatrix(m);buf.BeginDraw();};
	//Возвращает указатель на 4 вершины
	inline vertex* Get(){return (vertex*)buf.Get();}
	inline void EndDraw(){buf.EndDraw();};
};

class DrawStrip
{
	cVertexBuffer<sVertexXYZDT1>* buf;
	sVertexXYZDT1* pointer;
	int num;
public:
	DrawStrip():buf(NULL){ }

	void Begin();
	void End();
	inline void Set(sVertexXYZDT1& p1,sVertexXYZDT1& p2);
};

void DrawStrip::Set(sVertexXYZDT1& p1,sVertexXYZDT1& p2)
{
	pointer[num++]=p1;
	pointer[num++]=p2;
	if(num+4>buf->GetSize())
	{
		buf->Unlock(num);
		buf->DrawPrimitive(PT_TRIANGLESTRIP,num-2);

		pointer=buf->Lock(8);
		num=0;

		pointer[num++]=p1;
		pointer[num++]=p2;
	}
}

class DrawStripT2
{
	cVertexBuffer<sVertexXYZDT2>* buf;
	sVertexXYZDT2* pointer;
	int num;
public:
	DrawStripT2():buf(NULL){ }

	void Begin(const MatXf& mat=MatXf::ID);
	void End();
	inline void Set(sVertexXYZDT2& p1,sVertexXYZDT2& p2);
};

void DrawStripT2::Set(sVertexXYZDT2& p1,sVertexXYZDT2& p2)
{
	pointer[num++]=p1;
	pointer[num++]=p2;
	if(num+4>buf->GetSize())
	{
		buf->Unlock(num);
		buf->DrawPrimitive(PT_TRIANGLESTRIP,num-2);

		pointer=buf->Lock(8);
		num=0;

		pointer[num++]=p1;
		pointer[num++]=p2;
	}
}

