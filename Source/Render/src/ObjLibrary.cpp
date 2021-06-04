#include "StdAfxRD.h"
#include "ObjLibrary.h"
#include "SceneMesh.h"
#include "AnimChannel.h"
#include "ObjMesh.h"
#include "ObjLight.h"
#include "MeshBank.h"
#include "NParticle.h"

bool is_old_model=false;
bool WinVGIsOldModel()
{
	return is_old_model;
}

int ResourceFileRead(const char *fname,char *&buf,int &size)
{
	buf=0; size=0;
	ZIPStream f(fname);
	if(!!f) { f.close(); return -1<<0; }
	size=f.size();
	buf=new char[size];
	f.read(buf,size);
	f.close();
	return 0;
}

bool ResourceIsZIP()
{
	return ZIPIsOpen();
}
//////////////////////////////////////////////////////////////////////////////////////////
// вспомогательные инлайновые функции чтения
//////////////////////////////////////////////////////////////////////////////////////////

class CM3DError
{
	string fname;
public:
	void SetName(const char* a_fname){fname=a_fname;}
	cVisError& operator ()(){return VisError<<"Error LoadM3D() file: "<<fname.c_str()<<"\r\n";}
};

static CM3DError m3derror;

void GetMatrixObj(int time,MatXf &Matrix,sLodObject *LodObject,sNodeObject *NodeObject)
{
	Identity(Matrix);
	MatXf AnimationMatrix; // матрица положения объекта относительного родителя
	Identity(AnimationMatrix); // установка матрицы анимации
	sAnimationPosition &AnimationPosition=NodeObject->AnimationPosition;
	sAnimationRotation &AnimationRotation=NodeObject->AnimationRotation;
	sAnimationScale &AnimationScale=NodeObject->AnimationScale;
	sAnimationScaleRotation &AnimationScaleRotation=NodeObject->AnimationScaleRotation;
	if(AnimationPosition.length()==1)
		Translate(AnimationMatrix,Vect3f(AnimationPosition[0][1],AnimationPosition[0][2],AnimationPosition[0][3]));
	else
	{
		int dt=0x0FFFFFFF,n=-1;
		for(int i=0;i<AnimationPosition.length();i++)
			if(ABS(AnimationPosition[i][0]-time)<dt)
				dt=ABS(AnimationPosition[i][0]-time),n=i;
		if(n>=0)
			Translate(AnimationMatrix,Vect3f(AnimationPosition[n][1],AnimationPosition[n][2],AnimationPosition[n][3]));
	}
	if(AnimationRotation.length()==1) 
		Rotate(AnimationMatrix,QuatF(-AnimationRotation[0][4],AnimationRotation[0][1],AnimationRotation[0][2],AnimationRotation[0][3]));
	else 
	{
		int dt=0x0FFFFFFF,n=-1;
		for(int i=0;i<AnimationRotation.length();i++)
			if(ABS(AnimationRotation[i][0]-time)<dt)
				dt=ABS(AnimationRotation[i][0]-time),n=i;
		if(n>=0)
			Rotate(AnimationMatrix,QuatF(-AnimationRotation[n][4],AnimationRotation[n][1],AnimationRotation[n][2],AnimationRotation[n][3]));
	}
	if(AnimationScale.length()==1) 
		Scale(AnimationMatrix,Vect3f(AnimationScale[0][1],AnimationScale[0][2],AnimationScale[0][3]));
	else
	{
		int dt=0x0FFFFFFF,n=-1;
		for(int i=0;i<AnimationScale.length();i++)
			if(ABS(AnimationScale[i][0]-time)<dt)
				dt=ABS(AnimationScale[i][0]-time),n=i;
		if(n>=0)
			Scale(AnimationMatrix,Vect3f(AnimationScale[n][1],AnimationScale[n][2],AnimationScale[n][3]));
	}
	if(AnimationScaleRotation.length()==1) 
		Rotate(AnimationMatrix,QuatF(-AnimationScaleRotation[0][4],AnimationScaleRotation[0][1],AnimationScaleRotation[0][2],AnimationScaleRotation[0][3]));
	else 
	{
		int dt=0x0FFFFFFF,n=-1;
		for(int i=0;i<AnimationScaleRotation.length();i++)
			if(ABS(AnimationScaleRotation[i][0]-time)<dt)
				dt=ABS(AnimationScaleRotation[i][0]-time),n=i;
		if(n>=0)
			Rotate(AnimationMatrix,QuatF(-AnimationScaleRotation[n][4],AnimationScaleRotation[n][1],AnimationScaleRotation[n][2],AnimationScaleRotation[n][3]));
	}
	RightToLeft(AnimationMatrix);
	Matrix=AnimationMatrix*Matrix;
}

