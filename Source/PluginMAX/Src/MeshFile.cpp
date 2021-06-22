#include <cstring>
#include "SceneMesh.h"
#include "StreamBuffer.h"

/*
sBaseFieldError MeshFileError[]=
{
	{	MESHFILE_OK,				"no error"									},
	{	MESHFILE_NOT_FOUND,			"Error: MeshFile not found"					},
	{	MESHFILE_UNKNOWN_FORMAT,	"Error: MeshFile unknown fileformat"		},
	{	MESHFILE_ERROR_TYPE,		"Error: MeshFile unknown file type"			},
	{	MESHFILE_NOT_USE_FIELD,		"Warning: MeshFile this field not use"		},
	{	MESHFILE_ERROR,				"Error: MeshFile error file format"			}
};
*/

cMeshFile::cMeshFile()							
{ 
	f=0;
	type=0; 
	error=0; 
	ofs=0; 
	SizeID=SizeType=0;
	GlobalCurrentFieldType=0;
}

cMeshFile::~cMeshFile()						
{ 
	Close();
}
int cMeshFile::OpenRead(void *buf,int size)
{
	Close();
	f=new cStream;
	if(f->open(buf,size,STREAM_MEM|STREAM_IN)) return SetError(MESHFILE_NOT_FOUND);
	return MESHFILE_OK;
}
int cMeshFile::OpenWrite(int type)
{
	Close();
	cMeshFile::type=type;
	f=new cStream;
	if(f->open(1000000,STREAM_MEM|STREAM_OUT)) return SetError(MESHFILE_NOT_FOUND);
	f->seek(0);
	return MESHFILE_OK;
}
int	cMeshFile::Length()
{
	return f->length();
}
void* cMeshFile::GetBuffer()
{
	return f->str();
}
void cMeshFile::Close()
{
	if(f) delete f;f=0;
	type=0; 
	error=0; 
	ofs=0; 
}
int cMeshFile::eof()
{
	return f->eof();
}
void cMeshFile::WriteOfs()
{ 
	assert(type==MESHFILE_TYPE_TEXT); 
	for(int count=0;count<ofs;count++) 
		(*f)<<" "; 
}
int cMeshFile::ReadHeaderFile()
{
	char format[sizeof(MESHFILE_IDENTIFIER)+4];
	f->read(format,sizeof(MESHFILE_IDENTIFIER));
	f->read(&type,sizeof(type));
	f->read(&version,sizeof(version));
	if(stricmp(format,MESHFILE_IDENTIFIER)) return SetError(MESHFILE_UNKNOWN_FORMAT);
	if((type!=MESHFILE_TYPE_TEXT)&&(type!=MESHFILE_TYPE_BINARY)) SetError(MESHFILE_ERROR_TYPE);
	if(version<2)
		SizeID=sizeof(unsigned short),SizeType=sizeof(unsigned short);
	else if(version<=MESHFILE_MAXVERSION)
		SizeID=sizeof(unsigned char),SizeType=sizeof(unsigned char);
	if(version>MESHFILE_MAXVERSION) return SetError(MESHFILE_UNKNOWN_FORMAT);

	return MESHFILE_OK;
}
void cMeshFile::WriteHeaderFile(int version)
{
	cMeshFile::version=version;
	if((type!=MESHFILE_TYPE_TEXT)&&(type!=MESHFILE_TYPE_BINARY)) SetError(MESHFILE_ERROR_TYPE);
	f->write(MESHFILE_IDENTIFIER,sizeof(MESHFILE_IDENTIFIER));
	f->write(&type,sizeof(type));
	if(version==1)
		SizeID=sizeof(unsigned short),SizeType=sizeof(unsigned short);
	else if(version>=2)
		SizeID=sizeof(unsigned char),SizeType=sizeof(unsigned char);
	f->write(&version,sizeof(version));
	if(type==MESHFILE_TYPE_TEXT) (*f)<<"\n";
}

