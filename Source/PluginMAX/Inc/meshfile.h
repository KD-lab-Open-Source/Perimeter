#ifndef __MESHFILE_H__
#define __MESHFILE_H__

//#include <my_STL.h>
#include <string>
using namespace std;

#include "..\util\xmath.h"
#include "..\src\BaseClass.h"

#include "DefMeshFile.h"

//meshfile header
#define MESHFILE_IDENTIFIER				"m3d"
#define MESHFILE_TYPE_BINARY			'b'
#define MESHFILE_TYPE_TEXT				't'
#define MESHFILE_MAXVERSION				0x00000003

//meshfile error
#define MESHFILE_OK						0x00000000
#define MESHFILE_NOT_FOUND				0x00000001
#define MESHFILE_UNKNOWN_FORMAT			0x00000002
#define MESHFILE_ERROR_TYPE				0x00000003
#define MESHFILE_NOT_USE_FIELD			0x00000004
#define MESHFILE_ERROR					0x00000005

#define MESHFILE_BLOCK_OFS				3

class cStream;

class cMeshFile
{
public:
	cStream	*f;
	char	type;
	int		error;
	int		ofs;						// смещение в текстовом формате
	int		version;

	cMeshFile();
	~cMeshFile();
	int		OpenRead(void *buf,int size);
	int		OpenWrite(int type);
	void*	GetBuffer();
	int		Length();
	int		SetError(int err)			{ if(err) Close(); return error=err; }
	void	Close();

	int		ReadHeaderFile();
	void	WriteHeaderFile(int version=MESHFILE_MAXVERSION);

	int		ReadBlock();
	int		ReadFieldType();
	void	ReadField(void *buf,int size=1);
	
	void	ReadField(string& buf);
	void	WriteField(void *buf,int TypeField,int size=1);
	void	BeginWriteBlock(int BlockID);
	void	EndWriteBlock(int BlockID);

	int	eof();
private:
	void WriteOfs();
	int		SizeID;
	int		SizeType;
	int GlobalCurrentFieldType;
};

template <class cBaseMesh,const int SIZE> 
struct sBaseMeshArrayType
{
	cBaseMesh Base[SIZE];
	inline cBaseMesh& operator [] (int number) { return Base[number]; }
	~sBaseMeshArrayType()
	{
		Base[0]=0;
	}
};

struct AniMatrix
{
	float time;
	MatXf mat;
};

typedef sBaseMeshArrayType <int,3> sInt3;
typedef sBaseMeshArrayType <int,4> sInt4;
typedef sBaseMeshArrayType <float,2> sFloat2;
typedef sBaseMeshArrayType <float,4> sFloat4;
typedef sBaseMeshArrayType <float,5> sFloat5;
typedef sBaseMeshArrayType <unsigned short,3> sUShort3;
typedef sBaseMeshArrayType <unsigned short,4> sUShort4;

template <class cBase,const int MF_ID_BLOCK_BASEMESH,const int MF_TYPE_BASEMESH> 
class cBaseFileMeshArray: public cBaseDynArray <cBase>
{
public:
	cBaseFileMeshArray()					{ }
	~cBaseFileMeshArray()					{ Release(); }
	void Release()							{ Delete(); }
	void New(int NewSize)					{ cBaseDynArray<cBase>::New(NewSize); }
	int Read(cMeshFile &f)					
	{
		int CurrentSize=0,NewSize=0;
		while(!f.eof())
			switch(f.ReadFieldType())
			{
				case MF_TYPE_NUMBER:
					f.ReadField(&NewSize);
					New(NewSize);
					break;
				case MF_TYPE_BASEMESH:
					assert(CurrentSize<length());
					f.ReadField(Base,length());
					break;
				case MF_TYPE_BLOCK:
					switch(f.ReadBlock())
					{
						case MF_ID_BLOCK_BASEMESH:
							return MESHFILE_OK;
						default:
							assert(0);
							break;
					}
					break;
				default:
					assert(0);
					break;
			}
		return MESHFILE_ERROR;
	}
	int Write(cMeshFile &f)
	{ 
		if(length()<=0) return MESHFILE_OK;
		f.BeginWriteBlock(MF_ID_BLOCK_BASEMESH);
		f.WriteField(&length(),MF_TYPE_NUMBER);
		f.WriteField(Base,MF_TYPE_BASEMESH,length());
		f.EndWriteBlock(MF_ID_BLOCK_BASEMESH);
		return MESHFILE_OK;
	}
};

template <class cBaseMesh> 
class cBaseMeshPointerLibrary: public cBaseDynArrayPointer <cBaseMesh>
{
public:
	cBaseMeshPointerLibrary()								{ }
	~cBaseMeshPointerLibrary()								{ Release(); }
	void Release()											{ Delete(); }
	cBaseMesh* New(cBaseMesh *BaseMesh)
	{
		if(BaseMesh==0) BaseMesh=new cBaseMesh;
		else for(int i=0;i<length();i++) if(BaseMesh==Base[i]) { BaseMesh->Release(); return BaseMesh; }
		Resize(length()+1);
		Base[length()-1]=BaseMesh;
		BaseMesh->ID=length()-1;
		return BaseMesh;
	}

	cBaseMesh* GetByID(unsigned int ID)
	{
		for(int i=0;i<length();i++)
			if(Base[i]->ID==ID)
				return Base[i];
		return 0;
	}

	cBaseMesh* Get(void* Key)
	{
		for(int i=0;i<length();i++)
			if(Base[i]->cmp(Key)==0)
				return Base[i];
		return 0;
	}
	cBaseMesh* Get(const char* Key)
	{
		return Get((void*)Key);
	}
	inline cBaseMesh* Append()								
	{ 
		return cBaseMeshPointerLibrary::New(new cBaseMesh); 
	}
	int Write(cMeshFile &f)
	{
		for(int i=0;i<length();i++)
			if(Base[i]->Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
		return MESHFILE_OK;
	}
	inline cBaseMeshPointerLibrary& operator = (const cBaseMeshPointerLibrary &Array)
	{
		*(cBaseDynArrayPointer <cBaseMesh>*)this=*(cBaseDynArrayPointer <cBaseMesh>*)&Array;
		((cBaseDynArrayPointer <cBaseMesh>*)&Array)->Clear();
		return *this;
	}
};

#endif //__MESHFILE_H__
