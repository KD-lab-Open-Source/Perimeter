#include "SceneMesh.h"

int sObjectMesh::Read(cMeshFile &f)
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
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_OBJECT:
						return MESHFILE_OK;
					case MF_ID_BLOCK_ANIMATIONMESH:
						AnimationMeshLibrary.Append()->Read(f);
						break;
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
int sObjectMesh::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_OBJECT);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&name,MF_TYPE_NAME);
	f.WriteField(&parent,MF_TYPE_PARENT);
	f.WriteField(&matrix,MF_TYPE_MATRIX);
	if(AnimationMeshLibrary.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationVisibility.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationPosition.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationRotation.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationScale.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(AnimationScaleRotation.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	f.EndWriteBlock(MF_ID_BLOCK_OBJECT);
	return MESHFILE_OK;
}
/*
sObjectMesh& sObjectMesh::operator = (const sObjectMesh &ObjectMesh)
{
	Release();
	name=ObjectMesh.name;
	parent=ObjectMesh.parent;
	memcpy(matrix,ObjectMesh.matrix,12*sizeof(float));
	AnimationMeshLibrary=ObjectMesh.AnimationMeshLibrary;
	return *this;
}
*/