static void ReadMeshBound(int time,sBound *Bound,sObjectMesh *ObjectMesh,sLodObject *LodObject,const char *parent)
{ // импорт геометрии объекта
	sAnimationMesh *AnimationMesh=0;
	for(int nAnimMesh=0;nAnimMesh<ObjectMesh->AnimationMeshLibrary.length();nAnimMesh++)
		if(ObjectMesh->AnimationMeshLibrary[nAnimMesh]->time<=time)
			AnimationMesh=ObjectMesh->AnimationMeshLibrary[0];
	sFaceMesh &Face=AnimationMesh->Face;
	sVertexMesh	&Vert=AnimationMesh->Vertex;
	if(Face.length()<=0||Vert.length()<3)
		m3derror()<<"Object "<<ObjectMesh->name.c_str()<<" don't has polygons"<<VERR_END;
	// импорт пространственных координат
	MatXf Matrix;
	GetMatrixObj(time,Matrix,LodObject,ObjectMesh);
	Bound->Vertex.resize(Vert.length());
	int k;
	for(k=0;k<Vert.length();k++)
	{
		Vect3f v( Vert[k][0],Vert[k][1],Vert[k][2] );
		RightToLeft(v);
		Matrix.xformPoint( v, Bound->Vertex[k] );
	}
	Bound->Poly.resize(Face.length());
	for(k=0;k<Face.length();k++)
		Bound->Poly[k].set(Face[k][0],Face[k][1],Face[k][2]);
}

static void ReadMeshTri(int time,cObjMesh *Mesh,sObjectMesh *ObjectMesh,cAllMeshBank* pBanks)
{ // импорт геометрии объекта
	sAnimationMesh *AnimationMesh=0;
	for(int nAnimMesh=0;nAnimMesh<ObjectMesh->AnimationMeshLibrary.length();nAnimMesh++)
		if(ObjectMesh->AnimationMeshLibrary[nAnimMesh]->time<=time)
			AnimationMesh=ObjectMesh->AnimationMeshLibrary[0];
	sVertexMesh	&Vert=AnimationMesh->Vertex;
	sFaceMesh &Face=AnimationMesh->Face;
	vector<Vect2f> Texel;
	vector<Vect3f> Vertex(Vert.length());
	// импорт пространственных координат
	int k;
	for(k=0;k<Vertex.size();k++) 
	{
		Vertex[k].set(Vert[k][0],Vert[k][1],Vert[k][2]);
		RightToLeft(Vertex[k]);
	}

	sVertexNormalMesh	&Norm=AnimationMesh->VertexNormal;
	vector<Vect3f> Normal(Norm.length());
	for(k=0;k<Normal.size();k++) 
	{
		Normal[k].set(Norm[k][0],Norm[k][1],Norm[k][2]);
		//RightToLeft Normal
		Normal[k].x=-Normal[k].x;
		Normal[k].z=-Normal[k].z;
	}

	if(Face.length()<=0||Vert.length()<3)
		m3derror()<<" Object "<<ObjectMesh->name.c_str()<<" don't has polygons"<<VERR_END;
	vector<sPolygon> Polygon(Face.length()),TexPoly;
	for(k=0;k<Face.length();k++)
		Polygon[k].set(Face[k][0],Face[k][1],Face[k][2]);
	if(AnimationMesh->ChannelMappingLibrary.length())
	{ // импорт текстурных координат
		sChannelMapping *ChannelMapping=AnimationMesh->ChannelMappingLibrary[0];
		sTexFaceMesh &TexFace=ChannelMapping->TexFace;
		sTexVertexMesh &TexVertex=ChannelMapping->TexVertex;
		Texel.resize(TexVertex.length());
		for(int k=0;k<TexVertex.length();k++)
			Texel[k].set(TexVertex[k][0],TexVertex[k][1]),
			RightToLeft(Texel[k]);
		TexPoly.resize(TexFace.length());
		for(k=0;k<TexFace.length();k++)
			TexPoly[k].set(TexFace[k][0],TexFace[k][1],TexFace[k][2]);
	}

	if(Mesh->GetBank()->IsBump() && Texel.size()==0)
	{
		m3derror()<<" Object "<<ObjectMesh->name.c_str()<<" witch bump must have UVmap"<<VERR_END;
	}

	if(Normal.empty())
	{
		is_old_model=true;
		Mesh->SetTri(Mesh->GetBank()->AddMesh(Vertex,Polygon,TexPoly,Texel));
	}else
	{
		is_old_model=false;
		VISASSERT(TexPoly.empty());
		Mesh->SetTri(Mesh->GetBank()->AddMesh(Vertex,Polygon,Normal,Texel));
	}
}

cTexture* LoadTextureDef(const char* name,const char* path,const char* def_path,char* attr=NULL)
{
	char full_name[512];
	strcpy(full_name,path);
	strcat(full_name,name);

	bool enable_error=GetTexLibrary()->EnableError(false);

	cTexture *Texture=GetTexLibrary()->GetElement(full_name,attr);
	if(Texture==0)
	{
		if(def_path)
		{
			strcpy(full_name,def_path);
			strcat(full_name,name);
			Texture=GetTexLibrary()->GetElement(full_name,attr);
		}
		if(Texture==0)
		{
			strcpy(full_name,path);
			strcat(full_name,name);
			m3derror()<<"Texture not found - "<<full_name<<VERR_END;
		}
	}

	GetTexLibrary()->EnableError(enable_error);
	return Texture;
}

