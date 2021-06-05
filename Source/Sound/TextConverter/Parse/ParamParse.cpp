// TODO: change encoding to utf-8

#include "stdafx.h"
#include "ParamParse.h"
#include <stdio.h>

static char error[2048];
const offset_enum=1000;

void ParseParam::SkipSpace()
{
Retry:
	while(IsSpace(*cur))cur++;
	if(*cur=='/' && cur[1]=='/')
	{
		while(*cur && *cur!=0xD && *cur!=0xA)
			cur++;

		while(*cur && *cur==0xD && *cur==0xA)
		{
			if(*cur==10)
				line++;
			cur++;
		}

		if(*cur)goto Retry;
	}

	if(*cur=='/' && cur[1]=='*')
	{
		cur+=2;
		while(*cur && !(*cur=='*' && cur[1]=='/'))
		{
			if(*cur==10)
				line++;
			cur++;
		}

		if(*cur)
		{
			cur+=2;
			goto Retry;
		}
	}

}


ParseParam::ParseParam(char* _text,ParamBlock* _pRules,
					   NonStaticByConst _pGetIntByConstNonstatic)
:text(_text),line(1)
{
	b_char.Set('a','z');
	b_char.Set('A','Z');
	b_char.Set('_');

	space.Set((BYTE)32);
	space.Set((BYTE)9);
	space.Set((BYTE)10);
	space.Set((BYTE)13);

	cur=text;

	pGetIntByConst=NULL;
	pGetType=NULL;

	if(_pRules)
	{
		OneParam* op;
		op=_pRules->Find("enum");
		pGetIntByConst=op?op->GetBlock():NULL;

		op=_pRules->Find("type");
		pGetType=op?op->GetBlock():NULL;
	}

	pGetIntByConstNonstatic=_pGetIntByConstNonstatic;
}

void ParseParam::ReadName(char* buf)
{
	char* p=buf;
	SkipSpace();

	if(IsString())
	{
		ReadString(buf);
		return;
	}

	if(b_char.In(*cur))
		*p++=*cur++;

	while(b_char.In(*cur) || (*cur>='0' && *cur<='9'))
		*p++=*cur++;

	*p=0;

	if(*buf==0)
		throw "Невозможно прочитать имя объекта";
}

bool ParseParam::IsInt()
{
	char* p=cur;

	if(*p=='+' || *p=='-')
		p++;

	if(!(*p>='0' && *p<='9'))
		return false;

	while(*p>='0' && *p<='9')
		p++;

	return true;

}

bool ParseParam::IsFloat()
{
	char* p=cur;
	if(*p=='+' || *p=='-')
		p++;

	if(!(*p>='0' && *p<='9'))
		return false;

	while(*p>='0' && *p<='9')
		p++;

	if(*p=='.')
		return true;

	return *p=='e' || *p=='E';
}

bool ParseParam::IsString()
{
	return *cur=='"';
}

bool ParseParam::IsFArray()
{
	return (*cur=='F')&&(cur[1]=='(');
}


bool ParseParam::IsConstant()
{
	return (pGetIntByConst!=NULL || pGetIntByConstNonstatic!=NULL) 
		&& b_char.In(*cur);
}

int ParseParam::ReadConstant(LPCSTR name,ParamBlock* pGetType)
{
	char val[MAXSTRLEN];
	char* v=val;
	
	while(*cur && *cur!=';' && !IsSpace(*cur))
		*v++=*cur++;
	*v=0;


	if(pGetIntByConstNonstatic)
	{
		int ret;
		if(pGetIntByConstNonstatic(name,val,ret))
			return ret;
	}

	if(pGetIntByConst==NULL || pGetType==NULL)
		throw "Нет блока констант";

	OneParam* pconst=NULL;
	//Ищем, какой enum сопоставлен этому индентификатору
	for(int i=0;i<pGetType->GetSize();i++)
	{
		OneParam* op=(*pGetType)[i];

		if(op->type==OneParam::P_INT)
		if(strcmp(name,op->GetName())==0)
		{
			int n=op->GetInt();
			if(n>=offset_enum)
			{
				pconst=(*pGetIntByConst)[op->GetInt()-offset_enum];
				break;
			}
		}
	}

	if(pconst==NULL)
	{
		sprintf(error,"Для %s нет сопоставленных констант\n",name);
		throw error;
	}

	ParamBlock* pb=pconst->GetBlock();
	if(pb==NULL)
		throw "Внутренняя ошибка программы - неправильный блок констант";

	OneParam* op=pb->Find(val);
	if(op==NULL)
	{

		int n=sprintf(error,"Для %s=%s нет сопоставленных констант\n"
			"Известные константы:\n",name,val);

		for(int i=0;i<pb->GetSize();i++)
		{
			OneParam* op=(*pb)[i];
			if(op->type==OneParam::P_INT)
				n+=sprintf(error+n,"%s\n",op->GetName());
		}

		throw error;
	}

	if(op->type!=OneParam::P_INT)
		throw "Внутренняя ошибка программы - неправильный блок констант";

	return op->GetInt();
}

