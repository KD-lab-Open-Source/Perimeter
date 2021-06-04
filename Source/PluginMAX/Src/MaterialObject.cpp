#include "SceneMesh.h"

int sMaterialObject::ReadOld(cMeshFile &f)
{ // для совместимости с предыдущими версиями
	int id,time=0;
	while(!f.eof())
		switch(f.ReadFieldType())
		{
			case MF_TYPE_ID:
				f.ReadField(&id);
				break;
			case MF_TYPE_TIME:
				f.ReadField(&time);
				break;
			case MF_TYPE_AMBIENTCOLOR: 
				AmbientAnimErr.New(1);
				f.ReadField(&AmbientAnimErr[0][1]);
				AmbientAnimErr[0][0]=(float)time;
				break;
			case MF_TYPE_DIFFUSECOLOR: 
				DiffuseAnim.New(1);
				f.ReadField(&DiffuseAnim[0][1]);
				DiffuseAnim[0][0]=(float)time;
				break;
			case MF_TYPE_SPECULARCOLOR: 
				SpecularAnim.New(1);
				f.ReadField(&SpecularAnim[0][1]);
				SpecularAnim[0][0]=(float)time;
				break;
			case MF_TYPE_FLOAT_VAR1:
				ShininessAnim.New(1);
				f.ReadField(&ShininessAnim[0][1]);
				ShininessAnim[0][0]=(float)time;
				break;
			case MF_TYPE_FLOAT_VAR2: 
				ShinStrengthAnim.New(1);
				f.ReadField(&ShinStrengthAnim[0][1]);
				ShinStrengthAnim[0][0]=(float)time;
				break;
			case MF_TYPE_FLOAT_VAR3: 
				TransparencyAnim.New(1);
				f.ReadField(&TransparencyAnim[0][1]);
				TransparencyAnim[0][0]=(float)time;
				break;
			case MF_TYPE_INT_VAR1: 
				f.ReadField(&SelfIllumColorOn);
				break;
			case MF_TYPE_INT_VAR2: 
				f.ReadField(&Falloff);
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_ANIMATIONMATERIAL:
						return MESHFILE_OK;
					case MF_ID_BLOCK_SUBTEXMAP:
						SubTexMap.Append()->Read(f);
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
int sMaterialObject::Read(cMeshFile &f)
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
			case MF_TYPE_SHADING:
				f.ReadField(&Shading);
				break;
			case MF_TYPE_INT_VAR1: 
				f.ReadField(&SelfIllumColorOn);
				break;
			case MF_TYPE_INT_VAR2: 
				f.ReadField(&Falloff);
				break;
			case MF_TYPE_BLEND: 
				f.ReadField(&blend_type);
				break;
			case MF_TYPE_NUMBERSUBOBJECT:
				f.ReadField(&NumberSubObject);
				break;
			// для совместимости версий с анимацией материала и без анимации материала
			case MF_TYPE_AMBIENTCOLOR:
				AmbientAnim.New(1);
				f.ReadField(&AmbientAnim[0][1]);
				AmbientAnim[0][0]=0;
				break;
			case MF_TYPE_DIFFUSECOLOR:
				DiffuseAnim.New(1);
				f.ReadField(&DiffuseAnim[0][1]);
				DiffuseAnim[0][0]=0;
				break;
			case MF_TYPE_SPECULARCOLOR: 
				SpecularAnim.New(1);
				f.ReadField(&SpecularAnim[0][1]);
				SpecularAnim[0][0]=0;
				break;
			case MF_TYPE_FLOAT_VAR1: 
				ShininessAnim.New(1);
				f.ReadField(&ShininessAnim[0][1]);
				ShininessAnim[0][0]=0;
				break;
			case MF_TYPE_FLOAT_VAR2: 
				ShinStrengthAnim.New(1);
				f.ReadField(&ShinStrengthAnim[0][1]);
				ShinStrengthAnim[0][0]=0;
				break;
			case MF_TYPE_FLOAT_VAR3: 
				TransparencyAnim.New(1);
				f.ReadField(&TransparencyAnim[0][1]);
				TransparencyAnim[0][0]=0;
				break;
			case MF_TYPE_BLOCK:
				switch(f.ReadBlock())
				{
					case MF_ID_BLOCK_MATERIAL:
						return MESHFILE_OK;
					case MF_ID_BLOCK_ANIMATIONMATERIAL:
						ReadOld(f);
						break;
					case MF_ID_BLOCK_SUBTEXMAP:
						SubTexMap.Append()->Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONAMBIENT:
						AmbientAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONDIFFUSE:
						DiffuseAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONSPECULAR: 
						SpecularAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATIONEMISSIVE: 
						EmissiveAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATION_FLOAT_VAR1: 
						ShininessAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATION_FLOAT_VAR2: 
						ShinStrengthAnim.Read(f);
						break;
					case MF_ID_BLOCK_ANIMATION_FLOAT_VAR3: 
						TransparencyAnim.Read(f);
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
int sMaterialObject::Write(cMeshFile &f)
{
	f.BeginWriteBlock(MF_ID_BLOCK_MATERIAL);

	f.WriteField(&ID,MF_TYPE_ID);
	f.WriteField(&name,MF_TYPE_NAME);
	f.WriteField(&Shading,MF_TYPE_SHADING);
	if(parent.c_str()) f.WriteField(&parent,MF_TYPE_PARENT);
	if(NumberSubObject) f.WriteField(&NumberSubObject,MF_TYPE_NUMBERSUBOBJECT);
	if(SelfIllumColorOn) f.WriteField(&SelfIllumColorOn,MF_TYPE_INT_VAR1);
	if(Falloff) f.WriteField(&Falloff,MF_TYPE_INT_VAR2);
	f.WriteField(&blend_type,MF_TYPE_BLEND);
	
	if(AmbientAnim.length())
		if(AmbientAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(DiffuseAnim.length())
		if(DiffuseAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(SpecularAnim.length())
		if(SpecularAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(EmissiveAnim.length())
		if(EmissiveAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(ShininessAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(ShinStrengthAnim.Write(f)!=MESHFILE_OK) return MESHFILE_ERROR;
	if(TransparencyAnim.length())
		if(TransparencyAnim.Write(f)!=MESHFILE_OK)
			return MESHFILE_ERROR;
	if(SubTexMap.length())
		SubTexMap.Write(f);

	f.EndWriteBlock(MF_ID_BLOCK_MATERIAL);
	return MESHFILE_OK;
}
