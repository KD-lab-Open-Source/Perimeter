#include "SceneMesh.h"

int sSubTexmap::Read(cMeshFile &f)
{
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				ID=0; f.ReadField(&ID);
				break;
			case MF_TYPE_FILENAME:
				f.ReadField(name);
				break;
			case MF_TYPE_CHANNELNUMBER:
				f.ReadField(&ChannelNumber);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_SUBTEXMAP:
						return MESHFILE_OK;
					case MF_ID_BLOCK_ANIMATION_MATRIX:
						MatrixAnim.Read(f);
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
int sSubTexmap::Write(cMeshFile &f)
{
	if(ID==0) return MESHFILE_OK;
	f.BeginWriteBlock(MF_ID_BLOCK_SUBTEXMAP);
	
	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&name,MF_TYPE_FILENAME);
	f.WriteField(&ChannelNumber,MF_TYPE_CHANNELNUMBER);

	Vect3f diag=MatrixAnim[0].mat.rot().diag();
	Vect3f trans=MatrixAnim[0].mat.trans();
	if(MatrixAnim.length()>1 || 
		(
		 diag.x!=1 && diag.y!=1 && diag.z!=1 &&
		 trans.x!=0 && trans.y!=0 && trans.z!=0)
	   )
		MatrixAnim.Write(f);

	f.EndWriteBlock(MF_ID_BLOCK_SUBTEXMAP);
	return MESHFILE_OK;
}