int ParseParam::ReadInt()
{
	char buf[MAXSTRLEN];
	char* p=buf;
	if(*cur=='+' || *cur=='-') *p++=*cur++;
	while(*cur>='0' && *cur<='9')
	{
		*p++=*cur++;
	}
	*p=0;

	return atoi(buf);
}

float ParseParam::ReadFloat()
{
	char buf[MAXSTRLEN];
	char* p=buf;

	if(*cur=='+' || *cur=='-') *p++=*cur++;
	while(*cur>='0' && *cur<='9')*p++=*cur++;
	
	if(*cur=='.')
	{
		*p++=*cur++;
		while(*cur>='0' && *cur<='9')*p++=*cur++;
	}

	if(*cur=='e' || *cur=='E')
	{
		*p++=*cur++;
		if(*cur=='+' || *cur=='-') *p++=*cur++;
		while(*cur>='0' && *cur<='9')*p++=*cur++;
	}

	*p=0;

	return (float)atof(buf);
}

void ParseParam::ReadString(char* buf)
{
	if(*cur!='"')
		throw "Требуется \"";
	cur++;

	for(;*cur && *cur!='"';cur++)
		if(*cur!=10)
			*buf++=*cur;
		else
		{
			line++;
		}

	if(*cur!='"')
		throw "Требуется \"";
	cur++;
	*buf=0;
}

P_FArray* ParseParam::ReadFArray()
{
	cur+=2;//skip F(

	vector<float> ar;

	while(1)
	{
		SkipSpace();
		if(IsFloat())
			ar.push_back(ReadFloat());
		else
		if(IsInt())
			ar.push_back(ReadInt());
		else
			break;


		SkipSpace();
		if(*cur==',')
		{
			cur++;
			continue;
		}

		if(*cur==')')
		{
			P_FArray *p=(P_FArray*)malloc(
				sizeof(int)+sizeof(float)*ar.size());
			p->size=ar.size();
			for(int i=0;i<p->size;i++)
			{
				p->p[i]=ar[i];
			}

			cur++;
			return p;
		}

		throw "Требуется , или )";
	}
	
	throw "Требуется float";
	return NULL;
}

//////////////////OneBlock////////////////////
OneBlock::OneBlock(ParseParam& _pp,OneBlock* _parent)
:pp(_pp),parent(_parent),favalue(NULL)
{
	bconst_read=false;
	pGetType=NULL;
}

OneBlock::~OneBlock()
{
	vector<OneBlock*>::iterator it;
	FOR_EACH(child,it)
		delete *it;
	child.clear();

	if(favalue)
		free(favalue);
}

void OneBlock::Process()
{
	if(parent)
	{
		Read();
	}else
	{
		pp.SkipSpace();
		ReadBlock();
	}
}

