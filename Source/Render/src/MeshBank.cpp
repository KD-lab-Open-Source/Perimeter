#include "StdAfxRD.h"
#include "DrawBuffer.h"
#include "MeshBank.h"
#include "MeshTri.h"
#include "Scene.h"
#include "files/files.h"

struct Points
{
	std::vector<int> texel;//Текстурные координаты соответствующие этому пикселю
	Vect3f normal;
	int new_base_index;
};

inline void AddUnicalInt(std::vector<int>& v,int num)
{
	for(int i=0;i<v.size();i++)
		if(v[i]==num)
			return;
	v.push_back(num);
}

static int16_t FindNewPoint(Points& p,int16_t texel)
{
	VISASSERT(p.texel.size());

	for(int i=0;i<p.texel.size();i++)
	{
		if(p.texel[i]==texel)
			return p.new_base_index+i;
	}

	VISASSERT(0);
	return p.new_base_index;
}

cMeshStatic::cMeshStatic(const char* materialname) {
	MaterialName=materialname;
}

cMeshStatic::~cMeshStatic() {
    for (auto mesh : meshes) {
        delete mesh;
    }
}

void cMeshStatic::BeginBuildMesh()
{
    VISASSERT(vertexes.empty());
    VISASSERT(polygons.empty());
    vertexes.clear();
    polygons.clear();
}

cMeshTri* cMeshStatic::AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel)
{
	VISASSERT(Polygon.size()==TexPoly.size() || TexPoly.size()==0);
	DeleteSingularPolygon(Vertex,Polygon,TexPoly,Texel);

	VISASSERT(Polygon.size()==TexPoly.size() || TexPoly.size()==0);

	//Разбиваем вершины так, что-бы у каждой было по однму текселю
	std::vector<Points> pnt(Vertex.size());
	int n_polygon=Polygon.size();
	int i;
	for(i=0;i<Vertex.size();i++)
	{
		pnt[i].normal=Vect3f::ZERO;
		pnt[i].new_base_index=0;
	}

	for(i=0;i<TexPoly.size();i++)
	{
		sPolygon& p=Polygon[i];
		sPolygon& s=TexPoly[i];
		VISASSERT(p.p1<pnt.size());
		VISASSERT(p.p2<pnt.size());
		VISASSERT(p.p3<pnt.size());

		AddUnicalInt(pnt[p.p1].texel,s.p1);
		AddUnicalInt(pnt[p.p2].texel,s.p2);
		AddUnicalInt(pnt[p.p3].texel,s.p3);
	}

	//Считаем нормали
	for(i=0;i<Polygon.size();i++)
	{
		sPolygon &p=Polygon[i];
		Vect3f n;
		n.cross(Vertex[p.p3]-Vertex[p.p1],Vertex[p.p2]-Vertex[p.p1]);
		n.Normalize();
		pnt[p.p1].normal+=n;
		pnt[p.p2].normal+=n;
		pnt[p.p3].normal+=n;
	}
	for(i=0;i<Vertex.size();i++)
		pnt[i].normal.Normalize();

	int n_vertex=0;
	for(i=0;i<Vertex.size();i++)
	{
		n_vertex+=max(pnt[i].texel.size(),1);
	}

	std::vector<sVertexXYZNT1> new_vertex(n_vertex);
	std::vector<sPolygon> new_polygon(n_polygon);

	{
		int cur_vertex=0;
		for(i=0;i<pnt.size();i++)
		{
			Points& p=pnt[i];
			p.new_base_index=cur_vertex;
			if(p.texel.size())
			{
				for(int j=0;j<p.texel.size();j++,cur_vertex++)
				{
					sVertexXYZNT1& v=new_vertex[cur_vertex];
					v.pos=Vertex[i];
					*((Vect3f*)&v.n)=p.normal;
					
					Vect2f& t=Texel[p.texel[j]];
					v.uv[0]=t.x;
					v.uv[1]=t.y;
				}
			}else
			{
				sVertexXYZNT1& v=new_vertex[cur_vertex++];
				v.pos=Vertex[i];
				*((Vect3f*)&v.n)=p.normal;
				
				v.uv[0]=v.uv[1]=0;
			}
		}
	}

	if(TexPoly.size())
	{
		for(i=0;i<n_polygon;i++)
		{
			sPolygon& p=Polygon[i];
			sPolygon& t=TexPoly[i];

			new_polygon[i].p1=FindNewPoint(pnt[p.p1],t.p1);
			new_polygon[i].p2=FindNewPoint(pnt[p.p2],t.p2);
			new_polygon[i].p3=FindNewPoint(pnt[p.p3],t.p3);
		}
	}else
	{
		for(i=0;i<n_polygon;i++)
			new_polygon[i]=Polygon[i];
	}

	SortPolygon(&new_polygon.front(),n_polygon);

	//

	cMeshTri* pTri=new cMeshTri;
	meshes.push_back(pTri);

	pTri->OffsetPolygon=polygons.size();
	pTri->OffsetVertex=vertexes.size();
	pTri->NumPolygon=n_polygon;
	pTri->NumVertex=n_vertex;

	polygons.resize(pTri->OffsetPolygon+n_polygon);
    vertexes.resize(pTri->OffsetVertex+n_vertex);
    
	for (i=0;i<n_polygon;i++) {
		sPolygon& to=polygons[i+pTri->OffsetPolygon];
		to=new_polygon[i];
		to.p1+=pTri->OffsetVertex;
		to.p2+=pTri->OffsetVertex;
		to.p3+=pTri->OffsetVertex;
	}

	for (i=0;i<n_vertex;i++) {
        vertexes[pTri->OffsetVertex + i] = new_vertex[i];
    }
/*
#ifdef _DEBUG
	for(i=0;i<n_vertex;i++)
	{
		sVertexXYZNT1& v=temp->vertex[pTri->OffsetVertex+i];
		VISASSERT(v.u1()>=-100 && v.u1()<=100);
		VISASSERT(v.v1()>=-100 && v.v1()<=100);
	}
#endif //_DEBUG
*/
	return pTri;
}

