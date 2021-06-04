#ifndef _PARAMPARSE_H_
#define _PARAMPARSE_H_

#define _NOSTD_
#include "ParamBlock.h"
#include "BitSet.h"
#include <vector>

class MemBlock
{
public:
	BYTE* p;
	int size;
	int truesize;
	int addsize;
public:
	MemBlock(int _addsize=1024):
	  addsize(_addsize),p(NULL),size(0),truesize(0){}
	~MemBlock(){if(p)free(p);}

	void add(const void* buf,int sz)
	{
		int oldsize=size;
		size+=sz;

		if(size>truesize)
		{
			truesize+=max(addsize,sz);
			p=(BYTE*)(p?realloc(p,truesize):malloc(truesize));
		}

		memcpy(p+oldsize,buf,sz);
	}

	inline int tell(){return size;}
	inline BYTE* operator [](int n){return p+n;}
};

/*
	throw char*; если функция некорректно завершилась
	return true если обработали эту переменную
*/
typedef bool (*NonStaticByConst)(LPCSTR name,LPCSTR value,int& ret);

class ParseParam
{
	char* text;
	BitSet b_char,space;

	NonStaticByConst pGetIntByConstNonstatic;
public:
	ParamBlock* pGetIntByConst;//какие значения может принимать этот тип
	///Эти параметры пока не используются
	ParamBlock* pGetType;//какой тип у этой переменной

	int line;
	char* cur;

	ParseParam(char* _text,ParamBlock* _pRules,
				NonStaticByConst _pGetIntByConstNonstatic);
	inline bool IsSpace(char c)
	{
		if(c==10)line++;
		return space.In(c);
	}

	void SkipSpace();

	void ReadName(char* buf);

	bool IsInt();
	bool IsFloat();
	bool IsString();
	bool IsFArray();
	bool IsConstant();

	int ReadInt();
	float ReadFloat();
	void ReadString(char* buf);
	P_FArray* ReadFArray();
	int ReadConstant(LPCSTR name,ParamBlock* pGetType);
};

class OneBlock
{
public:
	string name;
	OneParam::TYPE type;

	int ivalue;
	float fvalue;
	string svalue;
	P_FArray* favalue;

	vector<OneBlock*> child;
	OneBlock* parent;

	ParseParam& pp;

	OneBlock(ParseParam& _pp,OneBlock* _parent);
	~OneBlock();

	void Process();

	void Save(MemBlock& m);
protected:
	void Read();
	void ReadBlock();

	ParamBlock* pGetType;
	bool bconst_read;
};


/*	return error
*/
char* ParseText(const char* text,ParamBlock** paramblock,int* size,
				ParamBlock* pRules,
				NonStaticByConst pGetIntByConstNonstatic=NULL);

char* ParseRules(const char* text,ParamBlock** paramblock,
				 int* size,ParamBlock* pRules);

#endif  _PARAMPARSE_H_
