#include "StdAfx.h"
#include "ExportMaterial.h"

ExportMaterial::ExportMaterial(CSaver& saver_)
:saver(saver_)
{
}

bool ExportMaterial::Export(IGameMaterial * mat)
{
	const char* mat_name=mat->GetMaterialName();
	saver.push(C3DX_MATERIAL);
	{
		saver.push(C3DX_MATERIAL_HEAD);
		saver<<mat_name;
		saver.pop();
	}

	{
		if(mat->GetAmbientData()==NULL ||
		   mat->GetDiffuseData()==NULL ||
		   mat->GetSpecularData()==NULL ||
		   mat->GetOpacityData()==NULL ||
		   mat->GetSpecularLevelData()==NULL
		)
		{
			Msg("Error: %s - Неподдерживаемый тип материала\n",mat_name);
			return false;
		}

		saver.push(C3DX_MATERIAL_STATIC);
		float f=0;
		Point3 p;

		verify(mat->GetAmbientData()->GetPropertyValue(p));
		saver<<p.x<<p.y<<p.z;

		verify(mat->GetDiffuseData()->GetPropertyValue(p));
		saver<<p.x<<p.y<<p.z;

		verify(mat->GetSpecularLevelData()->GetPropertyValue(f));
		verify(mat->GetSpecularData()->GetPropertyValue(p));
		p.x*=f;
		p.y*=f;
		p.z*=f;
		saver<<p.x<<p.y<<p.z;

		verify(mat->GetOpacityData()->GetPropertyValue(f));
		saver<<f;

		Mtl* max=mat->GetMaxMaterial();
		f=max->GetShininess()*100;
		saver<<f;

		saver.pop();
	}

	for(int i=0;i<mat->GetNumberOfTextureMaps();i++)
	{
		IGameTextureMap* tex_map=mat->GetIGameTextureMap(i);
		saver.push(C3DX_MATERIAL_TEXTUREMAP);
		DWORD slot=tex_map->GetStdMapSlot();
		saver<<slot;
		const char* tex_name=tex_map->GetBitmapFileName();
		char filename_out[_MAX_PATH]="";
		if(tex_name)
		{
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			_splitpath( tex_name, drive, dir, fname, ext );
			_makepath( filename_out, "", "", fname, ext );
		}
		saver<<filename_out;
		saver.pop();
	}

	saver.pop();
	return true;
}