void cMeshStatic::EndBuildMesh(bool bump)
{
    size_t n_vertex=vertexes.size();
    size_t n_polygon=polygons.size();
    size_t n_indices = n_polygon * sPolygon::PN;

    //Write vertex and indices into DrawBuffer
    int fmt = sVertexXYZNT1::fmt;
#ifdef PERIMETER_D3D9
    if (bump && gb_RenderDevice->GetRenderSelection() == DEVICE_D3D9) {
        fmt = sVertexDot3::fmt;
    }
#endif
    db.Create(n_vertex, false, n_indices, false, fmt, PT_TRIANGLES);
    db.set_as_active = false;

    void* pVertex = nullptr;
    indices_t* indices = nullptr;
    db.LockRaw(n_vertex, n_indices, pVertex, indices, true);
    //Since DB vertex may be sVertexXYZNT1 or bigger than sVertexXYZNT1 we need to go by VertexSize
    for (int i = 0; i < n_vertex; i++) {
        uint8_t* v = reinterpret_cast<uint8_t*>(pVertex) + i * db.vb.VertexSize;
        memcpy(v, &(vertexes[i]), sizeof(sVertexXYZNT1));
    }
    memcpy(indices, polygons.data(), n_indices * sizeof(indices_t));
    db.Unlock();

	for (cMeshTri* p : meshes) {
        p->db=&db;
        p->dbr.offset = p->OffsetPolygon * sPolygon::PN;
        p->dbr.len = p->NumPolygon * sPolygon::PN;
        p->VertexBuffer = &vertexes[p->OffsetVertex];
        p->PolygonBuffer = &polygons[p->OffsetPolygon];
		if (bump) {
            p->CalcBumpST();
        }
	}
}

void cMeshStatic::SortPolygon(sPolygon* polygon,int n_polygon)
{
	for(int i=1;i<n_polygon;i++)
	{
		int ioptimal=i;
		int koptimal=0;
		sPolygon& prev=polygon[i-1];
		for(int j=i;j<n_polygon;j++)
		{
			sPolygon& cur=polygon[j];
			int k=0;
			if(cur.p1==prev.p1)k++;
			if(cur.p2==prev.p1)k++;
			if(cur.p3==prev.p1)k++;
			if(cur.p2==prev.p2)k++;
			if(cur.p3==prev.p2)k++;
			if(cur.p3==prev.p3)k++;
			if(k>koptimal)
			{
				ioptimal=i;
				koptimal=k;
				if(k>1)
					break;
			}
		}

		if(ioptimal!=i)
		{
			sPolygon tmp=polygon[i];
			polygon[i]=polygon[ioptimal];
			polygon[ioptimal]=tmp;
		}
	}
}

void cMeshStatic::DeleteSingularPolygon(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon,
	std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel)
{
	for(int i=0;i<Polygon.size();i++)
	{ // удаление вырожденных треугольников
		sPolygon &p=Polygon[i];
		if(p.p1==p.p2||p.p1==p.p3||p.p2==p.p3)
		{
			Polygon.erase(Polygon.begin()+i);
			if(TexPoly.size())
				TexPoly.erase(TexPoly.begin()+i);
			i--;
		}
	}
}

