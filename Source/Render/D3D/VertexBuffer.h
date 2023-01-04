#pragma once

class cVertexBufferInternal
{
protected:
	VertexBuffer* vb;
	int numvertex;
	int fmt;
	int cur_min_vertex,start_vertex;
public:
	cVertexBufferInternal();
	~cVertexBufferInternal();
	void Destroy();

	uint8_t* Lock(int minvertex);
	void Unlock(int num_write_vertex);
	inline int GetSize(){return numvertex-cur_min_vertex;};

	void Create(int bytesize,int vertexsize,int fmt);
	void DrawPrimitive(ePrimitiveType Type, uint32_t Count, const MatXf &m);
	void DrawPrimitive(ePrimitiveType Type, uint32_t Count);
	void DrawIndexedPrimitive(uint32_t Count);
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

	inline void DrawPrimitive(ePrimitiveType Type, uint32_t Count, const MatXf &m)
	{
		buf.DrawPrimitive(Type,Count,m);
	}

	inline void DrawPrimitive(ePrimitiveType Type, uint32_t Count)
	{
		buf.DrawPrimitive(Type,Count);
	}
};

class cQuadBufferInternal:protected cVertexBufferInternal
{
	uint8_t* start_vertex;
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