int cMeshFile::ReadFieldType()
{
	GlobalCurrentFieldType=0;
	if(type==MESHFILE_TYPE_BINARY) 
		f->read(&GlobalCurrentFieldType,SizeType);
	else if(type==MESHFILE_TYPE_TEXT) 
	{
		assert(0);
	}
	if(f->eof()) return -1;
	return GlobalCurrentFieldType;
}
int cMeshFile::ReadBlock()
{
	int BlockID=0;
	if(type==MESHFILE_TYPE_BINARY) 
		f->read(&BlockID,SizeID);
	else if(type==MESHFILE_TYPE_TEXT) 
	{
		assert(0);
	}
	return BlockID;
}
void cMeshFile::ReadField(void *buf,int size)
{
	int TypeField=GlobalCurrentFieldType;
	if(type==MESHFILE_TYPE_BINARY) 
	{
		if(GlobalFieldType[TypeField].var!=MF_TYPE_STRING)
			for(int i=0;i<size;i++)
			{
				f->read(&((char*)buf)[i*GlobalTypeVariable[GlobalFieldType[TypeField].var].size],GlobalTypeVariable[GlobalFieldType[TypeField].var].size);
			}
		else
		{
			assert(0);
			for(int i=0;i<size;i++)
			{
				char tmp[257],ch=0;
				while((!f->eof())&&(ch!='"'))
					f->read(&ch,1);
				if(ch!='"') return; else tmp[0]=0;
				int count=0;
				for(;(!f->eof())&&(tmp[count]!='"')&&(++count<256);)
					f->read(&tmp[count],1);
				tmp[count]=0;
				if(count>1)	
					((string*)buf)[i]=&tmp[1];
			}
		}
	}else if(type==MESHFILE_TYPE_TEXT) 
	{
		assert(0);
	}
}

