#include "StdAfxRD.h"
#include "AnimChannelNode.h"
#include "..\..\PluginMAX\SceneMesh.h"

#include "LogicGeneric.h"

const int ATTRLOGIC_BORDER=1;
const int ATTRLOGIC_DELETE_ANIM=2;

cLogicTile::cLogicTile(int kind):cIUnkObj(kind)
{
}

class cLogicTileInt : public cLogicTile
{
	friend class cLogicGeneric;
	friend class cLogicObject;
protected:
	int					visible;
	string				name;

	bool				enable_rotate;
	Mat3f				RotateMatrix;

	//
	cAnimChannelNode*	AnimChannel;
	cIUnkObj			*Parent;
	bool				broot;
public:

	cLogicTileInt(const char *name,int kind);
	~cLogicTileInt();
	virtual int GetAnimation(MatXf *Matrix,int *Visible);

	virtual cIUnkClass* GetParent(){ return Parent; }
	void SetCopy(cIUnkObj* UObj);
	cIUnkObj* BuildCopy();

	cIUnkObj* GetRoot()
	{
		if(broot)
			return Parent;
		else
			return ((cLogicTileInt*)Parent)->GetRoot();
	}
protected:
	virtual void SetPosition(const MatXf& Matrix);
	virtual int SetChannel(const char *NameChainMask,float phase_);
	virtual void Draw(cCamera *UCamera);
	inline void SetNameObj(const char *Name)
	{
		if(Name)name=Name;
		else name.clear();
	}

	inline const char* GetNameObj()	const {return name.c_str();}
	virtual const char* GetName() const { return name.c_str();}

	inline void UpdatePosition()
	{
		GlobalMatrix=Parent->GetGlobalMatrix()*GetLocalMatrix();
		if(enable_rotate)
			GlobalMatrix.rot()*=RotateMatrix;

	}

	virtual void SetRotate(Mat3f* rotate);
	virtual cIUnkClass* NextObject(cIUnkClass *UObj);
};

cLogicTileInt::cLogicTileInt(const char *name,int kind):cLogicTile(kind)
{
	SetNameObj(name);
	Parent=NULL;
	visible=1;
	broot=false;
	enable_rotate=0;
	RotateMatrix=Mat3f::ZERO;
	AnimChannel=NULL;
	SetAttribute(ATTRLOGIC_DELETE_ANIM);
}

cLogicTileInt::~cLogicTileInt()
{
	if(GetAttribute(ATTRLOGIC_DELETE_ANIM))
	{
		delete AnimChannel;
	}
}

void cLogicTileInt::SetPosition(const MatXf& Matrix)
{
	VISASSERT(0);
}
int cLogicTileInt::SetChannel(const char *NameChainMask,float phase_)
{
	int number=AnimChannel->GetNumberChannel();
	int i;
	for(i = 0;i < number;i++)
		if(stricmp(AnimChannel->GetChannel(i)->GetName(),NameChainMask)==0)
			break;
	
	//xassert_s(i < AnimChannel->GetNumberChannel() && "Animation chain not found", NameChainMask);
	
	if(i>=number) return -2;
	//phase_=fmod(phase_+FRAME_PHASE_RANGE,FRAME_PHASE_RANGE); // анимироваться
	xassert(phase_>=0 && phase_<=1.0001);
	cAnimChainNode* anim=AnimChannel->GetChannel(i);
	anim->GetMatrix(phase_,LocalMatrix);
	anim->GetVisible(phase_,visible);
	UpdatePosition();
	
	return i;
}
int cLogicTileInt::GetAnimation(MatXf *Matrix,int *Visible) 
{
	if(Visible==0) Visible=&visible;
	if(Matrix) *Matrix=GetGlobalMatrix();
	return 0;
}

static void NormalizeMatrix(Mat3f& m)
{
	Vect3f vx,vy,vz;
	vx=m.xcol();vx.Normalize();
	vy=m.ycol();vy.Normalize();
	vz=m.zcol();vz.Normalize();

	m.setXcol(vx);
	m.setYcol(vy);
	m.setZcol(vz);
}