/////////////////////////////cMeshBank//////////////////////////////
cMeshBank::cMeshBank(const char* materialname,cAllMeshBank* _parent)
{
	bank=new cMeshStatic(materialname);
	parent=_parent;
	
	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		Texture[i]=0;
}

cMeshBank::cMeshBank()
{
	bank=NULL;
	parent=NULL;
	
	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		Texture[i]=0;
}

cMeshBank::~cMeshBank()
{
	RELEASE(bank);

	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		if( Texture[i] )
		{
			Texture[i]->Release();
			Texture[i]=0;
		}
}


void cMeshBank::BeginBuildMesh()
{
	bank->BeginBuildMesh();
}

cMeshTri* cMeshBank::AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel)
{
	return bank->AddMesh(Vertex,Polygon,TexPoly,Texel);
}

cMeshTri* cMeshBank::AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<Vect3f> &Normal, std::vector<Vect2f> &Texel)
{
	return bank->AddMesh(Vertex,Polygon,Normal,Texel);
}

void cMeshBank::EndBuildMesh()
{
	bank->EndBuildMesh(IsBump());
}

bool cMeshBank::IsBump()
{
	if(!Texture[1])
		return false;
	return Texture[1]->GetAttribute(MAT_BUMP)?true:false;
}

void cMeshBank::SetTexture(int n,cTexture * pTexture,int attr)
{
	VISASSERT(n>=0 && n<NUMBER_OBJTEXTURE);

#ifdef PERIMETER_DEBUG_ASSERT
    if (pTexture) {
        std::string filetest = string_to_lower(pTexture->GetName());
        if (filetest.find("_bump.") != std::string::npos) {
            xassert(n == 1);
        }
    }
#endif
	bank->Material.ClearAttribute(attr);
	if(Texture[n]) { Texture[n]->Release(); Texture[n]=0; } 

	if(n==1)
		bank->Material.PutAttribute(MAT_BUMP,pTexture && pTexture->GetAttribute(MAT_BUMP));

	Texture[n]=pTexture;
	if(pTexture) bank->Material.SetAttribute(attr);
	if(n==0) 
	{ 
		if(Texture[0])
			bank->Material.SetAttribute(Texture[0]->GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST));
	}
}

void cMeshBank::GetMaterial(int nChannel,float phase,int attribute,
		const sColor4f& Ambient,const sColor4f& Diffuse,const sColor4f& Specular,
		cObjMaterial& Mat)
{
	bank->AnimChannelMat.GetChannel(nChannel)->GetMaterial(phase,Mat);

	Mat.Diffuse=Mat.Diffuse*Diffuse;
	Mat.Specular.interpolate3(Mat.Specular,Specular,Specular.a);

	Mat.Ambient.interpolate3(Mat.Ambient,Ambient,Ambient.a);
	Mat.Ambient.a=0;

	Mat.ClearAttribute(0xFFFFFFFF);
	Mat.SetAttribute(bank->Material.GetAttribute());
	if(attribute&ATTRUNKOBJ_NOLIGHT)
		Mat.ClearAttribute(MAT_LIGHT);
}

void cMeshBank::GetMaterial(int nChannel,float phase,double MaterialAnimTime,
					int attribute,
					const sColor4f& Ambient,const sColor4f& Diffuse,const sColor4f& Specular,
					sDataRenderMaterial& Data)
{
	
	cObjMaterial mat;
	GetMaterial(nChannel,phase,attribute,
				 Ambient,Diffuse,Specular,mat);

	GetMaterial(nChannel,phase,MaterialAnimTime,
							mat,
							Data);
}

