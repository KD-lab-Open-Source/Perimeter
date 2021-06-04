#ifndef __SCENEMESH_H__
#define __SCENEMESH_H__

#include <my_STL.h>
#include <string>

#ifdef X_AXIS
#undef X_AXIS
#endif
#ifdef Y_AXIS
#undef Y_AXIS
#endif
#ifdef Z_AXIS
#undef Z_AXIS
#endif

#include <xmath.h>
#include "inc\meshfile.h"

enum eNodeObjectType
{
	NODEOBJECT_NULL				=	0,
	NODEOBJECT_MESH				=	1,
	NODEOBJECT_CAMERA			=	2,
	NODEOBJECT_LIGHT			=	3,
	NODEOBJECT_SHAPE			=	4,
	NODEOBJECT_HELPER			=	5,
};

enum eTextureMapType
{ /* texture map in 3dSMAX */
	TEXMAP_AM					=	0,   // ambient
	TEXMAP_DI					=	1,   // diffuse
	TEXMAP_SP					=	2,   // specular
	TEXMAP_SH					=	3,   // shininess
	TEXMAP_SS					=	4,   // shininess strength
	TEXMAP_SI					=	5,   // self-illumination
	TEXMAP_OP					=	6,   // opacity
	TEXMAP_FI					=	7,   // filter color
	TEXMAP_BU					=	8,   // bump 
	TEXMAP_RL					=	9,   // reflection
	TEXMAP_RR					=	10,  // refraction 
	TEXMAP_DP					=	11,  // displacement 
};

enum eIDObject
{
	IDOBJECT_UNKNOWN			=	0,
	IDOBJECT_DUMMY				=	1,
	IDOBJECT_GROUP				=	2,
	IDOBJECT_OMNI				=	3,
	IDOBJECT_TSPOT				=	4,
	IDOBJECT_DIR				=	5,
	IDOBJECT_FSPOT				=	6,
	IDOBJECT_TDIR				=	7,
	IDOBJECT_CAMERAFREE			=	8,
	IDOBJECT_CAMERATARGET		=	9,
	IDOBJECT_CAMERAPARALLEL		=	10,
};

#define HELPERNAME_DUMMY			"Dummy"
#define HELPERNAME_GROUP			"Group"

typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONPOSITION,MF_TYPE_KEYPOSITION> sAnimationPosition;
typedef cBaseFileMeshArray <sFloat5,MF_ID_BLOCK_ANIMATIONROTATION,MF_TYPE_KEYROTATION> sAnimationRotation;
typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONSCALE,MF_TYPE_KEYSCALE> sAnimationScale;
typedef cBaseFileMeshArray <sFloat2,MF_ID_BLOCK_ANIMATIONVISIBILITY,MF_TYPE_KEYVISIBILITY> sAnimationVisibility;
typedef cBaseFileMeshArray <sFloat5,MF_ID_BLOCK_ANIMATIONSCALEROTATION,MF_TYPE_KEYSCALEROTATION> sAnimationScaleRotation;

typedef cBaseFileMeshArray <Vect3f,MF_ID_BLOCK_VERTEXMESH,MF_TYPE_VERTEXMESH> sVertexMesh;
typedef cBaseFileMeshArray <sUShort4,MF_ID_BLOCK_FACEMESH,MF_TYPE_FACEMESH> sFaceMesh;
typedef cBaseFileMeshArray <Vect3f,MF_ID_BLOCK_VERTEXNORMALMESH,MF_TYPE_VERTEXMESH> sVertexNormalMesh;
typedef cBaseFileMeshArray <Vect3f,MF_ID_BLOCK_FACENORMALMESH,MF_TYPE_VERTEXMESH> sFaceNormalMesh;

typedef cBaseFileMeshArray <sFloat2,MF_ID_BLOCK_TEXVERTEXMESH,MF_TYPE_TEXVERTEXMESH> sTexVertexMesh;
typedef cBaseFileMeshArray <sUShort3,MF_ID_BLOCK_TEXFACEMESH,MF_TYPE_TEXFACEMESH> sTexFaceMesh;

