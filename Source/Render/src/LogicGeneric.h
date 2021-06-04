#pragma once
class cLogicTile : public cIUnkObj
{
public:
	cLogicTile(int kind);
	virtual int GetAnimation(MatXf *Matrix,int *Visible)=0;
	virtual cIUnkClass* GetParent()=0;
};

class cLogicTileInt;
class cLogicObject : public cIUnkObjScale 
{
protected:
	typedef vector<cLogicTileInt*> vtiles;
	vtiles				tiles;
	string				fname;								// имя файла из которого он был загружен

public:
	cLogicObject(const char *fname);
	~cLogicObject();
	virtual void SetScale(const Vect3f& scale);

	virtual float GetBoundRadius();
	virtual void GetBoundBox(sBox6f& Box);

	virtual cLogicTile* FindObject(const char *name);
	virtual void SetPosition(const MatXf& Matrix);
	virtual int SetChannel(const char *NameChain,float phase=0.0f);
	virtual void Draw(cCamera *UCamera);
	inline const char* GetName() const							{ return fname.c_str(); }

	void Attach(cLogicTileInt* p);
	virtual cIUnkClass* NextObject(cIUnkClass *UObj);
	virtual void Update();

	void SetCopy(cIUnkObj* UObj);
	cIUnkObj* BuildCopy();
};


class cLogicGeneric : public cUnknownClass
{
public:
	//Для общего использования
	cLogicGeneric();
	virtual ~cLogicGeneric();
	void Free();
	cLogicObject* GetElement(const char *pFName);
protected:
	vector<cLogicObject*> objects;
};
