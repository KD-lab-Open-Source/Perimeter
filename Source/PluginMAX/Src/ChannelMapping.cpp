#include "SceneMesh.h"

int sChannelMapping::Read(cMeshFile &f)
{
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_NUMBER:
				f.ReadField(&ChannelNumber);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_CHANNELMAPPING:
						return MESHFILE_OK;
					case MF_ID_BLOCK_TEXVERTEXMESH:
						TexVertex.Read(f);
						break;
					case MF_ID_BLOCK_TEXFACEMESH:
						TexFace.Read(f);
						break;
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
int sChannelMapping::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_CHANNELMAPPING);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&ChannelNumber,MF_TYPE_NUMBER);
	if(TexVertex.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(TexFace.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	f.EndWriteBlock(MF_ID_BLOCK_CHANNELMAPPING);
	return MESHFILE_OK;
}