static
cMeshBank* ReadMeshMat(cAllMeshBank *pBanks,sObjectMesh *ObjectMesh,cMaterialObjectLibrary& MaterialLibrary,
						char *TexturePath,char* DefTexturePath,cObjLibrary *ObjLibrary)
{ // импорт материала объекта
	if(ObjectMesh->AnimationMeshLibrary.length()==0)
	{
		m3derror()<<"not found Animation Mesh in "<<ObjectMesh->name.c_str()<<VERR_END;
		return NULL;
	}

	VISASSERT(ObjectMesh->AnimationMeshLibrary[0]);
	if(ObjectMesh->AnimationMeshLibrary[0]->Face.length()==0)
	{
		m3derror()<<"in object is 0 faces - "<<ObjectMesh->name.c_str()<<VERR_END;
		return NULL;
	}

	int nMaterial=ObjectMesh->AnimationMeshLibrary[0]->Face[0][3];
	if(nMaterial<0||nMaterial>=MaterialLibrary.length())
		return NULL;

	sMaterialObject *Material=MaterialLibrary[nMaterial];
	//Read Texture
	char TextureName[512]="",OpacityName[512]="",
		 ReflectionName[512]="",BumpName[512]="";
	for(int nSubTex=0;nSubTex<Material->SubTexMap.length();nSubTex++)
	{
		LPCSTR name=GetFileName(Material->SubTexMap[nSubTex]->name.c_str());
		switch(Material->SubTexMap[nSubTex]->ID)
		{
			case TEXMAP_DI:
				strcpy(TextureName,name);
				break;
			case TEXMAP_OP:
				strcpy(OpacityName,name);
				break;
			case TEXMAP_SP:
				break;
			case TEXMAP_SH:
				break;
			case TEXMAP_SI:
				break;
			case TEXMAP_FI:
				break;
			case TEXMAP_RL:
				strcpy(ReflectionName,name);
				break;
			case TEXMAP_BU:
				strcpy(BumpName,name);
				break;
			case TEXMAP_DP:
				break;
			case TEXMAP_SS:
				break;
			default:
				;//VISASSERT(0);
		}
	}
	
	sAttribute ObjectAttribute,MatAttribute;
	if(Material->Falloff)
		ObjectAttribute.SetAttribute(ATTRUNKOBJ_COLLISIONTRACE); // skin
	if(Material->Shading==1) // phong
		MatAttribute.SetAttribute(0);
	else // blin
		MatAttribute.SetAttribute(MAT_LIGHT); // use lighting
	if(Material->blend_type==sMaterialObject::BLEND_ADD)
		MatAttribute.SetAttribute(MAT_ALPHA_ADDBLENDALPHA);
	if(Material->blend_type==sMaterialObject::BLEND_SUB)
		MatAttribute.SetAttribute(MAT_ALPHA_SUBBLEND);

	cMeshBank* pBank=pBanks->CreateUnical(Material->name.c_str(),ObjectAttribute,MatAttribute);

	pBank->GetAnimChannelMat()->NewChannel(pBanks);

	if(TextureName[0])
	{
		if(OpacityName[0]&&stricmp(TextureName,OpacityName))
			m3derror()<<ObjectMesh->name.c_str()<<"\r\n"<<TextureName<<" != "<<OpacityName<<VERR_END;
		cTexture* Texture=LoadTextureDef(TextureName,TexturePath,DefTexturePath);
		pBank->SetTexture(0,Texture);
	}

	if(ReflectionName[0])
	{
		if(TextureName[0]==0)
			m3derror()<<"Material has reflection texture and don't has diffuse texture "<<ReflectionName<<VERR_END;
		cTexture* Texture=LoadTextureDef(ReflectionName,TexturePath,DefTexturePath);
		pBank->SetTexture(1,Texture,MAT_RENDER_SPHEREMAP|MAT_TEXNORMAL_STAGE2);
	}else
	if(BumpName[0])
	{
		if(TextureName[0]==0)
			m3derror()<<"Material has bump texture and don't has diffuse texture "<<BumpName<<VERR_END;
		cTexture* Texture=LoadTextureDef(BumpName,TexturePath,DefTexturePath,"Bump");
		pBank->SetTexture(1,Texture);
	}
	
	return pBank;
}

int isAnimateMaterial(cMeshScene &MeshScene,const char *name,int nLOD=0)
{
	int col=0;
	for(int i=0;i<MeshScene.ChannelLibrary.length();i++)
	{
		sMaterialObject *ObjMat=MeshScene.ChannelLibrary[i]->LodLibrary[nLOD]->MaterialLibrary.Get(name);
		VISASSERT(ObjMat);
		if(ObjMat->AmbientAnim.length()>1) col|=1<<0;
		if(ObjMat->DiffuseAnim.length()>1) col|=1<<1;
		if(ObjMat->SpecularAnim.length()>1) col|=1<<2;
		if(ObjMat->EmissiveAnim.length()>1) col|=1<<3;
		if(ObjMat->TransparencyAnim.length()>1) col|=1<<4;
		if(ObjMat->ShininessAnim.length()>1) col|=1<<5;
		for(int j=0;j<ObjMat->SubTexMap.length();j++)
			if( ObjMat->SubTexMap[j]->ID==TEXMAP_DI && ObjMat->SubTexMap[j]->MatrixAnim.length()>1 )
				col|=1<<6;
	}
	return col;
}

