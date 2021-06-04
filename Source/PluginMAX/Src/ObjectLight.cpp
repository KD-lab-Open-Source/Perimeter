#include "SceneMesh.h"

int sLightObject::Read(cMeshFile &f)
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
			case MF_TYPE_PARENT:
				f.ReadField(parent);
				break;
			case MF_TYPE_MATRIX:
				f.ReadField(&matrix);
				break;
			case MF_TYPE_NUMBERSUBOBJECT:
				f.ReadField(&idObject);
				break;
			case MF_TYPE_FILENAME:
				f.ReadField(TexProj);
				break;
			case MF_TYPE_INT_VAR1:
				f.ReadField(&UseGlobal);
				break;
			case MF_TYPE_MATRIX1:
				f.ReadField(&OfsMatrix);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_LIGHTOBJECT:
						return MESHFILE_OK;
					case MF_ID_BLOCK_ANIMATIONVISIBILITY:
						AnimationVisibility.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONPOSITION:
						AnimationPosition.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONROTATION:
						AnimationRotation.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONSCALE:
						AnimationScale.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONSCALEROTATION:
						AnimationScaleRotation.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONLIGHT:
						AnimationLightLibrary.Append()->Read(f);
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
int sLightObject::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_LIGHTOBJECT);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&name,MF_TYPE_NAME);
	f.WriteField(&parent,MF_TYPE_PARENT);
	f.WriteField(&matrix,MF_TYPE_MATRIX);
	f.WriteField(&idObject,MF_TYPE_NUMBERSUBOBJECT);
	f.WriteField(&OfsMatrix,MF_TYPE_MATRIX1);
	f.WriteField(&UseGlobal,MF_TYPE_INT_VAR1);
	if(!TexProj.empty()) f.WriteField(&TexProj,MF_TYPE_FILENAME);
	if(AnimationVisibility.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationPosition.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationRotation.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationScale.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationScaleRotation.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationLightLibrary.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	f.EndWriteBlock(MF_ID_BLOCK_LIGHTOBJECT);
	return MESHFILE_OK;
}
