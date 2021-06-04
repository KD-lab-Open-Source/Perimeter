#include "StdAfxRD.h"
#include "SpriteNode.h"
#include "Scene.h"

const int SPRITELIGHT_HIDETIME=2000;

cSpriteNode::cSpriteNode() : cAnimUnkObj(KIND_SPRITE_NODE)
{
	Intensity=1;
}
cSpriteNode::~cSpriteNode()
{
}
void cSpriteNode::Animate(float dt)
{
	if(GetFrame()) // обновить фазу
		GetFrame()->AddPhase(dt);
}
void cSpriteNode::PreDraw(cCamera *DrawNode)
{ 
	if(!DrawNode->TestVisible(GetGlobalMatrix(),-cUnkObj::GetScale(),cUnkObj::GetScale()))
		return;

	DrawNode->Attach(SCENENODE_OBJECTSORT,this); // спрайты всегда выводятся последними
}
void cSpriteNode::Draw(cCamera *DrawNode)
{
	if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION)&&(GetGlobalMatrix().trans().z<DrawNode->GetHReflection()))
		return;
	sColor4c Diffuse;
	if(GetAttribute(ATTRUNKOBJ_NOLIGHT)==0)
	{ // расчет освещения объкта
		sColor4f fDiffuse(0,0,0,1),fSpecular(0,0,0,1);
		DrawNode->GetScene()->GetLighting(GetGlobalMatrix().trans(),fDiffuse,fSpecular);
		Diffuse.set(fDiffuse*GetDiffuse());
	}else
		Diffuse.set(GetDiffuse().GetR(),GetDiffuse().GetG(),GetDiffuse().GetB(),round(255*GetDiffuse().a*GetIntensity()));

	cVertexBuffer<sVertexXYZDT1>* buf=gb_RenderDevice->GetBufferXYZDT1();
	sVertexXYZDT1 *v=buf->Lock(4);

	Vect3f sx=GetScale().x*DrawNode->GetWorldI(),sy=GetScale().y*DrawNode->GetWorldJ();
	v[0].pos=GetGlobalMatrix().trans()+sx+sy; v[0].u1()=0, v[0].v1()=0; 
	v[1].pos=GetGlobalMatrix().trans()+sx-sy; v[1].u1()=0, v[1].v1()=1;
	v[2].pos=GetGlobalMatrix().trans()-sx+sy; v[2].u1()=1, v[2].v1()=0;
	v[3].pos=GetGlobalMatrix().trans()-sx-sy; v[3].u1()=1, v[3].v1()=1;
	v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=Diffuse;
	buf->Unlock(4);

	gb_RenderDevice->SetNoMaterial(ALPHA_BLEND,GetFrame()->GetPhase(),GetTexture());
	buf->DrawPrimitive(PT_TRIANGLESTRIP,2,MatXf::ID);
}


///////////////////////////cSpriteNode////////////////////////
cSpriteManager::cSpriteManager()
: cAnimUnkObj(0)
{
	MTINIT(sprite_lock);
}

cSpriteManager::~cSpriteManager()
{
	MTDONE(sprite_lock);
	VISASSERT(sprites.size()==0);
}

void cSpriteManager::Animate(float dt)
{
	if(GetFrame()) // обновить фазу
		GetFrame()->AddPhase(dt);
}

void cSpriteManager::PreDraw(cCamera *camera)
{
	camera->Attach(SCENENODE_OBJECTSORT,this); // спрайты всегда выводятся последними
}

void cSpriteManager::Draw(cCamera *camera)
{
	MTEnter lock(sprite_lock);
	cQuadBuffer<sVertexXYZDT1>* quad=gb_RenderDevice->GetQuadBufferXYZDT1();
	gb_RenderDevice->SetNoMaterial(ALPHA_BLEND,GetFrame()->GetPhase(),GetTexture());

	quad->BeginDraw();
	list<cSprite>::iterator it;
	FOR_EACH(sprites,it)
	{
		cSprite& s=*it;
		if(s.ignore)
			continue;
		if(!camera->TestVisible(s.pos,s.radius))
			continue;

		sVertexXYZDT1* v=quad->Get();
		Vect3f sx=s.radius*camera->GetWorldI(),
			   sy=s.radius*camera->GetWorldJ();
		
		v[0].pos=s.pos+sx+sy; v[0].u1()=0, v[0].v1()=0; 
		v[1].pos=s.pos+sx-sy; v[1].u1()=0, v[1].v1()=1;
		v[2].pos=s.pos-sx+sy; v[2].u1()=1, v[2].v1()=0;
		v[3].pos=s.pos-sx-sy; v[3].u1()=1, v[3].v1()=1;
		v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=s.color;
	}
	quad->EndDraw();
}

cSprite* cSpriteManager::Create()
{
	MTEnter lock(sprite_lock);
	sprites.push_back(cSprite());
	cSprite* p=&sprites.back();
	p->manager=this;
	return p;
}

void cSprite::Release()
{
	manager->DeleteSprite(this);
}

void cSpriteManager::DeleteSprite(cSprite* sprite)
{
	MTEnter lock(sprite_lock);
	list<cSprite>::iterator it;
	FOR_EACH(sprites,it)
	{
		cSprite* s=&*it;
		if(s==sprite)
		{
			sprites.erase(it);
			return;
		}
	}

	VISASSERT(0 && "Cannot found sprite to delete");
}
