#ifndef _PARAMBLOCK_H_//Чтение параметров
#define _PARAMBLOCK_H_
#include <stdio.h>

#ifndef _NOSTD_
#include <strstream>
#endif//_NOSTD_

#pragma warning(disable:4200)
#pragma pack(1)

class ParamBlock;
struct P_FArray
{
	int size;
	float p[];
};

class OneParam
{
public:
	enum TYPE
	{
		P_NULL=0,
		P_INT,
		P_FLOAT,
		P_STRING,
		P_PARAMBLOCK,
		P_FARRAY,
	} type;
	char name[];
public:
	inline char* GetName(){return name;};
	inline void* GetValue(){for(char* p=name;*p++;);return p;};
	inline int GetInt(){ASSERT(type==P_INT); return *(int*)GetValue();};
	inline float GetFloat()
	{
		if(type==P_INT) return (float)GetInt();
		ASSERT(type==P_FLOAT); return *(float*)GetValue();
	}
	inline char* GetString(){ASSERT(type==P_STRING);return (char*)GetValue();}
	inline ParamBlock* GetBlock(){ASSERT(type==P_PARAMBLOCK);return (ParamBlock*)GetValue();}
	inline P_FArray* GetFArray(){ASSERT(type==P_FARRAY);return (P_FArray*)GetValue();}

#ifndef _NOSTD_
	void Print(std::ostream& o,char* sp);
#endif//_NOSTD_
};


class ParamBlock
{
	int n;
	int offset[];
public:
	inline int GetSize(){return n;}
	inline OneParam* operator[](int i){return (OneParam*)(((BYTE*)this)+offset[i]);}
	inline OneParam* Find(const char* name);

	inline char* GetS(const char* name);
	inline float GetF(const char* name);
	inline int GetI(const char* name);
	inline P_FArray* GetFA(const char* name);
	inline ParamBlock* GetBlock(const char* name);
#ifndef _NOSTD_
	void Print(std::ostream& o,char* sp="");
#endif//_NOSTD_
};

inline bool streq(const char* s1,const char* s2)
{
	while(*s1 && *s2)
	{
		if(*s1!=*s2)return false;
		s1++;s2++;
	}

	return *s1==*s2;
}

char* ParamBlock::GetS(const char* name)
{
	OneParam* p=Find(name);
	if(p==NULL)return NULL;
	return p->GetString();
}

float ParamBlock::GetF(const char* name)
{
	OneParam* p=Find(name);
	ASSERT(p);
	return p->GetFloat();
}

int ParamBlock::GetI(const char* name)
{
	OneParam* p=Find(name);
	ASSERT(p);
	return p->GetInt();
}

P_FArray* ParamBlock::GetFA(const char* name)
{
	OneParam* p=Find(name);
	ASSERT(p);
	return p->GetFArray();
}

ParamBlock* ParamBlock::GetBlock(const char* name)
{
	OneParam* p=Find(name);
	if(p==NULL)return NULL;
	return p->GetBlock();
}

OneParam* ParamBlock::Find(const char* name)
{
	for(int i=0;i<n;i++)
	{
		OneParam* p=(*this)[i];
		char* pname=p->GetName();

		//if(streq(name,pname))return p;

		{
			const char* s1=name;
			while(*s1 && *pname && *s1!='.')
			{
				if(*s1!=*pname)break;
				s1++;pname++;
			}

			if(*s1==0 && *pname==0)return p;
			if(*s1=='.' && *pname==0)
			{
				ParamBlock* pb=p->GetBlock();
				return pb->Find(s1+1);
			}
		}

	}
	return NULL;
}

#ifndef _NOSTD_
inline void OneParam::Print(std::ostream& o,char* sp)
{
	char word[MAXSTRLEN];
	if(type==P_NULL)
	{
		sprintf(word,"%s;\n",name);
		o<<sp<<word;
	}else
	if(type==P_STRING)
	{
		sprintf(word,"%s=\"%s\";\n",name,GetString());
		o<<sp<<word;
	}else
	if(type==OneParam::P_INT)
	{
		sprintf(word,"%s=%i;\n",name,GetInt());
		o<<sp<<word;
	}else
	if(type==OneParam::P_FLOAT)
	{
		sprintf(word,"%s=%f;\n",name,GetFloat());
		o<<sp<<word;
	}else
	if(type==OneParam::P_PARAMBLOCK)
	{
		ParamBlock* p=GetBlock();

		o<<sp<<name<<"{\n";
		p->Print(o,sp);

		o<<sp<<"};\n";
	}else
	if(type==OneParam::P_FARRAY)
	{
		P_FArray* p=GetFArray();
		sprintf(word,"%s%s=(",sp,name);
		o<<word;

		for(int i=0;i<p->size;i++)
		{
			if(i)o<<",";

			o<<p->p[i];
		}

		o<<");\n";
	}else
		ASSERT(0);

}

inline void ParamBlock::Print(std::ostream& o,char* sp)
{
	char sp1[MAXSTRLEN];
	sprintf(sp1,"%s  ",sp);

	for(int i=0;i<n;i++)
	{
		(*this)[i]->Print(o,sp1);
	}
}
#endif//_NOSTD_

#pragma warning(default:4200)
#pragma pack()
#endif  _PARAMBLOCK_H_