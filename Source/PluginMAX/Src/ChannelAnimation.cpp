#include "SceneMesh.h"

int sChannelAnimation::Read(cMeshFile &f)
{
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_NAME:
				f.ReadField(name);
				break;
			case MF_TYPE_FIRSTFRAME:
				f.ReadField(&FirstFrame);
				break;
			case MF_TYPE_LASTFRAME:
				f.ReadField(&LastFrame);
				break;
			case MF_TYPE_FRAMESPEED:
				f.ReadField(&FrameSpeed);
				break;
			case MF_TYPE_TICKSPERFRAME:
				f.ReadField(&TicksPerFrame);
				break;
			case MF_TYPE_NUMBERFRAME:
				f.ReadField(&NumberFrame);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_CHANNEL:
						if(NumberFrame==0) NumberFrame=GetNumberMeshAnimation();
						return MESHFILE_OK;
					case MF_ID_BLOCK_LOD:
						LodLibrary.Append()->Read(f);
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
int sChannelAnimation::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_CHANNEL);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&name,MF_TYPE_NAME);
	f.WriteField(&FirstFrame,MF_TYPE_FIRSTFRAME);
	f.WriteField(&LastFrame,MF_TYPE_LASTFRAME);
	f.WriteField(&FrameSpeed,MF_TYPE_FRAMESPEED);
	f.WriteField(&TicksPerFrame,MF_TYPE_TICKSPERFRAME);
	f.WriteField(&NumberFrame,MF_TYPE_NUMBERFRAME);
	if(LodLibrary.Write(f)!=MESHFILE_OK) return f.error;
	f.EndWriteBlock(MF_ID_BLOCK_CHANNEL);
	return MESHFILE_OK;
}
int sChannelAnimation::GetNumberMeshAnimation()
{
	int MaxNumber=0;
	cNodeObjectLibrary &NodeObjectLibrary=LodLibrary[0]->NodeObjectLibrary;
	for(int i=0;i<NodeObjectLibrary.length();i++)
		if(NodeObjectLibrary[i]->type==NODEOBJECT_MESH)
			if(MaxNumber<((sObjectMesh*)NodeObjectLibrary[i])->AnimationMeshLibrary.length())
				MaxNumber=((sObjectMesh*)NodeObjectLibrary[i])->AnimationMeshLibrary.length();
	return MaxNumber;
}
