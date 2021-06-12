// TODO: change encoding to utf-8

#include "StdAfx.h"
#include "Parse/ParamParse.h"
#include "MessageBlock.h"

static vector<string> stringID;

ParamBlock* ReadRules(char* rul,int* psize,ParamBlock* pRulesX)
{
	int f=_open( rul, _O_RDONLY |_O_BINARY);
	if(f==-1)
	{
		message_printf("Не могу открыть %s",rul);
		exit(1);
		return NULL;
	}

	int size=_lseek(f,0,SEEK_END);_lseek(f,0,SEEK_SET);
	char* text=new char[size+1];
	text[size]=0;
	_read(f,text,size);
	_close(f);

	ParamBlock* paramblock;
	int sizeparam;
	char* error=ParseRules(text,&paramblock,&sizeparam,pRulesX);
	if(error)
	{
		message_printf("file=%s\n%s",rul,error);
		exit(1);
		return NULL;
	}

	delete text;

	if(psize)
		*psize=sizeparam;
	return paramblock;
}

ParamBlock* ReadRulesRes(char* rul,int* psize,ParamBlock* pRulesX)
{
	HRSRC hrsrc=FindResource(NULL,rul,"RUL");
	HGLOBAL hglobal=hrsrc?LoadResource(NULL,hrsrc):NULL;
	if(hrsrc==NULL || hglobal==NULL)
	{
		message_printf("Не могу найти ресурс %s.rul",rul);
		exit(1);
		return NULL;
	}


	int size=SizeofResource(NULL,hrsrc);
	char* text=new char[size+1];
	text[size]=0;
	memcpy(text,LockResource(hglobal),size);

	ParamBlock* paramblock;
	int sizeparam;
	char* error=ParseRules(text,&paramblock,&sizeparam,pRulesX);
	if(error)
	{
		message_printf("file=%s\n%s",rul,error);
		exit(1);
		return NULL;
	}

	delete text;

	if(psize)
		*psize=sizeparam;
	return paramblock;
}


void InitStringID(bool beng,bool beginner)
{
	int sizeparam,sizeparam_m_rul;

	ParamBlock* m_rul=ReadRules("message.rul",&sizeparam_m_rul);
	LPSTR tname=beng?
		(beginner?"message_beginner_e.txt":"message_e.txt"):
		(beginner?"message_beginner.txt":"message.txt");
	ParamBlock* pp=ReadRules(tname,&sizeparam,m_rul);

	for(int i=0;i<pp->GetSize();i++)
	{
		OneParam* op=(*pp)[i];
		string s=op->GetName();
		stringID.push_back(s);
	}

	LPCSTR outname=beng?
		(beginner?"message_beginner_e.dat":"message_e.dat"):
		(beginner?"message_beginner.dat":"message.dat");
	int f=_open( outname, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 
                      _S_IREAD | _S_IWRITE );
	if(f==-1)
	{
		message_printf("Не могу записать %s",outname);
		exit(1);
	}

	_write(f,pp,sizeparam);
	_close(f);

	delete pp;
}

bool StringIDByConst(LPCSTR name,LPCSTR value,int& ret)
{
	if(name[0]!='T' || name[1]!='E' || 
	   name[2]!='X' || name[3]!='T')
	{
		return false;
	}

	for(int i=0;i<stringID.size();i++)
	{
		LPCSTR ss=stringID[i].c_str();
		if(strcmp(ss,value)==0)
		{
			ret=i;
			return true;
		}
	}

	throw "Не могу найти строковый индентификатор";
	return false;
}