void DrawPointer(MatXf m,sColor4c color,float len,int xyz)
{
	NormalizeMatrix(m.rot());
	Vect3f p,d1,d2;
	switch(xyz)
	{
	case 0:
		p.set(len,0,0);
		d1.set(0,len*0.1f,0);
		d2.set(0,0,len*0.1f);
		break;
	case 1:
		p.set(0,len,0);
		d1.set(len*0.1f,0,0);
		d2.set(0,0,len*0.1f);
		break;
	case 2:
		p.set(0,0,len);
		d1.set(0,len*0.1f,0);
		d2.set(len*0.1f,0,0);
		break;
	default:
		VISASSERT(0);
		return;
	}
	Vect3f front=p*(1-0.2f);
	
	Vect3f p0=m*Vect3f::ZERO;
	Vect3f pp=m*p;

	gb_RenderDevice->DrawLine(p0,pp,color);
	gb_RenderDevice->DrawLine(pp,m*(front+d1),color);
	gb_RenderDevice->DrawLine(pp,m*(front-d1),color);
	
	gb_RenderDevice->DrawLine(pp,m*(front+d2),color);
	gb_RenderDevice->DrawLine(pp,m*(front-d2),color);
}
/*
void cLogicTileInt::Draw(cCamera *UCamera)
{
	cCamera *DrawNode=(cCamera*)UCamera;
	if(GetAttribute(ATTRUNKOBJ_SHADOW))
		DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),GetScale()*Vect3f(-5,-5,-5),GetScale()*Vect3f(5,5,5),0,sColor4c(255,0,0,255));
	else
		DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),GetScale()*Vect3f(-5,-5,-5),GetScale()*Vect3f(5,5,5));
}
*/

static float logic_radius=1;
void cLogicTileInt::Draw(cCamera *DrawNode)
{
	sColor4c color;
	if(GetAttribute(ATTRUNKOBJ_SHADOW))
	{
		DrawPointer(GlobalMatrix,sColor4c(255,0,0),logic_radius*0.1f,0);
		DrawPointer(GlobalMatrix,sColor4c(0,255,0),logic_radius*0.1f,1);
		DrawPointer(GlobalMatrix,sColor4c(0,0,255),logic_radius*0.1f,2);
	}else
	{
		DrawPointer(GlobalMatrix,sColor4c(255,255,255),logic_radius*0.1f,1);
	}
}

void cLogicTileInt::SetRotate(Mat3f* rotate)
{
	if(rotate)
	{
		enable_rotate=true;
		RotateMatrix=*rotate;
	}else
	{
		enable_rotate=false;
	}
}
cIUnkClass* cLogicTileInt::NextObject(cIUnkClass *UObj)
{
	return NULL;
}

void cLogicTileInt::SetCopy(cIUnkObj* UObj)
{
	cLogicTile::SetCopy(UObj);
	cLogicTileInt* p=(cLogicTileInt*)UObj;

	p->visible=visible;
	p->enable_rotate=enable_rotate;
	p->RotateMatrix=RotateMatrix;

	//
	p->AnimChannel=AnimChannel;
	p->broot=broot;
	p->Parent=NULL;

	p->ClearAttribute(ATTRLOGIC_DELETE_ANIM);
}

cIUnkObj* cLogicTileInt::BuildCopy()
{
	cLogicTileInt* p=new cLogicTileInt(name.c_str(),GetKind());
	SetCopy(p);
	return p;
}

class cLogicConnect : public cLogicTileInt
{
public:
	cLogicConnect(const char *name):cLogicTileInt(name,KIND_LOGIC_BOUND)
	{
	}
	cIUnkObj* BuildCopy()
	{
		cLogicConnect* p=new cLogicConnect(name.c_str());
		SetCopy(p);
		return p;
	}
};

class cLogicMesh : public cLogicTileInt
{
public:
	cLogicMesh(const char *name):cLogicTileInt(name,KIND_LOGIC_BOUND)
	{
	}
	virtual float GetBoundRadius()
	{
		Vect3f mi,ma;
		float br=Box.min.distance(Box.max)/2;
		mi=GetGlobalMatrix()*Box.min;
		ma=GetGlobalMatrix()*Box.max;
		float Radius=mi.distance(ma)/2;
		return Radius;
	}