void cMeshBank::GetMaterial(int nChannel,float phase,double MaterialAnimTime,
							cObjMaterial& Mat,
							sDataRenderMaterial& Data)
{

	if(bank->AnimChannelMat.IsAnimTexMatrix(nChannel)) 
	{ // анимация текстурного маппинга
		bank->AnimChannelMat.GetChannel(nChannel)->GetTexMatrix(phase,Data.TexMatrix);
		Mat.SetAttribute(MAT_TEXMATRIX_STAGE1);
	}else
	{
		Mat.ClearAttribute(MAT_TEXMATRIX_STAGE1);
		::Identity(Data.TexMatrix);
	}

	if(!Mat.GetAttribute(MAT_LIGHT))
	{ // расчет освещения объкта
		sColor4f zero(0,0,0,1);
		Data.Ambient=zero;
		Data.Diffuse=Mat.Diffuse;
		Data.Specular=zero;
		Data.Emissive=zero;
		Data.Power = 0;
	}else
	{
		Data.Ambient=Mat.Ambient;
		Data.Diffuse=Mat.Diffuse;
		Data.Specular=Mat.Specular;
		Data.Emissive=Mat.Emissive;
		Data.Power = Mat.Power;
	}

	Data.Tex[0]=Texture[0];
	Data.Tex[1]=Texture[1];
	Data.mat = Mat.GetAttribute();

	Data.Phase=phase;
	if(Texture[0] && Texture[0]->GetTimePerFrame())
	{
		double len=Texture[0]->GetNumberFrame()*Texture[0]->GetTimePerFrame();
		Data.MaterialAnimPhase=xm::fmod(MaterialAnimTime,len)/len;
	}else
	if(Texture[1] && Texture[1]->GetTimePerFrame())
	{
		double len=Texture[1]->GetNumberFrame()*Texture[1]->GetTimePerFrame();
		Data.MaterialAnimPhase=xm::fmod(MaterialAnimTime,len)/len;
	}else
		Data.MaterialAnimPhase=0;
}

cMeshBank* cMeshBank::BuildStaticCopy()
{
	cMeshBank* pnew=new cMeshBank(bank->MaterialName.c_str(),NULL);
	pnew->bank->Material=bank->Material;

	int i;
	for(i=0;i<NUMBER_OBJTEXTURE;i++)
	{
		if(Texture[i])Texture[i]->IncRef();
		pnew->Texture[i]=Texture[i];
	}

	pnew->bank->ObjectAttribute=bank->ObjectAttribute;
	pnew->bank->AnimChannelMat.NewChannel(parent);
	for(i=0;i<bank->AnimChannelMat.GetNumberChannel();i++)
	{
		cAnimChainMaterial* orig=bank->AnimChannelMat.GetChannel(i);
		cAnimChainMaterial* cnew=pnew->bank->AnimChannelMat.GetChannel(i);

		cnew->KeyAmbient.resize(1);
		cnew->KeyAmbient[0]=orig->KeyAmbient[0];
		
		cnew->KeyDiffuse.resize(1);
		cnew->KeyDiffuse[0]=orig->KeyDiffuse[0];

		cnew->KeySpecular.resize(1);
		cnew->KeySpecular[0]=orig->KeySpecular[0];

		cnew->KeyEmissive.resize(1);
		cnew->KeyEmissive[0]=orig->KeyEmissive[0];
		
		cnew->KeyTransparency.resize(1);
		cnew->KeyTransparency[0]=orig->KeyTransparency[0];

		cnew->KeyPower.resize(1);
		cnew->KeyPower[0]=orig->KeyPower[0];
	}

	return pnew;
}

cTexture* LoadTextureDef(const char* name,const char* path,const char* def_path,char* attr=NULL);

cTexture* TextureWithAnnoterPath(cTexture* pTexture,const char* annoter_path,const char* def_texture_path)
{
	if(!pTexture)
		return NULL;

    std::string filename = convert_path_native(pTexture->GetName());
    filename = std::filesystem::u8path(filename).filename().u8string();

	std::string attr;
    if (pTexture->GetAttribute(TEXTURE_BUMP)) {
        attr = "Bump";
    }
    if (pTexture->GetAttribute(TEXTURE_NORMAL)) {
        attr += " Normal";
    }
	return LoadTextureDef(filename.c_str(),annoter_path,def_texture_path,attr.data());
}


cMeshBank* cMeshBank::BuildCopyWithAnotherTexture(const char* texture_path,const char* def_texture_path,cAllMeshBank* parent_)
{
	cMeshBank* pnew=new cMeshBank;
	pnew->bank=bank;
	pnew->bank->IncRef();
	pnew->parent=parent_;

	for(int i=0;i<NUMBER_OBJTEXTURE;i++)
	{
		pnew->Texture[i]=TextureWithAnnoterPath(Texture[i],texture_path,def_texture_path);
	}
	return pnew;
}

//////////////////
cAllMeshBank::cAllMeshBank(const char* fname,const char* texture_path_)
{
	root=NULL;
	filename=fname;
	texture_path=texture_path_;
}

cAllMeshBank::~cAllMeshBank()
{
	Free();
}

void cAllMeshBank::Free()
{
	std::vector<cMeshBank*>::iterator it;
	FOR_EACH(meshes,it)
		delete *it;

	meshes.clear();

	RELEASE(root);
}

void cAllMeshBank::BeginLoad()
{
	Free();
}

void cAllMeshBank::EndLoad()
{
	std::vector<cMeshBank*>::iterator it;
	FOR_EACH(meshes,it)
		(*it)->EndBuildMesh();
}

