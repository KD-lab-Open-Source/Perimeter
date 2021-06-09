#include "SceneMesh.h"

#include "../Inc/scenelogic.h"

int cLogicScene::Read(cMeshFile &f)
{
	if((f.ReadFieldType()!=MF_ID_BLOCK_LOGIC)||(f.ReadBlock()!=MF_ID_BLOCK_LOGIC)) return MESHFILE_ERROR;
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_NAME:
				f.ReadField(name);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_LOGIC:
						return MESHFILE_OK;
					case MF_ID_BLOCK_CHANNEL:
						ChannelLibrary.Append()->Read(f);
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
int cLogicScene::Write(cMeshFile &f)
{
	if(ID==0) return MESHFILE_OK;
	f.BeginWriteBlock(MF_ID_BLOCK_LOGIC);
	f.WriteField(&ID,MF_TYPE_ID); 
	f.WriteField(&name,MF_TYPE_NAME); 
	if(ChannelLibrary.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	f.EndWriteBlock(MF_ID_BLOCK_LOGIC);
	return MESHFILE_OK;
}