// описатели старых версий
typedef cBaseFileMeshArray <sInt4,MF_ID_BLOCK_FACEMESH,MF_TYPE_FACEMESH_VER1> sFaceMesh_Ver1;
typedef cBaseFileMeshArray <sInt3,MF_ID_BLOCK_TEXFACEMESH,MF_TYPE_TEXFACEMESH_VER1> sTexFaceMesh_Ver1;
typedef cBaseFileMeshArray <Vect3f,MF_ID_BLOCK_TEXVERTEXMESH,MF_TYPE_TEXVERTEXMESH_VER1> sTexVertexMesh_Ver1;

typedef cBaseFileMeshArray <AniMatrix,MF_ID_BLOCK_ANIMATION_MATRIX,MF_TYPE_MATRIXANIM> sAnimationMatrix;

struct sChannelMapping
{
	unsigned int		ID;
	unsigned int		ChannelNumber;			// 0-ColorVertex, 1-TexelVertex, 2..-Add TexelVertex
	sTexVertexMesh		TexVertex;				// массив текстурных uvw-координат вершин
	sTexFaceMesh		TexFace;				// массив текстурных вершин полигонов

	sChannelMapping()						{ ID=0; }
	~sChannelMapping()						{ Release(); }
	void Release()							
	{ 
		ID=0; 
		TexVertex.Release(); 
		TexFace.Release(); 
	}
	inline int cmp(void *Key)				{ return (*(unsigned int*)Key)!=ID; }
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};
typedef cBaseMeshPointerLibrary <sChannelMapping> sChannelMappingLibrary;

struct sAnimationMesh
{
	unsigned int			ID;
	int						time;			// время
	sVertexMesh				Vertex;			// массив xyz-координат вершин
	sFaceMesh				Face;			// массив вершин полигонов
	sVertexNormalMesh		VertexNormal;	// нормали для каждой вершиины полигона
	sFaceNormalMesh			FaceNormal;		// нормали для каждого полигона
	sChannelMappingLibrary	ChannelMappingLibrary;
	float					Box[6];		// характерный размер

	sAnimationMesh()						{ ID=0; time=0; }
	~sAnimationMesh()						{ Release(); }
	void Release()							
	{ 
		ID=0; 
		time=0; 
		Vertex.Release(); 
		Face.Release(); 
		VertexNormal.Release(); 
		FaceNormal.Release(); 
		ChannelMappingLibrary.Release(); 
	}
	inline int cmp(void *Key)				{ return (*(int*)Key)!=time; }
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};
typedef cBaseMeshPointerLibrary <sAnimationMesh> cAnimationMeshLibrary;

struct sNodeObject
{
	unsigned int			ID;
	unsigned int			type;
	string					name;
	string					parent;
	sAnimationVisibility	AnimationVisibility;
	sAnimationPosition		AnimationPosition;
	sAnimationRotation		AnimationRotation;
	sAnimationScale			AnimationScale;
	sAnimationScaleRotation	AnimationScaleRotation;
	float					matrix[12];

	sNodeObject(int Type=NODEOBJECT_NULL)	{ ID=0; type=Type; memset(matrix,0,12*sizeof(float)); }
	virtual ~sNodeObject()					{ Release(); }
	void Release();
	inline int cmp(void *Key)				{ return stricmp((char*)Key,name.c_str()); }
	virtual int Write(cMeshFile &f)			{ return 1; }
};
typedef cBaseMeshPointerLibrary <sNodeObject> cNodeObjectLibrary;

struct sObjectMesh : public sNodeObject
{
	cAnimationMeshLibrary	AnimationMeshLibrary;

	sObjectMesh() : sNodeObject(NODEOBJECT_MESH)	{ }
	~sObjectMesh()									{ Release(); }
	void Release()									{ AnimationMeshLibrary.Release(); }
	int Read(cMeshFile &f);
	virtual int Write(cMeshFile &f);
};

struct sHelperObject : public sNodeObject
{
	int					idObject;

	sHelperObject() : sNodeObject(NODEOBJECT_HELPER){ idObject=0; }
	~sHelperObject()								{ Release(); }
	void Release()									{ idObject=0; }
	int Read(cMeshFile &f);
	virtual int Write(cMeshFile &f);
	inline int isGroup()							{ return idObject==IDOBJECT_GROUP; }
	inline int isDummy()							{ return idObject==IDOBJECT_DUMMY; }
};

