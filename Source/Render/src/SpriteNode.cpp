#include "StdAfxRD.h"
#include "SpriteNode.h"
#include "Scene.h"
#include "DrawBuffer.h"

///////////////////////////cSpriteNode////////////////////////
cSpriteManager::cSpriteManager()
: cAnimUnkObj(0)
{
    sprites.resize(1000);
	MTINIT(sprite_lock);
}

cSpriteManager::~cSpriteManager()
{
	MTDONE(sprite_lock);
    
    for (const cSprite* sptr : sprites) {
        VISASSERT(sptr == nullptr);
    }
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
    gb_RenderDevice->SetWorldMatXf(MatXf::ID);
    
    if (sprites.empty()) return;

    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
    indices_t* ib = nullptr;
    sVertexXYZDT1 *v = nullptr;
    
    for (const cSprite* sptr : sprites) {
        if (sptr == nullptr) {
            continue;
        }
        const cSprite& s = *sptr;
        if (s.ignore || !camera->TestVisible(s.pos,s.radius)) {
            continue;
        }

        Vect3f sx=s.radius*camera->GetWorldI(),
               sy=s.radius*camera->GetWorldJ();

        db->AutoLockQuad(std::min(sprites.size(), static_cast<size_t>(10)), 1, v, ib);

        v[0].pos=s.pos+sx+sy; v[0].u1()=0, v[0].v1()=0;
        v[1].pos=s.pos+sx-sy; v[1].u1()=0, v[1].v1()=1;
        v[2].pos=s.pos-sx+sy; v[2].u1()=1, v[2].v1()=0;
        v[3].pos=s.pos-sx-sy; v[3].u1()=1, v[3].v1()=1;
        v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(s.color);
    }

    db->AutoUnlock();
}

cSprite* cSpriteManager::Create()
{
	MTEnter lock(sprite_lock);
    cSprite* p = nullptr;
    for (auto& sptr : sprites) {
        if (sptr == nullptr) {
            sptr = p = new cSprite();
            break;
        }
    }
    if (p == nullptr) {
        p = new cSprite();
        sprites.emplace_back(p);
    }
	p->manager=this;
	return p;
}

int64_t cSprite::Release()
{
    if (GetRef() <= 1) {
        manager->DeleteSprite(this);
        return cUnknownClass::Release();
    }
    return DecRef();
}

void cSpriteManager::DeleteSprite(cSprite* sprite)
{
	MTEnter lock(sprite_lock);
    for (auto& i : sprites) {
        if (i == sprite) {
            i = nullptr;
            return;
        }
    }

	VISASSERT(0 && "Cannot found sprite to delete");
}