void OneBlock::Read()
{
	char word[MAXSTRLEN];
	pp.ReadName(word);
	name=word;

	pp.SkipSpace();

	if(*pp.cur=='{')
	{
		pp.cur++;
		ReadBlock();
	}else
	if(*pp.cur=='=')
	{
		pp.cur++;
		pp.SkipSpace();

		if(pp.IsString())
		{
			type=OneParam::P_STRING;
			pp.ReadString(word);
			svalue=word;
		}else
		if(pp.IsFloat())
		{
			type=OneParam::P_FLOAT;
			fvalue=pp.ReadFloat();
		}else
		if(pp.IsInt())
		{
			type=OneParam::P_INT;
			ivalue=pp.ReadInt();
		}else
		if(pp.IsFArray())
		{
			type=OneParam::P_FARRAY;
			favalue=pp.ReadFArray();
		}else
		if(pp.IsConstant())
		{
			type=OneParam::P_INT;
			ivalue=pp.ReadConstant(name.c_str(),parent?parent->pGetType:pp.pGetType);
			bconst_read=true;
		}else
			throw "Непонятное значение";
	
		pp.SkipSpace();

	}else
	if(*pp.cur==';')
	{
		type=OneParam::P_NULL;
	}else
		throw "Требуется = или {";

	{//Проверка, правильный ли тип
		ParamBlock* parent_type=parent?parent->pGetType:pp.pGetType;
		if(parent_type && type!=OneParam::P_PARAMBLOCK)
		{
			bool ok=false;
			for(int i=0;i<parent_type->GetSize();i++)
			{
				OneParam* op=(*parent_type)[i];

				if(op->type==OneParam::P_INT)
				if(op->GetName()==name)
				if(op->GetInt()==type || 
					(op->GetInt()==OneParam::P_FLOAT &&
						type==OneParam::P_INT)||
					(op->GetInt()>=offset_enum && 
					type==OneParam::P_INT && bconst_read))
				{
					ok=true;
					break;
				}
			}

			if(!ok)
			{
				LPSTR textend=error+sprintf(error,"Неверный тип, правильный тип:");
				LPSTR begin=textend;

				for(int i=0;i<parent_type->GetSize();i++)
				{
					OneParam* op=(*parent_type)[i];

					if(op->type==OneParam::P_INT)
					if(op->GetName()==name)
					switch(op->GetInt())
					{
					case OneParam::P_NULL:
						textend=textend+sprintf(textend,"null ");
						break;
					case OneParam::P_INT:
						textend=textend+sprintf(textend,"int ");
						break;
					case OneParam::P_FLOAT:
						textend=textend+sprintf(textend,"float ");
						break;
					case OneParam::P_STRING:
						textend=textend+sprintf(textend,"string ");
						break;
					case OneParam::P_FARRAY:
						textend=textend+sprintf(textend,"farray ");
						break;
					default:
						if(pp.pGetIntByConst)
						{
							int n=op->GetInt()-offset_enum;
							if(n<0 || n>=pp.pGetIntByConst->GetSize())
								throw "Некорректное значение константы";
							textend=textend+sprintf(textend,(*pp.pGetIntByConst)[n]->GetName());
						}else
							throw "Нет блока констант";
					}
				}

				if(begin==textend)
					sprintf(error,"Переменная неопределённая в правилах");

				throw error;
			}
		}
	}

	if(*pp.cur!=';')
		throw "Здесь необходима ;";
	*pp.cur++;
}

void OneBlock::ReadBlock()
{
	type=OneParam::P_PARAMBLOCK;

	if(parent)
	{//Поиск pGetType
		if(parent->pGetType)
		{
			for(int i=0;i<parent->pGetType->GetSize();i++)
			{
				OneParam* op=(*parent->pGetType)[i];

				if(op->type==OneParam::P_PARAMBLOCK)
				if(op->GetName()==name)
				{
					pGetType=op->GetBlock();
				}
			}

			if(pGetType==NULL)
			{
				OneParam* op=parent->pGetType->Find("*");
				if(op && op->type==OneParam::P_PARAMBLOCK)
					pGetType=op->GetBlock();
			}
			if(pGetType==NULL)
			{
				sprintf(error,"Не могу найти типизацию для блока %s",name.c_str());
				throw error;
			}
		}
	}else
		pGetType=pp.pGetType;


	while(*pp.cur)
	{
		pp.SkipSpace();
		if(*pp.cur==0)break;
		if(*pp.cur=='}')
		{
			pp.cur++;
			return;
		}

		OneBlock* p=new OneBlock(pp,this);
		p->Process();
		child.push_back(p);
	}
	
	if(parent!=NULL)
		throw "Здесь необходима }";
}

#define WR(x) m.add(&x,sizeof(x));

