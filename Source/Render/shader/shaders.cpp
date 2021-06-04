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

vector<cShader*> cShader::all_shader;

void cD3DRender::SetVertexShaderConstant(int start,const D3DXMATRIX *pMat)
{
	RDCALL(lpD3DDevice->SetVertexShaderConstantF(start,(float*)pMat,4));
}

void cD3DRender::SetVertexShaderConstant(int start,const D3DXVECTOR4 *pVect)
{
	RDCALL(lpD3DDevice->SetVertexShaderConstantF(start,(float*)pVect,1));
}

void cD3DRender::SetPixelShaderConstant(int start,const D3DXVECTOR4 *pVect)
{
	RDCALL(lpD3DDevice->SetPixelShaderConstantF(start,(float*)pVect,1));
}

cShader::cShader()
{
	all_shader.push_back(this);
}

cShader::~cShader()
{
	vector<cShader*>::iterator it=
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

HRESULT cVertexShader::Compile(const char* name,const DWORD* shader)
{
	LPDIRECT3DVERTEXSHADER9 ddshader=0;
	HRESULT hr=gb_RenderDevice3D->lpD3DDevice->CreateVertexShader(shader, &ddshader);
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
	vector<SHADER>::iterator it;

	FOR_EACH(pShader,it)
	{
		LPDIRECT3DVERTEXSHADER9& shader=it->pShader;
		RELEASE(shader);
	}
	pShader.clear();
}

void cVertexShader::Restore()
{
	unsigned int fp=_controlfp(0,0);
	_controlfp( MCW_EM,  MCW_EM ); 
	Delete();
	RestoreShader();
	_clearfp();
	_controlfp(fp,0xFFFFFFFFul);
}

inline void cVertexShader::SetMatrix(const SHADER_HANDLE& h,const D3DXMATRIX* mat)
{
	if(h.num_register)
	{
		D3DXMATRIX out;
		D3DXMatrixTranspose(&out,mat);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register,(float*)&out,h.num_register));
//		pConstantTable->SetMatrix(gb_RenderDevice3D->lpD3DDevice,h,mat);
	}
}

inline void cVertexShader::SetMatrix4x4(const SHADER_HANDLE& h,int index,const D3DXMATRIX* mat)
{
	const int size=4;
	if(h.num_register)
	{
		D3DXMATRIX out;
		D3DXMatrixTranspose(&out,mat);
		xassert(index*size<=h.num_register);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register+index*size,(float*)&out,size));
	}
}

inline void cVertexShader::SetMatrix4x3(const SHADER_HANDLE& h,int index,const D3DXMATRIX* mat)
{
	const int size=3;
	if(h.num_register)
	{
		D3DXMATRIX out;
		D3DXMatrixTranspose(&out,mat);
		xassert(index*size<=h.num_register);
		RDCALL(gb_RenderDevice3D->lpD3DDevice->
			SetVertexShaderConstantF(h.begin_register+index*size,(float*)&out,size));
	}
}

inline void cVertexShader::SetVector(const SHADER_HANDLE& h,const D3DXVECTOR4* vect)
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
		gb_RenderDevice3D->SetVertexShaderConstant(h.begin_register,&D3DXVECTOR4(vect,vect,vect,vect));
//		pConstantTable->SetFloat(gb_RenderDevice3D->lpD3DDevice,h,vect);
	}
}

void cVertexShader::CompileAndFound(const char* name,const DWORD* shader)
{
    LPD3DXBUFFER pErrorBuf = NULL;

	LPDIRECT3DVERTEXSHADER9 ddshader=0;
	RDCALL(gb_RenderDevice3D->lpD3DDevice->CreateVertexShader(shader, &ddshader));
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
	sColor4f color=l->GetDiffuse();;
	Vect3f pos=l->GetPos();
	float radius=max(l->GetRadius(),1.0f);
	D3DXVECTOR4 position(pos.x,pos.y,pos.z,1);
	D3DXVECTOR4 r(1,1/radius/radius,0,0.5f);
	SetVector(spos,&position);
	SetVector(scolor,(D3DXVECTOR4*)&color);
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

HRESULT PixelShader::Compile(const char* name,const DWORD* shader)
{
	RELEASE(pShader);
	return gb_RenderDevice3D->lpD3DDevice->CreatePixelShader(shader, &pShader);
}

void PixelShader::Delete()
{
	RELEASE(pShader);
}

////////////////////////////////////////////////////////

void VSShadow::Select(const MatXf& world)
{
	D3DXMATRIX mat;
	cD3DRender_SetMatrix(mat,world);
	D3DXMatrixMultiply(&mat,&mat,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	D3DXMatrixTranspose(&mat,&mat);

	gb_RenderDevice3D->SetVertexShaderConstant(0, &mat);

	cVertexShader::Select();
}

void cVertexShader::SetTextureTransform(MatXf& m)
{
/*
	D3DXVECTOR4 c0(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				c1(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);

	gb_RenderDevice3D->SetVertexShaderConstant(26,&c0);
	gb_RenderDevice3D->SetVertexShaderConstant(27,&c1);
*/
}

void PSShowMap::Restore()
{
#include "o\showmap.ph"
}

void VSChaos::Select(float umin,float vmin,float umin2,float vmin2,
					 float umin_b0,float vmin_b0,float umin_b1,float vmin_b1)
{
	SetFog();
	SetMatrix(mWVP,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	SetVector(mUV,&D3DXVECTOR4(umin,vmin,umin2,vmin2));
	SetVector(mUVBump,&D3DXVECTOR4(umin_b0,vmin_b0,umin_b1,vmin_b1));
	SetMatrix(mWorldView,gb_RenderDevice3D->GetDrawNode()->matView);

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
		#include "chaos\o\chaos.vl"
	}else
	{
		#include "chaos\o\chaos_fog.vl"
	}
}

void PSChaos::Restore()
{
#include "chaos\o\chaos.ph"
}

void VSChaos::SetFog()
{
	SetVector(vFog,&gb_RenderDevice3D->dtAdvance->GetFogParam());
}

#include "ShadersGeforce3.inl"
#include "ShadersRadeon9700.inl"
#include "ShadersPS14.inl"
#include "ShadersGeforceFX.inl"

#include "ShaderSkin.inl"
#include "ShaderWater.inl"
