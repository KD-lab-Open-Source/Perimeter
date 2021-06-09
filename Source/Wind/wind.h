#include "../Water/Water.h"
struct WindNode
{
	float maxz;
	Vect2f vel;
};

class cmGrid
{
	int x_count;
	int y_count;
	int shift;
	int ng;
public:
	WindNode* nds;
	cmGrid() : nds(NULL){}
	~cmGrid(){ delete[] nds;}
	void InitGrid(int w,int h, int n);
	template<class Op> void Scan(float xp, float yp,float r,Op& op);
	Vect3f GetVelocity(Vect3f& pos);
	void Clear();
	void DrawGrid();
	void DrawZ();

	Vect2i GetSize(){return Vect2i(x_count,y_count);}
};
struct cQuantumWind
{
	cEffect* ef;
	Vect2f pos;
	Vect2f vel;
	float r;
	float dr;
	float k_fading;
	float maxz;
	float surf_z;
	cQuantumWind():ef(NULL){};
	~cQuantumWind();
	virtual void operator()(WindNode& nd, float x, float y);
	void Animate(float dt);
	virtual void Draw();
	bool IsLive();
};
struct cQuantumWindW:public cQuantumWind
{
	float w;
	virtual void operator()(WindNode& nd, float x, float y);
	virtual void Draw();
};

struct NodeObj
{
	int ix;
	Vect3f vel;
	Vect3f dis;
	NodeObj(int i){ix= i;vel = dis = Vect3f::ZERO;}
};

struct ObjNodes
{
	cObject3dx* obj;
	vector<NodeObj> inds;
	void Set(cObject3dx* pObj);
};

class cMapWind
{
	cWater* pWater;
	int map_width,map_heigth;
	cmGrid grid;
	vector<cQuantumWind*> quants;
	vector<ObjNodes> objects;
public: ///temp
	void Space(int mode);
	void Again();
public:
	cMapWind();
	~cMapWind();
	float GetZ(int x, int y){return pWater->GetZ(x,y);}
	bool show_grid;
	bool show_z;
	Vect3f GetVelocity(Vect3f& pos);
	void Draw();
	void Animate(float dt);
	void CreateQuant(Vect2f& pos, Vect2f& vel, float max_z, float r, float dr, float fading);
	void CreateQuantW(Vect2f& pos, Vect2f& vel, float max_z, float r, float dr, float fading, float w);
	void Init(int w, int h, int n,cWater* pWater); 
	Vect2i GetGridSize(){return grid.GetSize();}
	void GetSpeed(BYTE* data,int pitch,float mul);
};