inline void ReadMeshAnimMat(cMeshScene &MeshScene,int nChannel,int nLOD,cAnimChannelMaterial *AnimChannel,sObjectMesh *ObjectMesh)
{ // импорт материала объекта
	VISASSERT(ObjectMesh->AnimationMeshLibrary[0]);
	int nMaterial=ObjectMesh->AnimationMeshLibrary[0]->Face[0][3];
	sChannelAnimation *Channel=MeshScene.ChannelLibrary[nChannel];
	sLodObject *LodObject=Channel->LodLibrary[nLOD];
	if(nMaterial<0||nMaterial>=LodObject->MaterialLibrary.length()) return;
	sMaterialObject *Material=LodObject->MaterialLibrary[nMaterial];
	int isAnimMat=isAnimateMaterial(MeshScene,Material->name.c_str(),nLOD);
//	if(isAnimMat==0) return;

	int StartTime=Channel->FirstFrame*Channel->TicksPerFrame;
	cAnimChainMaterial *AnimChain=AnimChannel->GetChannel(nChannel);
	if(Material->AmbientAnim.length())
	{ // ambient
		AnimChain->KeyAmbient.resize(Material->AmbientAnim.length());
		for(int i=0;i<AnimChain->KeyAmbient.size();i++)
		{
			AnimChain->KeyAmbient[i].time=(float)(Material->AmbientAnim[i][0]-StartTime);
			AnimChain->KeyAmbient[i].color.set(Material->AmbientAnim[i][1],Material->AmbientAnim[i][2],Material->AmbientAnim[i][3],1);
		}
	}else
	{
		AnimChain->KeyAmbient.resize(1);
		AnimChain->KeyAmbient[0].time=0;
		AnimChain->KeyAmbient[0].color.set(0,0,0,1);
	}

	VISASSERT(Material->DiffuseAnim.length());
//	if(isAnimMat&(1<<1))
	{ // diffuse
		AnimChain->KeyDiffuse.resize(Material->DiffuseAnim.length());
		for(int i=0;i<AnimChain->KeyDiffuse.size();i++)
		{
			AnimChain->KeyDiffuse[i].time=(float)(Material->DiffuseAnim[i][0]-StartTime);
			AnimChain->KeyDiffuse[i].color.set(Material->DiffuseAnim[i][1],Material->DiffuseAnim[i][2],Material->DiffuseAnim[i][3],1);
		}
	}

	if(Material->SpecularAnim.length())
	{ // specular
		AnimChain->KeySpecular.resize(Material->SpecularAnim.length());
		for(int i=0;i<AnimChain->KeySpecular.size();i++)
		{
			AnimChain->KeySpecular[i].time=(float)(Material->SpecularAnim[i][0]-StartTime);
			AnimChain->KeySpecular[i].color.set(Material->SpecularAnim[i][1],Material->SpecularAnim[i][2],Material->SpecularAnim[i][3],1);
		}
	}else
	{
		AnimChain->KeySpecular.resize(1);
		AnimChain->KeySpecular[0].time=0;
		AnimChain->KeySpecular[0].color.set(0,0,0,1);
	}

	if(Material->EmissiveAnim.length())
	{ // specular
		AnimChain->KeyEmissive.resize(Material->EmissiveAnim.length());
		for(int i=0;i<AnimChain->KeyEmissive.size();i++)
		{
			AnimChain->KeyEmissive[i].time=(float)(Material->EmissiveAnim[i][0]-StartTime);
			AnimChain->KeyEmissive[i].color.set(Material->EmissiveAnim[i][1],Material->EmissiveAnim[i][2],Material->EmissiveAnim[i][3],1);
		}
	}else
	{
		AnimChain->KeyEmissive.resize(1);
		AnimChain->KeyEmissive[0].time=0;
		AnimChain->KeyEmissive[0].color.set(0,0,0,1);
	}

	if(Material->TransparencyAnim.length())
	{ // transparency
		AnimChain->KeyTransparency.resize(Material->TransparencyAnim.length());
		for(int i=0;i<AnimChain->KeyTransparency.size();i++)
		{
			AnimChain->KeyTransparency[i].time=(float)(Material->TransparencyAnim[i][0]-StartTime);
			AnimChain->KeyTransparency[i].a=1-Material->TransparencyAnim[i][1];
		}
	}else
	{
		AnimChain->KeyTransparency.resize(1);
		AnimChain->KeyTransparency[0].time=0;
		if(Material->TransparencyAnim.length())
			AnimChain->KeyTransparency[0].a=1-Material->TransparencyAnim[0][1];
		else
			AnimChain->KeyTransparency[0].a=1.0f;
	}

	VISASSERT(Material->ShininessAnim.length());
//	if(isAnimMat&(1<<5))
	{ // transparency
		AnimChain->KeyPower.resize(Material->ShininessAnim.length());
		for(int i=0;i<AnimChain->KeyPower.size();i++)
		{
			AnimChain->KeyPower[i].time=(float)(Material->ShininessAnim[i][0]-StartTime);
			AnimChain->KeyPower[i].a=100*Material->ShininessAnim[i][1];
		}
	}

	if(isAnimMat&(1<<6)) // texture mapping
	for(int j=0;j<Material->SubTexMap.length();j++)
	if(Material->SubTexMap[j]->ID==TEXMAP_DI)
	{
		AnimChain->KeyTexMatrix.resize(Material->SubTexMap[j]->MatrixAnim.length());
		for(int i=0;i<AnimChain->KeyTexMatrix.size();i++)
		{
			AniMatrix& in=Material->SubTexMap[j]->MatrixAnim[i];
			sKeyTexMatrix& out=AnimChain->KeyTexMatrix[i];
			out.time=(float)(in.time-StartTime);
			out.m=in.mat;
			RightToLeft(out.m);
		}
		break;
	}

	AnimChannel->EndBuild();
}
inline void ReadLightAnimMat(cMeshScene &MeshScene,int nChannel,cAnimChannelMaterial *AnimChannel,sLightObject *LightObject)
{ // импорт анимации материала источника света
	VISASSERT(LightObject->AnimationLightLibrary[0]);
	sChannelAnimation *Channel=MeshScene.ChannelLibrary[nChannel];
	cAnimChainMaterial *AnimChain=AnimChannel->GetChannel(nChannel);
	int StartTime=Channel->FirstFrame*Channel->TicksPerFrame;
	AnimChain->KeyDiffuse.resize(LightObject->AnimationLightLibrary.length());
	for(int i=0;i<AnimChain->KeyDiffuse.size();i++)
	{
		AnimChain->KeyDiffuse[i].time=(float)(LightObject->AnimationLightLibrary[i]->time-StartTime);
		float *Diffuse=LightObject->AnimationLightLibrary[i]->DiffuseColor;
		AnimChain->KeyDiffuse[i].color.set(Diffuse[0],Diffuse[1],Diffuse[2],1.f);
	}

	AnimChannel->EndBuild();
}

