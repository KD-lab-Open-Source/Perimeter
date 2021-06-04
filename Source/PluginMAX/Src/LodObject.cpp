#include "SceneMesh.h"

int sLodObject::Read(cMeshFile &f)
{
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_LOD:
				f.ReadField(&LevelDetail);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_LOD:
						return MESHFILE_OK;
					case MF_ID_BLOCK_OBJECT:
						{
							sObjectMesh *ObjectMesh=new sObjectMesh;
							NodeObjectLibrary.New(ObjectMesh);
							ObjectMesh->Read(f);
							break;
						}
					case MF_ID_BLOCK_HELPEROBJECT:
						{
							sHelperObject *HelperObject=new sHelperObject;
							NodeObjectLibrary.New(HelperObject);
							HelperObject->Read(f);
							break;
						}
					case MF_ID_BLOCK_LIGHTOBJECT:
						{
							sLightObject *LightObject=new sLightObject;
							NodeObjectLibrary.New(LightObject);
							LightObject->Read(f);
							break;
						}
					case MF_ID_BLOCK_CAMERAOBJECT:
						{
							sCameraObject *CameraObject=new sCameraObject;
							NodeObjectLibrary.New(CameraObject);
							CameraObject->Read(f);
							break;
						}
					case MF_ID_BLOCK_MATERIAL:
						MaterialLibrary.Append()->Read(f);
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
int sLodObject::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_LOD);
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&LevelDetail,MF_TYPE_LOD);
	if(MaterialLibrary.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(NodeObjectLibrary.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	f.EndWriteBlock(MF_ID_BLOCK_LOD);
	return MESHFILE_OK;
}