	virtual void GetBoundBox(sBox6f& OutBox)
	{ 
		//Потом перемпавить без умножения на матрицу
		Vect3f min=GetGlobalMatrix().xformPoint(Box.min,min);
		Vect3f max=GetGlobalMatrix().xformPoint(Box.max,max);
		OutBox.min=::GetMin(min,max);
		OutBox.max=::GetMax(min,max);
	}
	virtual void Draw(cCamera *DrawNode)
	{
		if(!GetRoot()->GetAttr(ATTRUNKOBJ_SHOWBOUND))
			return;

		if(GetAttribute(ATTRUNKOBJ_SHADOW))
			DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),Box.min,Box.max,0,sColor4c(255,0,0,255));
		else
			DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),Box.min,Box.max);
	}

	void FirstCalcBound(sAnimationMesh *AnimationMesh);

	void SetCopy(cIUnkObj* UObj)
	{
		cLogicTileInt::SetCopy(UObj);
		cLogicMesh* p=(cLogicMesh*)UObj;
		p->Box=Box;
	}

	cIUnkObj* BuildCopy()
	{
		cLogicMesh* p=new cLogicMesh(name.c_str());
		SetCopy(p);
		return p;
	}
protected:
	sBox6f	Box;
};

void GetMatrixObj(int time,MatXf &Matrix,sLodObject *LodObject,sNodeObject *pNodeObject,char *parent);
void ReadAnimNode(sChannelAnimation *Channel,sLodObject *LodObject,cAnimChannelNode *AnimChannel,sNodeObject *NodeObject,const char *parent);

static void ReadAnimNodeLogic(sChannelAnimation *Channel,sLodObject *LodObject,cAnimChannelNode *AnimChannel,sNodeObject *NodeObject,const char *parent)
{
	ReadAnimNode(Channel,LodObject,AnimChannel,NodeObject,parent);

	cAnimChainNode *AnimChain=AnimChannel->GetChannel(Channel->ID);
	AnimChain->SetName(Channel->name.c_str());
}

cLogicGeneric *gb_LogicGeneric=0;

