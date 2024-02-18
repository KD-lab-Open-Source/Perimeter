#pragma once 

class cIUnkObj : public cIUnkClass
{ // базовый класс объектов
public:
	cIUnkObj(int kind);
	~cIUnkObj() override;

	void Animate(float dt) override { }
	void PreDraw(cCamera *UClass) override { }
	void Draw(cCamera *UClass) override { }
	void SetPosition(const MatXf& Matrix) override;
	const MatXf& GetPosition() const override { return LocalMatrix; }
	const MatXf& GetGlobalPosition() const override { return GlobalMatrix; }

	virtual cIUnkObj* BuildCopy()													{return NULL; }
	
	// инлайновые функции доступа к переменным
	inline const MatXf& GetLocalMatrix() const 										{ return LocalMatrix; }
	inline const MatXf& GetGlobalMatrix() const										{ return GlobalMatrix; }
	inline float& GetRadius()														{ return Radius; }

	virtual Vect3f GetCenterObject() override {return GetGlobalMatrix().trans();}

protected:
	MatXf			LocalMatrix;	// локальная матрица объекта относительно родителя
	MatXf			GlobalMatrix;	// глобальная матрица объекта, относительно мировых координат
	float			Radius;			// примерный размер объекта

protected:
	virtual void SetCopy(cIUnkObj* UObj);
};

class cIUnkObjScale : public cIUnkObj
{
public:
	cIUnkObjScale(int kind);
	~cIUnkObjScale() override;

	virtual const Vect3f& GetScale() const 			{ return Scale; }
	virtual void SetScale(const Vect3f& scale);
protected:
	Vect3f			Scale;	// масштаб
protected:
	void SetCopy(cIUnkObj* UObj) override;
};

