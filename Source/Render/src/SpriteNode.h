#pragma once

class cSprite : public cUnknownClass
{
public:
	cSprite() : cUnknownClass(0) {}
    ~cSprite() override = default;

	int64_t Release() override;
	void SetPos(const Vect3f& p){pos=p;};
	void SetColor(sColor4c c){color=c;}
	void SetRadius(float r){radius=r;}
	void SetIgnore(bool b){ignore=b;}
protected:
	Vect3f pos = Vect3f(0,0,0);
	sColor4c color = sColor4c(255,255,255,255);
	float radius = 10;
	bool ignore = false;

	class cSpriteManager* manager = nullptr;
	friend class cSpriteManager;
};

class cSpriteManager : public cAnimUnkObj//cIUnkClass
{
public:
	cSpriteManager();
	~cSpriteManager();

	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);

	cSprite* Create();
protected:
	std::vector<cSprite*> sprites;
	MTDECLARE(sprite_lock);

	void DeleteSprite(cSprite* sprite);
	friend cSprite;
};