cLogicGeneric::cLogicGeneric()	: cUnknownClass(KIND_LIB_LOGIC)
{
}
cLogicGeneric::~cLogicGeneric()
{
	Free();
	gb_LogicGeneric=0;
}
void cLogicGeneric::Free()
{
	for(int i=objects.size()-1;i>=0;i--)
	if(objects[i] && objects[i]->GetRef()==1)
	{
		objects[i]->Release();
	}

	objects.clear();
}
cLogicObject* cLogicGeneric::GetElement(const char *pFName)
{ 
	char *fname=(char*)pFName;
	if(fname==0||fname[0]==0) return 0;
	for(int i=0;i<objects.size();i++)
	if(objects[i] && (stricmp(objects[i]->GetName(),fname))==0 )
	{
/*Ссылка на один объект
		objects[i]->IncRef();
		return objects[i];
/*/
		cLogicObject* p=(cLogicObject*)objects[i]->BuildCopy();
		return p;
/**/
	}

	// загрузка логического объекта
	int size=0;
	char *buf=0;
	if(ResourceFileRead(fname,buf,size)){
		ErrH.Abort("Logic model not found", XERR_USER, 0, fname);
		return 0;
	}
	cMeshScene MeshScene; // загрузка MeshScene сцены из файла
	cMeshFile f;
	if(f.OpenRead(buf,size)==MESHFILE_NOT_FOUND) return 0;
	f.ReadHeaderFile();
	MeshScene.Read(f);
	f.Close();
	int nChannel;
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{
		char* cur=(char*)MeshScene.ChannelLibrary[nChannel]->name.c_str();
		_strlwr(cur);
	}
/*	// поиск и установка первым канала анимации с именем "main"
	int nChannelMain=-1;
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{
		char* cur=(char*)MeshScene.ChannelLibrary[nChannel]->name.c_str();
		_strlwr(cur);
		if(stricmp(cur,"main")==0)
			nChannelMain=nChannel;
	}
	if(nChannelMain>=0)
	{
		sChannelAnimation *tmp=MeshScene.ChannelLibrary[0];
		MeshScene.ChannelLibrary[0]=MeshScene.ChannelLibrary[nChannelMain];
		MeshScene.ChannelLibrary[nChannelMain]=tmp;
	}
*/
	cLogicObject *LogicObj=new cLogicObject(fname);
	// создание объекта по описаннию в сцене
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{ // импорт канала анимации
		sChannelAnimation *Channel=MeshScene.ChannelLibrary[nChannel];
		int FirstTime=Channel->FirstFrame*Channel->TicksPerFrame; Channel->ID=nChannel;
		for(int LevelDetail=0;LevelDetail<Channel->LodLibrary.length();LevelDetail++)
		{ // импорт уровня детализации
			sLodObject *LodObject=Channel->LodLibrary[LevelDetail];
			for(int nNodeObject=0;nNodeObject<LodObject->NodeObjectLibrary.length();nNodeObject++)
			{ // импорт корневого объекта
				sNodeObject *NodeObject=LodObject->NodeObjectLibrary[nNodeObject];
				char NameMesh[256],NameParent[256];
				strcpy(NameMesh,NodeObject->name.c_str()); 
				if(!NodeObject->parent.empty()) strcpy(NameParent,NodeObject->parent.c_str()); else NameParent[0]=0;
				if((!NodeObject->name.empty())&&(TestFirstName(NodeObject->name.c_str(),"Bip")))
					continue; // пропустить "Bip" из CharacterStudio
				cObjectNode *CurrentNode=0;
				switch(NodeObject->type)
				{
					case NODEOBJECT_MESH:
						{ // импорт 3d-объекта
							sObjectMesh *ObjectMesh=(sObjectMesh*)NodeObject;
							char *LogicName=TestFirstName(NodeObject->name.c_str(),"logic ");
							cLogicMesh *LogicTile=(cLogicMesh*)LogicObj->FindObject(LogicName);
							bool first=false;
							if(LogicTile==0) 
							{
								first=true;
								LogicTile=new cLogicMesh(LogicName);
								LogicTile->SetAttribute(ATTRLOGIC_BORDER);
								LogicTile->AnimChannel=new cAnimChannelNode;
								LogicTile->AnimChannel->NewChannel(MeshScene.ChannelLibrary.length());
								if(!NodeObject->parent.empty())
								{
									char* parent_name=TestFirstName(NodeObject->parent.c_str(),"logic ");
									LogicTile->Parent=LogicObj->FindObject(parent_name);
								}

								LogicObj->Attach(LogicTile);
							}
							ReadAnimNodeLogic(Channel,LodObject,LogicTile->AnimChannel,ObjectMesh,ObjectMesh->parent.c_str());

							if(first)
								LogicTile->FirstCalcBound(ObjectMesh->AnimationMeshLibrary[0]);
						}
						break;
					case NODEOBJECT_HELPER:
						{ // импорт вспомогательного-объекта
							sHelperObject *HelperObject=(sHelperObject*)NodeObject;
							char *LogicName=TestFirstName(NodeObject->name.c_str(),"logic ");
							VISASSERT(LogicName && "Uncorrect l3d model");
							cLogicConnect *LogicTile=(cLogicConnect*)LogicObj->FindObject(LogicName);
							if(LogicTile==0) 
							{
								LogicTile=new cLogicConnect(LogicName);
								LogicTile->AnimChannel=new cAnimChannelNode;
								LogicTile->AnimChannel->NewChannel(MeshScene.ChannelLibrary.length());
								if(!NodeObject->parent.empty())
									LogicTile->Parent=(cUnkObj*)LogicObj->FindObject(TestFirstName(NodeObject->parent.c_str(),"logic "));
								LogicObj->Attach(LogicTile);
							}
							ReadAnimNodeLogic(Channel,LodObject,LogicTile->AnimChannel,HelperObject,HelperObject->parent.c_str());
						}
						break;
					default:
						VISASSERT(NodeObject->type==NODEOBJECT_MESH||NodeObject->type==NODEOBJECT_HELPER);
				}
			}
		}
	}
	objects.push_back(LogicObj); 
	LogicObj->SetChannel(MeshScene.ChannelLibrary[0]->name.c_str(),0);
/*
	LogicObj->IncRef();
	return LogicObj;
/*/
	return (cLogicObject*)LogicObj->BuildCopy();
/**/
}

cLogicGeneric* CreateILogicGeneric()
{
	if(gb_LogicGeneric) gb_LogicGeneric->IncRef();
	else gb_LogicGeneric=new cLogicGeneric;
	return gb_LogicGeneric;
}


///////////////////////////////////////////////////////////////////////
cLogicObject::cLogicObject(const char *fname_):cIUnkObjScale(KIND_LOGIC_OBJ)
{
	fname=fname_;
}
cLogicObject::~cLogicObject()
{
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	{
		delete *it;
	}
}
void cLogicObject::SetScale(const Vect3f& scale)
{ 
	cIUnkObjScale::Scale=scale;
}

