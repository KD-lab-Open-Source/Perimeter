#include "StdAfxRD.h"
#include "SpriteNode.h"
#include "Scene.h"

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
    
    gb_RenderDevice->SetNoMaterial(ALPHA_BLEND,GetFrame()->GetPhase(),GetTexture());

#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        cQuadBuffer<sVertexXYZDT1>* quad=gb_RenderDevice3D->GetQuadBufferXYZDT1();
    
        quad->BeginDraw();
        std::list<cSprite>::iterator it;
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
#endif
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
	std::list<cSprite>::iterator it;
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