void ReadAnimNode(sChannelAnimation *Channel,sLodObject *LodObject,cAnimChannelNode *AnimChannel,sNodeObject *NodeObject,const char *parent)
{ // импорт анимации узла
	float StartTime=Channel->FirstFrame*Channel->TicksPerFrame,
		AnimTime=(Channel->LastFrame-Channel->FirstFrame)*Channel->TicksPerFrame;
	cAnimChainNode *AnimChain=AnimChannel->GetChannel(Channel->ID);
	AnimChain->GetTime()=AnimTime;
	sAnimationVisibility &AnimationVisibility=NodeObject->AnimationVisibility;
	AnimChannel->NewVisible(Channel->ID,AnimationVisibility.length());
	for(int i=0;i<AnimChain->GetNumberVisible();i++)
	{
		AnimChain->GetVisible(i).time=AnimationVisibility[i][0]-StartTime;
		AnimChain->GetVisible(i).visible=round(AnimationVisibility[i][1]);
	}

	if(NodeObject->AnimationPosition.length()>1 || 
		NodeObject->AnimationRotation.length()>1 || 
		NodeObject->AnimationScale.length()>1 || 
		NodeObject->AnimationScaleRotation.length()>1 
		)
	{
		AnimTime/=(Channel->NumberFrame-1);

		AnimChannel->NewMatrix(Channel->ID,Channel->NumberFrame);
		for(int nFrame=0;nFrame<Channel->NumberFrame;nFrame++)
		{
			MatXf Matrix;
			int TimeFrame=round(nFrame*AnimTime);
			GetMatrixObj(StartTime+TimeFrame,Matrix,LodObject,NodeObject);
			AnimChain->GetMatrix(nFrame).time=TimeFrame;
			AnimChain->GetMatrix(nFrame).mat=Matrix;
		}
	}
	else
	{
		AnimChannel->NewMatrix(Channel->ID,1);
		MatXf Matrix;
		GetMatrixObj(StartTime,Matrix,LodObject,NodeObject);
		AnimChain->GetMatrix(0).time=0;
		AnimChain->GetMatrix(0).mat=Matrix;
	}
	
}

