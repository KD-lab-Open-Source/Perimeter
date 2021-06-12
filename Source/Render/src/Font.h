#ifndef _FONT_H_
#define _FONT_H_
#pragma once

class cFontInternal : public cUnknownClass
{
public:
	cFontInternal(int kind=KIND_FONT);
	virtual ~cFontInternal();

	bool Create(LPCSTR root_dir,LPCSTR language_dir,LPCSTR fname,int h,bool silentErr=0);
	
	virtual cTexture* GetTexture()		{return pTexture;};
	virtual float GetHeight()			{return FontHeight*GetTexture()->GetHeight();}
	bool Reload(LPCSTR root_dir,LPCSTR language_dir);

	vector<Vect3f>		Font; // x,y - position, z - font width
	float				FontHeight;

	string font_name;
	int GetStatementHeight(){return statement_height;};
protected:
	int statement_height;
	cTexture* pTexture;
	friend class cFont;
	
	bool CreateTexture(LPCSTR fontname,LPCSTR fname,int height);
	bool CreateImage(LPCSTR filename,LPCSTR fontname,int height,class cFontImage* image);
	bool Save(const char* fname,cFontImage& fnt);
	bool Load(const char* fname,cFontImage& fnt);
};

class cFont : public cUnknownClass
{
public:
	cFont(cFontInternal* p):cUnknownClass(KIND_FONT),pFont(p),Scale(1,1){};
	virtual ~cFont(){};

	virtual cTexture* GetTexture(){return pFont->pTexture;};
	virtual float GetHeight()	  {return pFont->GetHeight(); }

	virtual Vect2f GetScale() const     { return Scale; }
	virtual void SetScale(const Vect2f& scale){Scale=scale; }

	inline cFontInternal* GetInternal(){return pFont;}
protected:
	Vect2f Scale;
	cFontInternal* pFont;
};


#endif //_FONT_H_