void OneBlock::Save(MemBlock& m)
{
	int t=m.tell();
	if(parent)
	{
		WR(type);
		m.add(name.c_str(),name.length()+1);
	}

	if(type==OneParam::P_NULL)
	{
	}else
	if(type==OneParam::P_STRING)
	{
		m.add(svalue.c_str(),svalue.length()+1);
	}else
	if(type==OneParam::P_INT)
	{
		WR(ivalue);
	}else
	if(type==OneParam::P_FLOAT)
	{
		WR(fvalue);
	}else
	if(type==OneParam::P_PARAMBLOCK)
	{
		int n=child.size();
		int tt=m.tell();
		WR(n);
		int offset=m.tell();
		for(int i=0;i<n;i++)
		{
			WR(n);//Скипуем offset
		}

		for(i=0;i<n;i++)
		{
			int* offseti=(int*)m[offset+i*sizeof(int)];
			*offseti=m.tell()-tt;
			child[i]->Save(m);
		}
	}else
	if(type==OneParam::P_FARRAY)
	{
		WR(favalue->size);
		for(int i=0;i<favalue->size;i++)
		{
			WR(favalue->p[i]);
		}
	}else
		ASSERT(0);
}

//////////////////////////////////////
char* ParseText(const char* text,ParamBlock** paramblock,int* size,ParamBlock* pRules,
				NonStaticByConst pGetIntByConstNonstatic)
{
	if(paramblock)
		*paramblock=NULL;
	if(size)
		*size=0;

	ParseParam pp((char*)text,pRules,pGetIntByConstNonstatic);

	try 
	{
		OneBlock ob(pp,NULL);
		ob.Process();
		//ob.Print("");

		MemBlock m;
		ob.Save(m);
		if(paramblock)
		{
			*paramblock=(ParamBlock*)m.p;
			m.p=NULL;
		}

		if(size)
			*size=m.size;
	}catch(char* err)
	{
		static char word[MAXSTRLEN];
		sprintf(word,"line=%i %s\n",pp.line,err);
		return word;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////
static OneBlock *p_parent,*p_enum;
bool RulesNonStaticByConst(LPCSTR name,LPCSTR value,int& ret)
{
	if(p_enum==NULL)
	{
		if(p_parent->child.size()>0)
		{
			if(!(p_parent->child[0]->name=="enum"))
				throw "Первый блок в .rul файле должен быть enum";
			p_enum=p_parent->child[0];
		}else
			return false;
	}

	if(strcmp(value,"null")==0)
	{
		ret=OneParam::P_NULL;
		return true;
	}
	if(strcmp(value,"int")==0)
	{
		ret=OneParam::P_INT;
		return true;
	}
	if(strcmp(value,"float")==0)
	{
		ret=OneParam::P_FLOAT;
		return true;
	}
	if(strcmp(value,"string")==0)
	{
		ret=OneParam::P_STRING;
		return true;
	}
	if(strcmp(value,"farray")==0)
	{
		ret=OneParam::P_FARRAY;
		return true;
	}

	for(int i=0;i<p_enum->child.size();i++)
	{
		OneBlock* block=p_enum->child[i];
		if(block->name==value)
		{
			if(block->type!=OneParam::P_PARAMBLOCK)
				throw "Внутренняя ошибка - должен быть блок";

			ret=i+offset_enum;
			return true;
		}
	}

	throw "Не понятный тип или enum";
	return false;
}

char* ParseRules(const char* text,ParamBlock** paramblock,
				 int* size,ParamBlock* pRules)
{
	p_parent=p_enum=NULL;

	if(paramblock)
		*paramblock=NULL;
	if(size)
		*size=0;

	ParseParam pp((char*)text,pRules,RulesNonStaticByConst);

	try 
	{
		OneBlock ob(pp,NULL);
		p_parent=&ob;
		ob.Process();

		MemBlock m;
		ob.Save(m);
		if(paramblock)
		{
			*paramblock=(ParamBlock*)m.p;
			m.p=NULL;
		}

		if(size)
			*size=m.size;
	}catch(char* err)
	{
		static char word[MAXSTRLEN];
		sprintf(word,"line=%i %s\n",pp.line,err);
		return word;
	}

	return NULL;
}


