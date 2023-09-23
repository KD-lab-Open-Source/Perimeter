#include "StdAfxRD.h"
#include "D3DRender.h"
#include "shaders.h"
#include "UnkLight.h"

#define VAR_HANDLE(x) GetVariableByName(x,#x);

/*
О HLSL:
 Для ускорения 
	вместо
		pConstantTable->SetFloat
	используется
		gb_RenderDevice3D->SetVertexShaderConstant

  Константы по умолчанию не устанапливаются!!!
  
*/

std::vector<cShader*> cShader::all_shader;

void cD3DRender::SetVertexShaderConstant(int start,const Mat4f *pMat)
{
	RDCALL(lpD3DDevice->SetVertexShaderConstantF(start,(float*)pMat,4));
}

void cD3DRender::SetVertexShaderConstant(int start,const Vect4f *pVect)
{
	RDCALL(lpD3DDevice->SetVertexShaderConstantF(start,(float*)pVect,1));
}

void cD3DRender::SetPixelShaderConstant(int start,const Vect4f *pVect)
{
	RDCALL(lpD3DDevice->SetPixelShaderConstantF(start,(float*)pVect,1));
}

cShader::cShader()
{
	all_shader.push_back(this);
}

cShader::~cShader()
{
	std::vector<cShader*>::iterator it=
	find(all_shader.begin(),all_shader.end(),this);
	if(it!=all_shader.end())
		all_shader.erase(it);
	else
		VISASSERT(0);
}

cVertexShader::cVertexShader()
{
	pShaderInfo=NULL;
	ShaderInfoSize=0;
}

cVertexShader::~cVertexShader()
{
	Delete();
}

HRESULT cVertexShader::Compile(const char* name,const uint32_t* shader)
{
	LPDIRECT3DVERTEXSHADER9 ddshader=0;
    const DWORD* shader_code = checked_reinterpret_cast_ptr<const uint32_t, const DWORD>(shader);
	HRESULT hr=gb_RenderDevice3D->lpD3DDevice->CreateVertexShader(shader_code, &ddshader);
	SHADER s;
	s.pShader=ddshader;
	pShader.push_back(s);
	return hr;
}

void cVertexShader::Select(int num)
{
	for(int i=0;i<8;i++)
		gb_RenderDevice3D->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,i);
	gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,0);
	gb_RenderDevice3D->SetTextureStageState(1,D3DTSS_TEXTURETRANSFORMFLAGS,0);

	VISASSERT(num>=0 && num<pShader.size());
	SHADER& s=pShader[num];

	gb_RenderDevice3D->SetVertexShader(s.pShader);
}

void cVertexShader::GetHandle()
{
}


void cVertexShader::Delete()
{
	std::vector<SHADER>::iterator it;

	FOR_EACH(pShader,it)
	{
		LPDIRECT3DVERTEXSHADER9& shader=it->pShader;
		RELEASE(shader);
	}
	pShader.clear();
}

void cVertexShader::Restore()
{
	//unsigned int fp=_controlfp(0,0);
	//_controlfp( _MCW_EM,  _MCW_EM );

	Delete();
	RestoreShader();

	//_clearfp();
	//_controlfp(fp,0xFFFFFFFFul);
}

inline void cVertexShader::SetMatrix(const SHADER_HANDLE& h,const Mat4f* mat)
{
	if(h.num_register)
	{
		Mat4f out;
        out.xpose(*mat);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register,(float*)&out,h.num_register));
//		pConstantTable->SetMatrix(gb_RenderDevice3D->lpD3DDevice,h,mat);
	}
}

inline void cVertexShader::SetMatrix4x4(const SHADER_HANDLE& h,int index,const Mat4f* mat)
{
	const int size=4;
	if(h.num_register)
	{
		Mat4f out;
        out.xpose(*mat);
		xassert(index*size<=h.num_register);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register+index*size,(float*)&out,size));
	}
}

inline void cVertexShader::SetMatrix4x3(const SHADER_HANDLE& h,int index,const Mat4f* mat)
{
	const int size=3;
	if(h.num_register)
	{
		Mat4f out;
        out.xpose(*mat);
		xassert(index*size<=h.num_register);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register+index*size,(float*)&out,size));
	}
}

inline void cVertexShader::SetVector(const SHADER_HANDLE& h,const Vect4f* vect)
{
	if(h.num_register)
	{
		gb_RenderDevice3D->SetVertexShaderConstant(h.begin_register,vect);
//		pConstantTable->SetVector(gb_RenderDevice3D->lpD3DDevice,h,vect);
	}
}

inline void cVertexShader::SetFloat(const SHADER_HANDLE& h,const float vect)
{
	if(h.num_register)
	{
        Vect4f v(vect,vect,vect,vect);
		gb_RenderDevice3D->SetVertexShaderConstant(h.begin_register,&v);
//		pConstantTable->SetFloat(gb_RenderDevice3D->lpD3DDevice,h,vect);
	}
}

