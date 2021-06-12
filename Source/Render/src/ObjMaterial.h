#ifndef __OBJMATERIAL_H__
#define __OBJMATERIAL_H__

enum eAttributeMaterial
{
	ATTRIBUTE_MATERIAL_NULL		=	0<<0,
	// аттрибуты поверхности материала	1..7
	ATTRIBUTE_COLOR_MUL			=	1<<1,	// материал содержит мультипликативный цвет
	ATTRIBUTE_COLOR_ADD			=	1<<2,	// материал содержит аддитивный цвет
	ATTRIBUTE_ALPHA				=	1<<3,	// материал содержит прозрачность
	ATTRIBUTE_TEXTURE			=	1<<4,	// материал содержит текстуру
	ATTRIBUTE_OPACITY			=	1<<5,	// материал с прозрачностью в текстуре
	ATTRIBUTE_ALPHATEST			=	1<<6,	// материал с текстурой для которой надо включить альфатест
//	ATTRIBUTE_BUMP				=	1<<7,	// текстура с поддержкой бампа
	// аттрибуты материального тела	10..15
	ATTRIBUTE_PLASTIC			=	1<<10,	// пластмасса - расчет освещенности для пластмасс
	ATTRIBUTE_METAL				=	1<<11	// металл - расчет освещенности для мелиических поверхностей с бликом
};

// cObjectMaterial - класс материала для объектов (KIND_OBJ_MATERIAL,TYPE_NULL)
// удаляет текстуры на прямую из библиотеки, возможно последующее включение бампмап-текстур и т.д.
class cObjectMaterial : public cUnknownClass
{	// материал объекта, цвет формирует как = texture * MulColor + AddColor
	eAttributeMaterial	attribute;

	cUnknownClass		*Texture;		// только анимированные текстуры
//	cUnknownClass		*BumpMap;

	sColor4f			MulCol;			// мультипликативная компонента цвета
	sColor4f			AddCol;			// аддитивная компонента цвета
public:
	cObjectMaterial():cUnknownClass(KIND_OBJ_MATERIAL)	{ Texture=0; attribute=ATTRIBUTE_MATERIAL_NULL; MulCol.set(1,1,1,1); AddCol.set(0,0,0,1); }
	cObjectMaterial(cObjectMaterial *Material)			{ attribute=Material->attribute; if(Texture=Material->Texture) Texture->IncRef(); MulCol=Material->MulColor(); AddCol=Material->AddColor(); }
	~cObjectMaterial()									{ Release(); }
	void Release();

	void DelTexture(cUnknownClass *Texture);
	void SetTexture(cUnknownClass *Texture,int number=0);
	void SetAttribute(int Attribute)					{ attribute=eAttributeMaterial(Attribute); }
	int GetAttribute(int Attribute)						{ return attribute&Attribute; }
	int GetAttribute()									{ return attribute; }

	inline sColor4f& MulColor()							{ return MulCol; }
	inline sColor4f& AddColor()							{ return AddCol; }

	inline void SetMulColor(sColor4f& color);
	inline void SetAddColor(sColor4f& color);
};

#endif //__OBJMATERIAL_H__