void cAllMeshBank::SetFrame(cMeshScene *MeshScene)
{
	SetNumberChannel(MeshScene->ChannelLibrary.length());
	for(int i=0;i<GetNumberChannel();i++)
	{
		sChannelAnimation *Channel=MeshScene->ChannelLibrary[i];
		GetChannel(i).name=Channel->name;
		GetChannel(i).Time=(Channel->LastFrame-Channel->FirstFrame)*Channel->TicksPerFrame;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация cObjLibrary
//////////////////////////////////////////////////////////////////////////////////////////
cObjLibrary::cObjLibrary() : cUnknownClass(KIND_LIB_OBJECT)
{
}
cObjLibrary::~cObjLibrary()
{
	Free();
	VISASSERT(GetNumberObj()==0);
}

void cObjLibrary::FreeOne(FILE* f)
{
	bool close=false;
#ifdef TEXTURE_NOTFREE
	if(!f)
	{
		f=fopen("obj_notfree1.txt","at");
		close=true;
	}
#endif 
	if(f)fprintf(f,"Object freeing\n");

	//temp_freeone - количество объектов в cObjLibrary::objects
	//ссылающихся на cAnimChannelNode
	int compacted=0;
	OBJECTS::iterator it;
	FOR_EACH(objects,it)
	{
		(*it)->root->GetAnimChannel()->temp_freeone=0;
	}

	FOR_EACH(objects,it)
	{
		(*it)->root->GetAnimChannel()->temp_freeone++;
	}

	for(int i=0;i<2;i++)
	{
		FOR_EACH(objects,it)
		{
			cAllMeshBank*& p=*it;
			if(p)
			{
				int ref=p->root->GetAnimChannel()->GetRef();
				int temp=p->root->GetAnimChannel()->temp_freeone;
				if(ref<=temp)
				{
					p->Release();
					p=NULL;
					compacted++;
				}else
				{
					if(f && i)
						fprintf(f,"%s - %i\n",p->root->GetFileName(),ref);
				}
			}
		}
	}

	if(f)
	{
		fprintf(f,"Objects free %i, not free %i\n",compacted,objects.size()-compacted);
		fflush(f);
	}
}

void cObjLibrary::Compact(FILE* f)
{
	MTAuto mtlock(&lock);
	FreeOne(f);
	remove_null_element(objects);
}

void cObjLibrary::Free(FILE* f)
{
	MTAuto mtlock(&lock);
	FreeOne(f);
	objects.clear();
}

cObjectNodeRoot* cObjLibrary::GetElement(const char* pFileName,const char* pTexturePath)
{
	MTAuto mtlock(&lock);
	return GetElementInternal(pFileName,pTexturePath,true);
}

cObjectNodeRoot* cObjLibrary::GetElementInternal(const char* pFileName,const char* pTexturePath,bool enable_error_not_found)
{
	if(!pFileName)
	{
		VISASSERT(0);
		return NULL;
	}

	char fname[512]="",TexturePath[512]="";
	char DefPath[512];

	strcpy(fname,(char*)pFileName); 
	_strlwr(fname);

	char *DefTexturePath=NULL;
	strcpy(DefPath,fname);
	int i;
	for(i=strlen(DefPath)-1; i>=0 && DefPath[i]; i-- )
		if(DefPath[i]=='\\')
			break;
	DefPath[i+1]=0;
	strcat(DefPath,"TEXTURES\\");
	_strlwr(DefPath);


	if(pTexturePath) 
	{
		strcpy(TexturePath,(char*)pTexturePath);
		_strlwr(TexturePath);
		if(stricmp(TexturePath,DefPath)!=0)
			DefTexturePath=DefPath;
	}
	else
	{
		strcpy(TexturePath,DefPath);
	}

	cAllMeshBank* nearest_bank=NULL;
	for(i=0;i<GetNumberObj();i++)
	if(stricmp(GetObj(i)->GetFileName(),fname)==0)
	{
		cAllMeshBank* bank=GetObj(i);
		nearest_bank=bank;

		if(stricmp(bank->GetTexturePath(),TexturePath)==0)
		{
			cObjectNodeRoot* tmp=(cObjectNodeRoot*)bank->root->BuildCopy();
			return tmp;
		}
	}

	cAllMeshBank *ObjNode=NULL;
	cAllMeshBank *ObjNodeLod=NULL;
	if(nearest_bank)
	{
		ObjNode=nearest_bank->BuildCopyWithAnotherTexture(TexturePath,DefTexturePath);
		if(ObjNode->root->RootLod)
		{
			ObjNodeLod=nearest_bank->root->RootLod->GetRoot()->BuildCopyWithAnotherTexture(TexturePath,DefTexturePath);
			ObjNode->root->RootLod->Release();
			ObjNode->root->RootLod=ObjNodeLod->root;
			ObjNode->root->RootLod->IncRef();
		}
	}else
	{
		ObjNode=LoadM3D(fname,TexturePath,DefTexturePath,enable_error_not_found);
	}

	cObjectNodeRoot *tmp=NULL;
	if(ObjNode)
	{
		objects.push_back(ObjNode); 
		if(ObjNodeLod)
			objects.push_back(ObjNodeLod); 
		tmp=(cObjectNodeRoot*)ObjNode->root->BuildCopy(); 
	}
	return tmp;
}

static bool is_space(char c)
{
	return c==' ' || c==8;
}

static bool is_name_char(char c)
{
	return (c>='0' && c<='9') || 
		   (c>='a' && c<='z') ||
		   (c>='A' && c<='Z') || c=='_';
}

static void ParseNodeEffect(cObjectNode *CurrentNode)
{
	static char eff[]="effect:";
	const char* cur=CurrentNode->GetNameObj();
	const char* end;
	if(strncmp(cur,eff,sizeof(eff)-1)!=0)
		return;
	if(CurrentNode->GetAnimChannel()->effect_key)
		return;
	cur+=sizeof(eff)-1;
	while(is_space(*cur))
		cur++;
	end=cur;
	while(is_name_char(*end))
		end++;

	string file_name(cur,end-cur);
	EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary(file_name.c_str(),true);

	if(!lib)
	{
		m3derror()<<"Object: \""<<CurrentNode->GetNameObj()<<"\"\r\n"<<
					"Effect library path: "<<gb_VisGeneric->GetEffectPath()<<"\"\r\n"<<
					"Library not found: \""<<file_name.c_str()<<"\""<<VERR_END;
		return;
	}

	cur=end;
	while(is_space(*cur))
		cur++;

	end=cur;
	while(is_name_char(*end))
		end++;

	string effect_name(cur,end-cur);
	CurrentNode->GetAnimChannel()->effect_key=lib->Get(effect_name.c_str());
	if(!CurrentNode->GetAnimChannel()->effect_key)
	{
		m3derror()<<"Object: \""<<CurrentNode->GetNameObj()<<"\"\r\n"<<
					"Effect library path: "<<gb_VisGeneric->GetEffectPath()<<"\r\n"
					"Effect not found: \""<<effect_name.c_str()<<"\""<<VERR_END;
	}

}

cAllMeshBank* cObjLibrary::LoadM3D(char *fname,char *TexturePath,char *DefTexturePath,bool enable_error_not_found)
{
	m3derror.SetName(fname);
	int size=0;
	char *buf=0;

	if(ResourceFileRead(fname,buf,size))
	{
		if(enable_error_not_found)
			m3derror()<<"File not found "<<VERR_END;
		return 0;
	}
	
	cMeshScene MeshScene; // загрузка MeshScene сцены из файла
	cMeshFile f;

	if(f.OpenRead(buf,size)==MESHFILE_NOT_FOUND) return 0;
	if(f.ReadHeaderFile())
	{
		m3derror()<<"Cannot read file"<<VERR_END;
		return 0;
	}

	MeshScene.Read(f);
	f.Close();

	gb_RenderDevice3D->SetCurrentConvertDot3Mul(MeshScene.bump_scale);
	int nChannel;
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{
		char* name=(char*)MeshScene.ChannelLibrary[nChannel]->name.c_str();
		_strlwr(name);
	}
/*	// поиск и установка первым канала анимации с именем "main"
	int nChannelMain=-1;
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{
		char* name=(char*)MeshScene.ChannelLibrary[nChannel]->name.c_str();
		_strlwr(name);
		if(stricmp(name,"main")==0)
			nChannelMain=nChannel;
	}

	if(nChannelMain>=0) 
	{
		sChannelAnimation *tmp=MeshScene.ChannelLibrary[0];
		MeshScene.ChannelLibrary[0]=MeshScene.ChannelLibrary[nChannelMain];
		MeshScene.ChannelLibrary[nChannelMain]=tmp;
	}
*/
	cAllMeshBank* pAllMeshBank=new cAllMeshBank(fname,TexturePath);
	pAllMeshBank->SetFrame(&MeshScene);
	pAllMeshBank->BeginLoad();

	cObjectNodeRoot *BaseNode=NULL;

	{
		sNodeObject *FirstObject=MeshScene.ChannelLibrary[0]->LodLibrary[0]->NodeObjectLibrary[0];
		if(!(FirstObject->type==NODEOBJECT_HELPER && isGroupName(FirstObject->name.c_str())))
		{
			BaseNode=new cObjectNodeRoot;
			BaseNode->SetFileName(fname);
			BaseNode->SetRoot(pAllMeshBank);
			BaseNode->SetGroup(true);
			BaseNode->LocalMatrix=MatXf::ID;
			BaseNode->GlobalMatrix=MatXf::ID;
		}
	}

	// создание объекта по описаннию в сцене
	for(nChannel=0;nChannel<MeshScene.ChannelLibrary.length();nChannel++)
	{ // импорт канала анимации
		sChannelAnimation *Channel=MeshScene.ChannelLibrary[nChannel];
		Channel->ID=nChannel;
		int FirstTime=Channel->FirstFrame*Channel->TicksPerFrame;
		VISASSERT(Channel->LodLibrary.length()==1);
		int LevelDetail=0;
		sLodObject *LodObject=Channel->LodLibrary[LevelDetail];
		for(int nNodeObject=0;nNodeObject<LodObject->NodeObjectLibrary.length();nNodeObject++)
		{ // импорт корневого объекта
			sNodeObject *NodeObject=LodObject->NodeObjectLibrary[nNodeObject];
			if(!NodeObject->parent.empty() && stricmp(NodeObject->parent.c_str(),"scene root")==0)
				NodeObject->parent.clear();
			if( !NodeObject->name.empty() && (TestFirstName(NodeObject->name.c_str(),"Bip")))
				continue; // пропустить "Bip" из CharacterStudio

			cObjectNode* CurrentNode=NULL;
			if(NodeObject->type==NODEOBJECT_MESH && !NodeObject->name.empty() && stricmp(NodeObject->name.c_str(),"_base_")==0)
			{// импорт основы для постановки объектов
				if( BaseNode->GetBase()==0 )
				{
					sBound	*Base;
					Base=new sBound;
					ReadMeshBound(FirstTime,Base,
						(sObjectMesh*)NodeObject,LodObject,NodeObject->parent.c_str());
					BaseNode->SetBase(Base);
				}
				continue;
			}

			bool firstpass=false;

			if(BaseNode)
			{
				cObjectNode *ParentNode;
				if(!NodeObject->parent.empty())
					ParentNode=BaseNode->FindObject(NodeObject->parent.c_str());
				else 
					ParentNode=BaseNode;
				if( !ParentNode )
					m3derror()<<"Error: cObjLibrary::LoadM3D()\r\nObject \""<<NodeObject->name.c_str()<<"\" don't find parent \""<<NodeObject->parent.c_str()<<"\""<<VERR_END;
				
				CurrentNode=ParentNode->FindObject(NodeObject->name.c_str());
				if(CurrentNode==0) 
				{ 
					firstpass=true;
					switch(NodeObject->type)
					{
					case NODEOBJECT_MESH:
						CurrentNode=new cObjMesh;
						break;
					case NODEOBJECT_LIGHT:
						{
							sLightObject *LightObject=(sLightObject*)NodeObject;
							if(LightObject->idObject==IDOBJECT_FSPOT||
							   LightObject->idObject==IDOBJECT_OMNI)
								CurrentNode=new cObjLight;
						}
						break;
					case NODEOBJECT_HELPER:
						CurrentNode=new cObjectNode;
						break;
					default:
						continue;
					}
					if(!CurrentNode)
						continue;
					CurrentNode->SetRoot(pAllMeshBank);
					ParentNode->AttachChild(CurrentNode);
				}
			}else 
			{
				VISASSERT(NodeObject->type==NODEOBJECT_HELPER);
				CurrentNode=BaseNode=new cObjectNodeRoot;
				BaseNode->SetFileName(fname);
				CurrentNode->SetRoot(pAllMeshBank);
			}
			// импорт анимации объекта
			if(CurrentNode->GetAnimChannel()==0) 
			{
				CurrentNode->SetAnimChannel(new cAnimChannelNode);
				CurrentNode->GetAnimChannel()->NewChannel(MeshScene.ChannelLibrary.length());
				CurrentNode->GetAnimChannel()->ObjectName=NodeObject->name;
				CurrentNode->SetGroup(isGroupName(NodeObject->name.c_str()));
				GetMatrixObj(Channel->FirstFrame,CurrentNode->LocalMatrix,LodObject,NodeObject);
			}
			// импорт анимации узла
			ReadAnimNode(Channel,LodObject,CurrentNode->GetAnimChannel(),NodeObject,CurrentNode->GetParentNodeName());

			if(Option_EnableLinkEffectToModel)
				ParseNodeEffect(CurrentNode);
			if(CurrentNode->GetAnimChannel()->effect_key)
			{
				CurrentNode->NodeAttribute.SetAttribute(ATTRNODE_EFFECT);
				if(CurrentNode->GetAnimChannel()->effect_key->IsCycled())
					CurrentNode->NodeAttribute.SetAttribute(ATTRNODE_EFFECT_CYCLED);
			}

			if(NodeObject->type==NODEOBJECT_MESH)
			{ // импорт 3d-объекта
				VISASSERT(CurrentNode->GetKind()==KIND_OBJMESH);
				sObjectMesh *ObjectMesh=(sObjectMesh*)NodeObject;
				cObjMesh *Mesh=(cObjMesh*)CurrentNode;
				if(firstpass)
				{ // импорт статического объекта
					// импорт материала объекта
					cMeshBank* pBank=ReadMeshMat(pAllMeshBank,ObjectMesh,LodObject->MaterialLibrary,
						TexturePath,DefTexturePath,this);
					if(pBank==NULL)
					{
						m3derror()<<"Error: cObjLibrary::LoadM3D()\r\nFile "<<fname<<
							" object "<<NodeObject->name<<" not have material"<<VERR_END;
						return 0;
					}

					Mesh->SetBank(pBank,true);
					Mesh->SetAttribute(Mesh->GetBank()->GetObjectAttribute().GetAttribute());
					// импорт геометрии объекта
					ReadMeshTri(FirstTime,Mesh,ObjectMesh,pAllMeshBank);
				}

				// импорт анимации материала объекта
				ReadMeshAnimMat(MeshScene,nChannel,LevelDetail,Mesh->GetBank()->GetAnimChannelMat(),ObjectMesh);
			}
			else if(NodeObject->type==NODEOBJECT_LIGHT)
			{ // импорт объекта источника света
				VISASSERT(CurrentNode->GetKind()==KIND_LIGHT);
				// импорт объекта
				sLightObject *LightObject=(sLightObject*)NodeObject;
				cObjLight *Light=(cObjLight*)CurrentNode;
				if(firstpass)
				{ // импорт статического объекта
					// импорт текстуры объекта
					const char *TextureName=::GetFileName(LightObject->TexProj.c_str());
					cTexture* Texture=LoadTextureDef(TextureName,TexturePath,DefTexturePath);
					Light->SetTextureLight(Texture);
					// импорт положения объекта
					Light->GetRadius()=LightObject->AnimationLightLibrary[0]->FarBeginAttenuation;
					Light->GetFarAttenuation()=LightObject->AnimationLightLibrary[0]->FarFinishAttenuation;
					if(LightObject->UseGlobal)
						Light->SetAttribute(ATTRUNKOBJ_COLLISIONTRACE);
				}
				// импорт анимации объекта
				if(Light->GetAnimChannelMat()==0)
				{
					Light->SetAnimChannelMat(new cAnimChannelMaterial);
					Light->GetAnimChannelMat()->NewChannel(pAllMeshBank);
				}
				// импорт анимации узла
				ReadLightAnimMat(MeshScene,nChannel,Light->GetAnimChannelMat(),LightObject);
			}else
				VISASSERT(NodeObject->type==NODEOBJECT_MESH||NodeObject->type==NODEOBJECT_HELPER);
		}
	}

	pAllMeshBank->EndLoad();
	pAllMeshBank->root=BaseNode;
	
	BaseNode->SetPosition(MatXf::ID);
	BaseNode->Update();
	BaseNode->BuildChild();
	BaseNode->CalcMatrix();
	BaseNode->CalcBorder();
	BaseNode->CalcObj();
	BaseNode->RootLod=LoadLod(fname,TexturePath);
	BaseNode->BuildShadow();

	if(!BaseNode->GetAnimChannel())
	{
		BaseNode->SetAnimChannel(new cAnimChannelNode);
	}

	return pAllMeshBank;
}

cObjectNodeRoot* cObjLibrary::LoadLod(char *in_filename,char *TexturePath)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(in_filename,drive,dir,fname,ext);
	strcat(fname,"_lod");
	_makepath(path_buffer,drive,dir,fname,ext);
	return GetElementInternal(path_buffer,TexturePath,false);
}