void cVertexShader::CompileAndFound(const char* name,const uint32_t* shader)
{
	LPDIRECT3DVERTEXSHADER9 ddshader=0;
    const DWORD* shader_code = checked_reinterpret_cast_ptr<const uint32_t, const DWORD>(shader);
	RDCALL(gb_RenderDevice3D->lpD3DDevice->CreateVertexShader(shader_code, &ddshader));
	GetHandle();
	SHADER s;
	s.pShader=ddshader;
	pShader.push_back(s);
}


void cVertexShader::GetVariableByName(SHADER_HANDLE& sh,const char* name)
{
	int i;
	for(i=0;i<ShaderInfoSize;i++)
	{
		ConstShaderInfo& s=pShaderInfo[i];
		if(strcmp(s.name,name)==0)
		{
			break;
		}
	}

	if(i>=ShaderInfoSize)
	{
		if(!pShader.empty())
		{
			VISASSERT(sh.begin_register==0 && sh.num_register==0);
		}

		sh.begin_register=0;
		sh.num_register=0;
		return;
	}

	ConstShaderInfo& desc=pShaderInfo[i];
	if(!pShader.empty() && sh.num_register)
	{
		//Потому как SHADER_HANDLE один, а шейдеров много
		VISASSERT(sh.begin_register==desc.begin_register && sh.num_register==desc.num_register);
	}
	sh.begin_register=desc.begin_register;
	sh.num_register=desc.num_register;
}

void VSScene::SetLight(SHADER_HANDLE& spos,SHADER_HANDLE& scolor,SHADER_HANDLE& sparam,cUnkLight* l)
{
	sColor4f color=l->GetDiffuse();
	Vect3f pos=l->GetPos();
	float radius=max(l->GetRadius(),1.0f);
	Vect4f position(pos.x,pos.y,pos.z,1);
	Vect4f r(1,1/radius/radius,0,0.5f);
	SetVector(spos,&position);
	SetVector(scolor, reinterpret_cast<const Vect4f*>(&color));
	SetVector(sparam,&r);
}

/////////////////////////////////PixelShader/////////////////////////////
PixelShader::PixelShader()
{
	pShader=NULL;
}

PixelShader::~PixelShader()
{
	RELEASE(pShader);
}

void PixelShader::Select()
{
	gb_RenderDevice3D->SetPixelShader(pShader);
}

HRESULT PixelShader::Compile(const char* name,const uint32_t* shader)
{
	RELEASE(pShader);
    const DWORD* shader_code = checked_reinterpret_cast_ptr<const uint32_t, const DWORD>(shader);
	return gb_RenderDevice3D->lpD3DDevice->CreatePixelShader(shader_code, &pShader);
}

void PixelShader::Delete()
{
	RELEASE(pShader);
}

////////////////////////////////////////////////////////

void VSShadow::Select(const MatXf& world)
{
	Mat4f mat;
    Mat4fSetTransposedMatXf(mat, world);
    mat = mat * gb_RenderDevice3D->GetDrawNode()->matViewProj;
    mat.xpose();

	gb_RenderDevice3D->SetVertexShaderConstant(0, &mat);

	cVertexShader::Select();
}

void cVertexShader::SetTextureTransform(MatXf& m)
{
/*
	Vect4f c0(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				c1(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);

	gb_RenderDevice3D->SetVertexShaderConstant(26,&c0);
	gb_RenderDevice3D->SetVertexShaderConstant(27,&c1);
*/
}

void PSShowMap::Restore()
{
#include "o/showmap.ph"
}

void VSChaos::Select(float umin,float vmin,float umin2,float vmin2,
					 float umin_b0,float vmin_b0,float umin_b1,float vmin_b1)
{
	SetFog();
	SetMatrix(mWVP, &gb_RenderDevice3D->GetDrawNode()->matViewProj);
    Vect4f uv(umin,vmin,umin2,vmin2);
    Vect4f uvb(umin_b0,vmin_b0,umin_b1,vmin_b1);
	SetVector(mUV,&uv);
	SetVector(mUVBump,&uvb);
	SetMatrix(mWorldView, &gb_RenderDevice3D->GetDrawNode()->matView);

	cVertexShader::Select();
}

void VSChaos::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mWVP);
	VAR_HANDLE(mUV);
	VAR_HANDLE(mUVBump);
	VAR_HANDLE(mWorldView);
	VAR_HANDLE(vFog);
}

void VSChaos::RestoreShader()
{
	if(gb_RenderDevice3D->bSupportTableFog)
	{
		#include "Chaos/o/chaos.vl"
	}else
	{
		#include "Chaos/o/chaos_fog.vl"
	}
}

void PSChaos::Restore()
{
#include "Chaos/o/chaos.ph"
}

void VSChaos::SetFog()
{
    Vect4f v = gb_RenderDevice3D->dtAdvance->GetFogParam();
    SetVector(vFog,&v);
}

#include "ShadersGeforce3.inl"
#include "ShadersRadeon9700.inl"
#include "ShadersPS14.inl"
#include "ShadersGeforceFX.inl"