void cMeshFile::ReadField(string& buf)
{
	buf.clear();
	int TypeField=GlobalCurrentFieldType;
	if(type==MESHFILE_TYPE_BINARY) 
	{
		if(GlobalFieldType[TypeField].var==MF_TYPE_STRING)
		{
			char tmp[257],ch=0;
			while((!f->eof())&&(ch!='"'))
				f->read(&ch,1);
			if(ch!='"') return; else tmp[0]=0;
			int count=0;
			for(;(!f->eof())&&(tmp[count]!='"')&&(++count<256);)
				f->read(&tmp[count],1);
			tmp[count]=0;
			if(count>1)	
				buf=&tmp[1];
		}else 
			assert(0);
	}else if(type==MESHFILE_TYPE_TEXT) 
	{
		if(GlobalFieldType[TypeField].var==MF_TYPE_STRING)
		{
			char tmp[257],ch=0;
			while((!f->eof())&&(ch!='"'))
				f->read(&ch,1);
			if(ch=='"') return; else tmp[0]=0;
			int count=0;
			for(;(!f->eof())&&(tmp[count]!='"')&&(++count<256);)
				f->read(&tmp[count],1);
			tmp[count]=0;
			buf=&tmp[1];
		}else
			assert(0);

	}
}
void cMeshFile::BeginWriteBlock(int BlockID)
{
	if(type==MESHFILE_TYPE_BINARY) 
	{
		f->write(&GlobalFieldType[MF_TYPE_BLOCK].type,SizeType);
		f->write(&GlobalBlockID[BlockID].type,SizeID);
	}
	else if(type==MESHFILE_TYPE_TEXT) 
	{
		WriteOfs();
		(*f)<<GlobalFieldType[MF_TYPE_BLOCK].str<<" "<<GlobalBlockID[BlockID].str<<"\n";
	}
	ofs+=MESHFILE_BLOCK_OFS;
}
void cMeshFile::EndWriteBlock(int BlockID)
{
	ofs-=MESHFILE_BLOCK_OFS;
	if(type==MESHFILE_TYPE_BINARY) 
	{
		f->write(&GlobalFieldType[MF_TYPE_BLOCK].type,SizeType);
		f->write(&GlobalBlockID[BlockID].type,SizeID);
	}
	else if(type==MESHFILE_TYPE_TEXT) 
	{
		WriteOfs();
		(*f)<<GlobalFieldType[MF_TYPE_BLOCK].str<<" "<<GlobalBlockID[BlockID].str<<"\n";
	}
}
void cMeshFile::WriteField(void *buf,int TypeField,int size)
{
	if(type==MESHFILE_TYPE_BINARY)
	{
		f->write(&GlobalFieldType[TypeField].type,SizeType);
		if(GlobalTypeVariable[GlobalFieldType[TypeField].var].size>=0)
			for(int i=0;i<size;i++)
				f->write(&((char*)buf)[GlobalTypeVariable[GlobalFieldType[TypeField].var].size*i],
						GlobalTypeVariable[GlobalFieldType[TypeField].var].size);
		else 
			for(int i=0;i<size;i++)
				if(!((string*)buf)[i].empty())
				{
					f->write("\"",1);
					string& s=((string*)buf)[i];
					f->write((void*)s.c_str(),s.size());
					f->write("\"",1);
				}
				else
					f->write("\"\"",2);
	}
	else if(type==MESHFILE_TYPE_TEXT) 
	{
		int i;
		WriteOfs();
		f->write(GlobalFieldType[TypeField].str,strlen(GlobalFieldType[TypeField].str));
		switch(GlobalFieldType[TypeField].var)
		{
			case MF_TYPE_CHAR:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((char*)buf)[i]<<"\n"; break;
			case MF_TYPE_UCHAR:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((unsigned char*)buf)[i]<<"\n"; break;
			case MF_TYPE_SHORT:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((short*)buf)[i]<<"\n"; break;
			case MF_TYPE_USHORT:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((unsigned short*)buf)[i]<<"\n"; break;
			case MF_TYPE_INT:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((int*)buf)[i]<<"\n"; break;
			case MF_TYPE_UINT:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((unsigned int*)buf)[i]<<"\n"; break;
			case MF_TYPE_FLOAT:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((float*)buf)[i]<<"\n"; break;
			case MF_TYPE_DOUBLE:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((double*)buf)[i]<<"\n"; break;
			case MF_TYPE_FLOAT2:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((float*)buf)[2*i+0]<<"\t"<<((float*)buf)[2*i+1]<<"\n"; break;
			case MF_TYPE_FLOAT3:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((float*)buf)[3*i+0]<<"\t"<<((float*)buf)[3*i+1]<<"\t"<<((float*)buf)[3*i+2]<<"\n"; break;
			case MF_TYPE_FLOAT4:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((float*)buf)[4*i+0]<<"\t"<<((float*)buf)[4*i+1]<<"\t"<<((float*)buf)[4*i+2]<<"\t"<<((float*)buf)[4*i+3]<<"\n"; break;
			case MF_TYPE_FLOAT5:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((float*)buf)[5*i+0]<<"\t"<<((float*)buf)[5*i+1]<<"\t"<<((float*)buf)[5*i+2]<<"\t"<<((float*)buf)[5*i+3]<<"\t"<<((float*)buf)[5*i+4]<<"\n"; break;
			case MF_TYPE_FLOAT12:
				for(i=0;i<size;i++)
				{
					(*f)<<"\t"<<((float*)buf)[12*i+0]<<"\t"<<((float*)buf)[12*i+1]<<"\t"<<((float*)buf)[12*i+2]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[12*i+3]<<"\t"<<((float*)buf)[12*i+4]<<"\t"<<((float*)buf)[12*i+5]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[12*i+6]<<"\t"<<((float*)buf)[12*i+7]<<"\t"<<((float*)buf)[12*i+8]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[12*i+9]<<"\t"<<((float*)buf)[12*i+10]<<"\t"<<((float*)buf)[12*i+11]<<"\n"; 
				}
				break;
			case MF_TYPE_FLOAT13:
				for(i=0;i<size;i++)
				{
					(*f)<<"\t"<<((float*)buf)[13*i+0]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[13*i+1]<<"\t"<<((float*)buf)[13*i+2]<<"\t"<<((float*)buf)[13*i+3]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[13*i+4]<<"\t"<<((float*)buf)[13*i+5]<<"\t"<<((float*)buf)[13*i+6]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[13*i+7]<<"\t"<<((float*)buf)[13*i+8]<<"\t"<<((float*)buf)[13*i+9]<<"\n"; 
					WriteOfs(); (*f)<<"\t"<<((float*)buf)[13*i+10]<<"\t"<<((float*)buf)[13*i+11]<<"\t"<<((float*)buf)[13*i+12]<<"\n"; 
				}
				break;
			case MF_TYPE_INT3:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((int*)buf)[3*i+0]<<"\t"<<((int*)buf)[3*i+1]<<"\t"<<((int*)buf)[3*i+2]<<"\n"; break;
			case MF_TYPE_INT4:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((int*)buf)[4*i+0]<<"\t"<<((int*)buf)[4*i+1]<<"\t"<<((int*)buf)[4*i+2]<<"\t"<<((int*)buf)[4*i+3]<<"\n"; break;
			case MF_TYPE_USHORT3:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((unsigned short*)buf)[3*i+0]<<"\t"<<((unsigned short*)buf)[3*i+1]<<"\t"<<((unsigned short*)buf)[3*i+2]<<"\n"; break;
			case MF_TYPE_USHORT4:
				for(i=0;i<size;i++)
					(*f)<<"\t"<<((unsigned short*)buf)[4*i+0]<<"\t"<<((unsigned short*)buf)[4*i+1]<<"\t"<<((unsigned short*)buf)[4*i+2]<<"\t"<<((unsigned short*)buf)[4*i+3]<<"\n"; break;
			case MF_TYPE_STRING:
				for(i=0;i<size;i++)
				{
					string& s=((string*)buf)[i];
					if(!s.empty()) (*f)<<" \""<<s.c_str()<<"\"\n";
					else (*f)<<" \""<<"none"<<"\"\n";
				}
				break;
		}
	}
}
