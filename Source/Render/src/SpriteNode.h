#pragma once

class cSprite
{
public:
	cSprite()
	{
		pos.set(0,0,0);
		color.set(255,255,255,255);
		radius=10;
		ignore=false;
	}

	void Release();
	void SetPos(const Vect3f& p){pos=p;};
	void SetColor(sColor4c c){color=c;}
	void SetRadius(float r){radius=r;}
	void SetIgnore(bool b){ignore=b;}
protected:
	Vect3f pos;
	sColor4c color;
	float radius;
	bool ignore;

	class cSpriteManager* manager;
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
	std::list<cSprite> sprites;
	MTDECLARE(sprite_lock);

	void DeleteSprite(cSprite* sprite);
	friend cSprite;
};