cMeshBank* cAllMeshBank::FindUnical(const char* materialname,sAttribute ObjectAttribute,sAttribute MatAttribute)
{
	cMeshBank* mb=NULL;
	const int obj_mask=ATTRUNKOBJ_COLLISIONTRACE;
	const int mat_mask=MAT_LIGHT;

	std::vector<cMeshBank*>::iterator it;
	FOR_EACH(meshes,it)
	{
		cMeshBank* pBank=*it;
		if(stricmp(pBank->GetMaterialName(),materialname)==0)
		{
			VISASSERT(pBank->GetObjectAttribute().GetAttribute(obj_mask)==ObjectAttribute.GetAttribute(obj_mask));
			VISASSERT(pBank->GetMaterial()->GetAttribute(mat_mask)==MatAttribute.GetAttribute(mat_mask));
			mb=pBank;
			break;
		}
	}

	return mb;
}

cMeshBank* cAllMeshBank::CreateUnical(const char* materialname,sAttribute ObjectAttribute,sAttribute MatAttribute)
{
	cMeshBank* mb=FindUnical(materialname,ObjectAttribute,MatAttribute);

	if(mb==NULL)
	{
		mb=new cMeshBank(materialname,this);
		meshes.push_back(mb);
		mb->GetObjectAttribute().SetAttribute(ObjectAttribute.GetAttribute());
		mb->GetMaterial()->SetAttribute(MatAttribute.GetAttribute());
		mb->BeginBuildMesh();
	}
	return mb;
}

cAllMeshBank* cAllMeshBank::BuildCopyWithAnotherTexture(const char* texture_path,const char* def_texture_path_)
{
	cAllMeshBank* bank=new cAllMeshBank(filename.c_str(),texture_path);
	bank->def_texture_path=def_texture_path_?def_texture_path_:texture_path;
	bank->Channel=Channel;

	bank->meshes.resize(meshes.size());
	for(int i=0;i<meshes.size();i++)
	{
		bank->meshes[i]=meshes[i]->BuildCopyWithAnotherTexture(texture_path,def_texture_path_,this);
	}

	bank->root=(cObjectNodeRoot*)root->BuildCopy();
	bank->root->ChangeBank(bank);

	return bank;
}

cMeshTri* cMeshStatic::AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<Vect3f> &Normal, std::vector<Vect2f> &Texel)
{
	VISASSERT(Vertex.size()==Normal.size());
	VISASSERT(Texel.empty() || Vertex.size()==Texel.size());

	int n_vertex=Vertex.size();
	std::vector<sVertexXYZNT1> new_vertex(n_vertex);
	std::vector<sPolygon>& new_polygon=Polygon;
	int n_polygon=new_polygon.size();

	int i;

	if(!Texel.empty())
	{
		for(i=0;i<n_vertex;i++)
		{
			sVertexXYZNT1& v=new_vertex[i];
			v.pos=Vertex[i];
			v.n=Normal[i];
			Vect2f& t=Texel[i];
			v.uv[0]=t.x;
			v.uv[1]=t.y;
		}
	}else
	{
		for(i=0;i<n_vertex;i++)
		{
			sVertexXYZNT1& v=new_vertex[i];
			v.pos=Vertex[i];
			v.n=Normal[i];
			v.uv[0]=0;
			v.uv[1]=0;
		}
	}

	//

	cMeshTri* pTri=new cMeshTri;
	meshes.push_back(pTri);

	pTri->OffsetPolygon=polygons.size();
	pTri->OffsetVertex=vertexes.size();
	pTri->NumPolygon=n_polygon;
	pTri->NumVertex=n_vertex;

	polygons.resize(pTri->OffsetPolygon+n_polygon);
	vertexes.resize(pTri->OffsetVertex+n_vertex);
	for(i=0;i<n_polygon;i++) {
		sPolygon& to=polygons[i+pTri->OffsetPolygon];
		to=new_polygon[i];
		to.p1+=pTri->OffsetVertex;
		to.p2+=pTri->OffsetVertex;
		to.p3+=pTri->OffsetVertex;
	}

	for(i=0;i<n_vertex;i++) {
        vertexes[pTri->OffsetVertex + i] = new_vertex[i];
    }
/*
#ifdef _DEBUG
	for(i=0;i<n_vertex;i++)
	{
		sVertexXYZNT1& v=temp->vertex[pTri->OffsetVertex+i];
		VISASSERT(v.u1()>=-100 && v.u1()<=100);
		VISASSERT(v.v1()>=-100 && v.v1()<=100);
	}
#endif //_DEBUG
*/
	return pTri;
}