struct sAnimationLight
{
	unsigned int			ID;
	int						time;					// время
	float					DiffuseColor[3];		// цвет источника
	float					TDist;
	float					FarBeginAttenuation;
	float					FarFinishAttenuation;

	sAnimationLight()						{ ID=0; DiffuseColor[0]=DiffuseColor[1]=DiffuseColor[2]=TDist=FarBeginAttenuation=FarFinishAttenuation=0; }
	~sAnimationLight()						{ Release(); }
	void Release()							
	{ 
		ID=0; 
		time=0; 
		DiffuseColor[0]=DiffuseColor[1]=DiffuseColor[2]=TDist=FarBeginAttenuation=FarFinishAttenuation=0;
	}
	inline int cmp(void *Key)				{ return (*(int*)Key)!=time; }
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};
typedef cBaseMeshPointerLibrary <sAnimationLight> cAnimationLightLibrary;

struct sLightObject : public sNodeObject
{
	int						idObject;
	int						UseGlobal;
	string					TexProj;							// имя текстуры 
	float					OfsMatrix[12];
	cAnimationLightLibrary	AnimationLightLibrary;

	sLightObject() : sNodeObject(NODEOBJECT_LIGHT)	{ idObject=0; OfsMatrix[0]=OfsMatrix[4]=OfsMatrix[8]=1; OfsMatrix[1]=OfsMatrix[2]=OfsMatrix[3]=OfsMatrix[5]=OfsMatrix[6]=OfsMatrix[7]=OfsMatrix[9]=OfsMatrix[10]=OfsMatrix[11]=0; UseGlobal=0; }
	~sLightObject()									{ Release(); }
	void Release()									{ idObject=0; TexProj.clear(); AnimationLightLibrary.Release(); OfsMatrix[0]=OfsMatrix[4]=OfsMatrix[8]=1; OfsMatrix[1]=OfsMatrix[2]=OfsMatrix[3]=OfsMatrix[5]=OfsMatrix[6]=OfsMatrix[7]=OfsMatrix[9]=OfsMatrix[10]=OfsMatrix[11]=0; UseGlobal=0; }
	int Read(cMeshFile &f);
	virtual int Write(cMeshFile &f);
};

struct sCameraObject : public sNodeObject
{
	int						idObject;
	string					TexProj;							// имя текстуры 

	sCameraObject() : sNodeObject(NODEOBJECT_CAMERA){ idObject=0; }
	~sCameraObject()								{ Release(); }
	void Release()									{ idObject=0; }
	int Read(cMeshFile &f);
	virtual int Write(cMeshFile &f);
};

struct sSubTexmap
{
	unsigned int		ID;							// идентификатор текстуры 
	string				name;						// имя текстуры 
	int					ChannelNumber;				// номер канала текстурной генерации
	sAnimationMatrix	MatrixAnim;
	sSubTexmap()									{ Release(); }
	~sSubTexmap()									{ Release(); }
	void Release();
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
	inline int cmp(void *Key)				{ return stricmp((char*)Key,name.c_str()); }
};
typedef cBaseMeshPointerLibrary <sSubTexmap> cSubTexmapArray;

typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONAMBIENT,MF_TYPE_AMBIENTANIM> sAnimationAmbient;
typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONDIFFUSE,MF_TYPE_DIFFUSEANIM> sAnimationDiffuse;
typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONSPECULAR,MF_TYPE_SPECULARANIM> sAnimationSpecular;
typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONEMISSIVE,MF_TYPE_EMISSIVEANIM> sAnimationEmissive;
typedef cBaseFileMeshArray <sFloat4,MF_ID_BLOCK_ANIMATIONAMBIENT,MF_TYPE_AMBIENTANIMERR> sAnimationAmbientErr;
typedef cBaseFileMeshArray <sFloat2,MF_ID_BLOCK_ANIMATION_FLOAT_VAR1,MF_TYPE_FLOAT_VAR1_ANIM> sAnimationShininess;
typedef cBaseFileMeshArray <sFloat2,MF_ID_BLOCK_ANIMATION_FLOAT_VAR2,MF_TYPE_FLOAT_VAR2_ANIM> sAnimationShinStrength;
typedef cBaseFileMeshArray <sFloat2,MF_ID_BLOCK_ANIMATION_FLOAT_VAR3,MF_TYPE_FLOAT_VAR3_ANIM> sAnimationTransparency;

