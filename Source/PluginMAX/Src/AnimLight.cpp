#include "SceneMesh.h"

int sAnimationLight::Read(cMeshFile &f)
{
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_TIME:
				f.ReadField(&time);
				break;
			case MF_TYPE_DIFFUSECOLOR: 
				f.ReadField(DiffuseColor);
				break;
			case MF_TYPE_FLOAT_VAR1:
				f.ReadField(&TDist);
				break;
			case MF_TYPE_FLOAT_VAR2:
				f.ReadField(&FarBeginAttenuation);
				break;
			case MF_TYPE_FLOAT_VAR3:
				f.ReadField(&FarFinishAttenuation);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_ANIMATIONLIGHT:
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
int sAnimationLight::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_ANIMATIONLIGHT);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&time,MF_TYPE_TIME);
	f.WriteField(DiffuseColor,MF_TYPE_DIFFUSECOLOR);
	f.WriteField(&TDist,MF_TYPE_FLOAT_VAR1);
	f.WriteField(&FarBeginAttenuation,MF_TYPE_FLOAT_VAR2);
	f.WriteField(&FarFinishAttenuation,MF_TYPE_FLOAT_VAR3);
	f.EndWriteBlock(MF_ID_BLOCK_ANIMATIONLIGHT);
	return MESHFILE_OK;
}