float cLogicObject::GetBoundRadius()
{ 
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	if((*it)->GetAttribute(ATTRLOGIC_BORDER))
	{
		return (*it)->GetBoundRadius();
	}	
	
	return 0;
}

void cLogicObject::GetBoundBox(sBox6f& Box)
{ 
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	if((*it)->GetAttribute(ATTRLOGIC_BORDER))
	{
		(*it)->GetBoundBox(Box);
		return;
	}	
	
	Box.min.set(0,0,0);
	Box.max.set(0,0,0);
}

cLogicTile* cLogicObject::FindObject(const char *name)
{
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	{
		const char* tile_name=(*it)->GetNameObj();
		if(tile_name && name && stricmp(tile_name,name)==0)
			return *it;
		if(tile_name==NULL && name==NULL)
			return *it;
	}
	return NULL;
}

void cLogicObject::SetPosition(const MatXf& Matrix)
{ 
	LocalMatrix=Matrix;
	GlobalMatrix=LocalMatrix;
	GlobalMatrix.rot().scale(GetScale());
	Update();
}

void cLogicObject::Update()
{
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	{
		cLogicTileInt* p=*it;
		if(p->Parent==this)
			p->UpdatePosition();
	}

	FOR_EACH(tiles,it)
	{
		cLogicTileInt* p=*it;
		if(p->Parent!=this)
			p->UpdatePosition();
	}
}

int cLogicObject::SetChannel(const char *NameChainMask,float phase_)
{ 
	xassert(phase_>=0 && phase_<=1.00001);
	int ret=-2;
	vtiles::iterator it;
	FOR_EACH(tiles,it)
		ret=(*it)->SetChannel(NameChainMask,phase_);
	return ret;
}
void cLogicObject::Draw(cCamera *UCamera)
{
	cCamera *DrawNode=(cCamera*)UCamera;

	logic_radius=GetBoundRadius();
	const float s=3;
	if(GetAttribute(ATTRUNKOBJ_SHADOW))
		DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),Vect3f(-s,-s,-s),Vect3f(s,s,s),0,sColor4c(255,0,0,255));
	else 
		DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),Vect3f(-s,-s,-s),Vect3f(s,s,s),0,sColor4c(0,255,0,255));
	vtiles::iterator it;
	FOR_EACH(tiles,it)
		(*it)->Draw(UCamera);
}

void cLogicObject::Attach(cLogicTileInt* p)
{
	if(!p->Parent)
	{
		p->Parent=this;
		p->broot=true;
	}
	tiles.push_back(p);
}

cIUnkClass* cLogicObject::NextObject(cIUnkClass *UObj)
{
	if(tiles.empty())
		return NULL;
	if(UObj==NULL)
		return tiles[0];

	for(int i=1;i<tiles.size();i++)
		if(tiles[i-1]==UObj)
			return tiles[i];

	return NULL;
}

void cLogicMesh::FirstCalcBound(sAnimationMesh *AnimationMesh)
{
	MatXf mat;
	AnimChannel->GetChannel(0)->GetMatrix(0,mat);
	Box.min.set(1e30f,1e30f,1e30f);
	Box.max.set(-1e30f,-1e30f,-1e30f);
	sVertexMesh	&Vertex=AnimationMesh->Vertex;
	for(int k=0;k<Vertex.length();k++)
	{ // импорт пространственных координат
		Vect3f pos(Vertex[k][0],Vertex[k][1],Vertex[k][2]);
		RightToLeft(pos);
		Vect3f p=mat*pos;
		Box.AddBound(pos);
	}
}

void cLogicObject::SetCopy(cIUnkObj* UObj)
{
	cIUnkObjScale::SetCopy(UObj);
	cLogicObject* logic=(cLogicObject*)UObj;
	vtiles::iterator it;
	FOR_EACH(tiles,it)
	{
		cLogicTileInt *cur,*to;
		cur=*it;
		to=(cLogicTileInt*)cur->BuildCopy();
		if(cur->broot)
		{
			to->Parent=logic;
		}else
		{
			cLogicTile* parent=logic->FindObject(cur->Parent->GetName());
			VISASSERT(parent);
			to->Parent=parent;
		}

		logic->tiles.push_back(to);
	}
}

cIUnkObj* cLogicObject::BuildCopy()
{
	cLogicObject* p=new cLogicObject(fname.c_str());
	SetCopy(p);
	return p;
}