struct sMaterialObject
{
public:
	unsigned int				ID;					// порядковый номер в библиотеке
	string						name;				// имя материала
	string						parent;				// родитель, тот который содержит этот материал (MultiSubObjectMaterial)
	int							NumberSubObject;	// число материалов в данном, которые будут ссылаться через parent на данный
	int							Shading;			// тип материала
	int							SelfIllumColorOn;
	int							Falloff;		// 1: out, 0: in

	enum BLEND_TYPE
	{
		BLEND_NORMAL=0,
		BLEND_ADD=1,
		BLEND_SUB=2
	};
	int							blend_type;

	sAnimationAmbient			AmbientAnim;
	sAnimationDiffuse			DiffuseAnim;
	sAnimationSpecular			SpecularAnim;
	sAnimationEmissive			EmissiveAnim;
	sAnimationAmbientErr		AmbientAnimErr;
	cSubTexmapArray				SubTexMap;
	sAnimationShininess			ShininessAnim;
	sAnimationShinStrength		ShinStrengthAnim;
	sAnimationTransparency		TransparencyAnim;
	
	sMaterialObject()
	{
		ID=NumberSubObject=Shading=SelfIllumColorOn=Falloff=0;
		blend_type=BLEND_NORMAL;
	}
	~sMaterialObject()						{ Release(); }
	void Release()							
	{ 
		ID=NumberSubObject=Shading=SelfIllumColorOn=Falloff=0;
		blend_type=BLEND_NORMAL;
		name.clear(); 
		parent.clear(); 
		AmbientAnim.Release(); 
		DiffuseAnim.Release(); 
		SpecularAnim.Release();
		EmissiveAnim.Release();
		AmbientAnimErr.Release(); 
		SubTexMap.Release(); 
		ShininessAnim.Release(); 
		ShinStrengthAnim.Release(); 
		TransparencyAnim.Release();
	}
	inline int cmp(void *Key)				{ return stricmp((char*)Key,name.c_str()); }
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
	int ReadOld(cMeshFile &f);
};
typedef cBaseMeshPointerLibrary <sMaterialObject> cMaterialObjectLibrary;

struct sLodObject
{
	unsigned int				ID;		
	unsigned int				LevelDetail;	//level of detail
	cNodeObjectLibrary			NodeObjectLibrary;
	cMaterialObjectLibrary		MaterialLibrary;
	
	sLodObject()							{ ID=0; LevelDetail=0; }
	~sLodObject()							{ Release(); }
	void Release()							{ LevelDetail=0; 
		NodeObjectLibrary.Release(); 
		MaterialLibrary.Release(); 
	}
	inline int cmp(void *Key)				{ return (*(unsigned int*)Key)!=LevelDetail; }

	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};
typedef cBaseMeshPointerLibrary <sLodObject> cLodObjectLibrary;

struct sChannelAnimation
{
	unsigned int				ID;
	string						name;
	int							FirstFrame;
	int							LastFrame;
	int							FrameSpeed;
	int							TicksPerFrame;
	int							NumberFrame;	// временная переменная только для *.m3d с чтением морфинга
	cLodObjectLibrary			LodLibrary;

	sChannelAnimation()						{ ID=1; TicksPerFrame=FrameSpeed=LastFrame=FirstFrame=NumberFrame=0; }
	~sChannelAnimation()					{ Release(); }
	void Release()							{ name.clear(); LodLibrary.Release(); }
	inline int cmp(void *Key)				{ return stricmp((char*)Key,name.c_str()); }
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
	int GetNumberMeshAnimation();
};
typedef cBaseMeshPointerLibrary <sChannelAnimation> cChannelAnimationLibrary;

class cMeshScene
{
public:
	unsigned int				ID;
	string						name;
	float						bump_scale;
	cChannelAnimationLibrary	ChannelLibrary;
	
	cMeshScene(){ID=1;bump_scale=1.0f;}
	~cMeshScene()							{ Release(); }
	void Release()							{ name.clear(); ChannelLibrary.Release(); }
	
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};

#endif // __SCENEMESH_H__