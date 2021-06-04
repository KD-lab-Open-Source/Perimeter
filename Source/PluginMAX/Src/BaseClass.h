#ifndef __BASECLASS_H__
#define __BASECLASS_H__

#include <string.h>
#include "assert.h"

template <class cBase> class cBaseNode
{
public:
	cBase	*Parent;
	cBase	*Child;
	cBase	*Sibling;

	cBaseNode()
	{
		Child=0;
		Parent=0;
		Sibling=0;
	}
	~cBaseNode()
	{
		if(Parent) Parent->DetachChild((cBase*)this);
		for(cBase *child=Child;child;)
		{
			cBase *tmp=child;
			child=child->Sibling;
			delete tmp;
		}
	}
	void AttachChild(cBase *child)
	{
		child->Parent=(cBase*)this;
		if(Child==0) Child=child;
		else 
		{ 
			cBase *tmp=Child; 
			Child=child; 
			child->Sibling=tmp; 
		}
	}
	void AttachChildEnd(cBase *child)
	{
		child->Parent=(cBase*)this;
		if(Child==0) Child=child;
		else 
		{ 
			cBase *tmp=Child; 
			while(tmp->Sibling) 
				tmp=tmp->Sibling; 
			tmp->Sibling=child; 
		}
	}
	void DetachChild(cBase *child)
	{
		child->Parent=0; 
		if(child==Child) { Child=child->Sibling; return; }
		cBase *tmp=Child;
		while((tmp->Sibling)&&(tmp->Sibling!=child))
			tmp=tmp->Sibling;
		tmp->Sibling=child->Sibling;
	}
};

template <class cBase> class cBaseArray
{
public: 
	cBase	*Base;
	int		CurrentSize,MaxSize,dSize;

	cBaseArray()									{ Base=0; CurrentSize=MaxSize=0; dSize=2; }
	cBaseArray(cBaseArray *Array)					{ Base=0; SetArray(Array); }
	cBaseArray(int MaxLength,int MulSize)			{ Base=0; New(MaxLength); CurrentSize=0; dSize=MulSize; }
	~cBaseArray()									{ Delete(); }

	inline void New(int NewSize)					{ Delete(); if((MaxSize=NewSize)!=0) Base=new cBase[MaxSize]; }
	inline void Delete()							{ if(Base) delete [] Base; Base=0; MaxSize=CurrentSize=0; }
	inline void Clear()								{ CurrentSize=0; }
	inline int& length()							{ return CurrentSize; }
	inline void Pack()								{ if(CurrentSize<MaxSize) Resize(CurrentSize,CurrentSize); }
	inline cBase& operator [] (int number)			{ /*assert(number<MaxSize);*/ return Base[number]; }
	inline cBase& operator () (int number)			{ if(number>=MaxSize) Resize(number+dSize,MaxSize); if(number>=CurrentSize) CurrentSize=number+1; return Base[number]; }
	inline void SetArray(cBaseArray *Array)			{ Delete(); dSize=Array->dSize;	Base=new cBase[MaxSize=Array->MaxSize];	for(CurrentSize=0;CurrentSize<Array->CurrentSize;CurrentSize++) Base[CurrentSize]=Array->Base[CurrentSize]; }
	inline void MovArray(cBaseArray *Array)			{ Delete(); dSize=Array->dSize;	MaxSize=Array->MaxSize; CurrentSize=Array->CurrentSize; Base=Array->Base; Array->MaxSize=0; Array->CurrentSize=0; Array->Base=0; }
	inline int AddBase(cBase *base,char test=0);
	inline int Attach(cBase &base)
	{
		if(CurrentSize>=MaxSize) Resize(MaxSize+dSize,MaxSize);
		Base[CurrentSize]=base;
		return CurrentSize++; 
	}
	inline void Detach(cBase &base)
	{
		for(int i=0;i<length();i++)
			if(Base[i]==base) 
				break;
		if(i>=length()) return;
		length()--;
		memcpy(&Base[i],&Base[i+1],(length()-i)*sizeof(cBase));
	}
	inline void Detach(int number)
	{
		if(number>=length()) return;
		length()--;
		memcpy(&Base[number],&Base[number+1],(length()-number)*sizeof(cBase));
	}
	template <class cBaseOther>
	inline cBaseArray& operator = (const cBaseArray <cBaseOther> &Array)
	{
		New(Array.length());
		for(cBase *bNew=Base,*eNew=&Base[length()],*bOld=Array.Base;bNew<eNew;bNew++,bOld++)
			*bNew=*bOld;
		return *this;
	}
	inline void Resize(int NewSize,int OldSize=-1)							
	{
		if(OldSize<0) OldSize=length();
		if(NewSize==0) { Delete(); return; }
		cBase *tmp=new cBase[MaxSize=NewSize];
		for(int i=0;i<OldSize;i++) tmp[i]=Base[i];
		if(Base) delete [] Base;
		Base=tmp;
	}
};
template <class cBase> inline int cBaseArray<cBase>::AddBase(cBase *base,char test)
{
	if(test) for(int i=0;i<length();i++) if(memcmp(&Base[i],base,sizeof(cBase))==0) return i;
	if(CurrentSize>=MaxSize) Resize(MaxSize+dSize,MaxSize);
	Base[CurrentSize]=*base;
	return CurrentSize++; 
}

template <class cBase> class cBaseArrayPointer
{
public: 
	cBase*	*Base;
	int		CurrentSize,MaxSize,dSize;

	cBaseArrayPointer()								{ Base=0; CurrentSize=MaxSize=0; dSize=1; }
	cBaseArrayPointer(cBaseArrayPointer *Array)		{ Base=0; SetArray(Array); }
	cBaseArrayPointer(int MaxLength,int AddSize=1)	{ Base=0; New(MaxLength); CurrentSize=0; dSize=AddSize; }
	~cBaseArrayPointer()							{ Delete(); }

	inline void New(int NewSize)					{ Delete(); if((MaxSize=NewSize)!=0) { Base=new cBase*[MaxSize]; memset(Base,0,MaxSize*sizeof(cBase*)); } }
	inline void Delete()							{ if(Base) { for(int i=0;i<length();i++) if(Base[i]) delete Base[i]; delete [] Base; Base=0; } MaxSize=CurrentSize=0; }
	inline void Clear()								{ CurrentSize=0; }
	inline int& length()							{ return CurrentSize; }
	inline void Pack()								{ if(CurrentSize<MaxSize) Resize(CurrentSize,CurrentSize); }
	inline cBase*& operator [] (int number)			{ /*assert(number<MaxSize);*/ return Base[number]; }
	inline cBase*& operator () (int number)			{ if(number>=MaxSize) Resize(number+dSize,MaxSize); if(number>=CurrentSize) CurrentSize=number+1; return Base[number]; }
	inline void SetArray(cBaseArrayPointer *Array)	{ Delete(); dSize=Array->dSize;	Base=new cBase*[MaxSize=Array->MaxSize]; memcpy(Base,Array->Base,sizeof(cBase*)*(CurrentSize=Array->CurrentSize)); }
	inline int AddBase(cBase *base,char test=1);
	inline void Attach(cBase *base)					{ if(CurrentSize>=MaxSize) Resize(MaxSize+dSize,MaxSize); Base[CurrentSize++]=base;	}
	inline void Detach(cBase *base)					
	{ 
		for(int number=0;number<length();number++)
			if(Base[number]==base) 
				break;
		if(number>=length()) return;
		length()--;
		memcpy(&Base[number],&Base[number+1],(length()-number)*sizeof(cBase*));
	}
	inline void Resize(int NewSize,int OldSize=-1)							
	{
		if(OldSize<0) OldSize=length();
		if(NewSize==0) { Delete(); return; }
		cBase* *tmp=new cBase*[MaxSize=NewSize];
		memcpy(tmp,Base,sizeof(cBase*)*OldSize);
		if(Base) delete [] Base;
		Base=tmp;
	}
};
template <class cBase> inline int cBaseArrayPointer<cBase>::AddBase(cBase *base,char test)
{
	if(test) for(int i=0;i<length();i++) if(Base[i]==base) return i;
	if(CurrentSize>=MaxSize) Resize(MaxSize,MaxSize+dSize);
	Base[CurrentSize]=base;
	return CurrentSize++; 
}

template <class cBase> class cBaseArrayManager : public cBaseArrayPointer <cBase>
{
public:
	cBaseArrayManager()										{ }
	cBaseArrayManager(int MaxLength,int AddSize=1):cBaseArrayPointer<cBase>(MaxLength,AddSize)	{ }
	~cBaseArrayManager()									{ assert(length()==0); }
	inline int Attach(cBase *base)							
	{ 
		for(int i=0;i<length();i++)
			if(Base[i]==0) 
			{
				Base[i]=base;
				return i;
			}
		cBaseArrayPointer<cBase>::Attach(base);
		return length()-1;
	}
	inline void Detach(cBase *base)					
	{ 
		for(int i=0;i<length();i++)
			if(Base[i]==base) 
			{
				Base[i]=0;
				break;
			}
	}
};

#define BASEDYNARRAY_DSIZE								1

template <class cBase> class cBaseDynArray
{
public: 
	cBase	*Base;
	int		size;

	cBaseDynArray()									{ Base=0; size=0; }
	cBaseDynArray(cBaseDynArray *Array)				{ Base=0; size=0; *this=*Array; }
	cBaseDynArray(int MaxLength)					{ Base=0; size=0; New(MaxLength); }
	~cBaseDynArray()								{ Delete(); }

	inline void New(int NewSize)					{ Delete(); if((size=NewSize)!=0) Base=new cBase[size]; }
	inline void New(cBase *NewBase,int NewSize)		{ Delete(); Base=NewBase; size=NewSize;	}
	inline void Delete()							{ if(Base) delete [] Base; Base=0; size=0; }
	inline int& length()							{ return size; }
	inline void Pack(int NewSize)					{ Resize(NewSize); }
	inline cBase& operator [] (int number)			{ assert(number<size); return Base[number]; }
	inline cBase& operator () (int number)			{ if(number>=size) Resize(number+BASEDYNARRAY_DSIZE); return Base[number]; }
	inline void Clear()								{ memset(Base,0,size*sizeof(cBase)); }
	inline cBaseDynArray& operator = (const cBaseDynArray &Array)
	{
		New(Array.size);
		for(cBase *bNew=Base,*eNew=&Base[length()],*bOld=Array.Base;bNew<eNew;bNew++,bOld++)
			*bNew=*bOld;
		return *this;
	}
	inline cBase* Find(cBase *base,int (*fcmp)(cBase *,cBase *))
	{
		for(cBase *b=Base,*e=&Base[length()];b<e;b++)
			if(fcmp(base,b))
				return b;
		return 0;
	}
	inline void Resize(int NewSize)							
	{
		if(NewSize==0) { Delete(); return; }
		if(size==NewSize) return;
		int OldSize=size < NewSize ? size : NewSize;
		cBase *tmp=Base; Base=new cBase[size=NewSize];
		for(int i=0;i<OldSize;i++)
			Base[i]=tmp[i];
		if(tmp) delete [] tmp;
	}
	inline int Attach(cBase &base)
	{
		Resize(size+1);
		Base[size-1]=base;
		return size; 
	}
	inline void Detach(int number)
	{ // ищет указатель элемент Base в списке и удаляет его из списка
		if(number>=length()) return;
		if(length()==1) { Delete(); return; }
		length()--;
		cBase *tmp=Base; Base=new cBase[length()];
		for(int i=0;i<number;i++)
			Base[i]=tmp[i];
		for(i=number;i<length();i++)
			Base[i]=tmp[i+1];
		if(tmp) delete [] tmp;
	}
};

template <class cBase> class cBaseDynArrayPointer
{
public: 
	cBase	**Base;
	int		size;

	cBaseDynArrayPointer()							{ Base=0; size=0; }
	cBaseDynArrayPointer(cBaseDynArrayPointer *Array){ Base=0; size=0; *this=*Array; }
	cBaseDynArrayPointer(int MaxLength)				{ Base=0; size=0; New(MaxLength); }
	~cBaseDynArrayPointer()							{ Delete(); }

	inline void New(int NewSize)					
	{ 
		Delete(); 
		if((size=NewSize)!=0) Base=new cBase*[size]; 
	}
	inline void Delete()							
	{ 
		for(int i=0;i<length();i++)
			if(Base[i]) 
				delete Base[i];
		if(Base) delete Base; 
		Base=0; size=0;
//		memset(this,0,sizeof(cBaseDynArrayPointer)); 
	}
	inline int& length()							{ return size; }
	inline void Pack(int NewSize)					{ Resize(NewSize); }
	inline cBase*& operator [] (int number)			{ assert(number<size); return Base[number]; }
	inline cBase*& operator () (int number)			{ if(number>=size) Resize(number+BASEDYNARRAY_DSIZE); return Base[number]; }
	inline void Clear()								{ for(int i=0;i<length();i++) Base[i]=0; }
	inline cBaseDynArrayPointer& operator = (const cBaseDynArrayPointer &Array)
	{
		New(Array.size);
		for(int i=0;i<length();i++)
			Base[i]=Array.Base[i];
		return *this;
	}
	inline cBase* Find(cBase *base,int (*fcmp)(cBase *,cBase *))
	{
		for(cBase **b=Base,**e=&Base[length()];b<e;b++)
			if(fcmp(base,*b))
				return *b;
		return 0;
	}
	inline void Attach(cBase *base)
	{ // добавляет указатель в конец списка
		Resize(length()+BASEDYNARRAY_DSIZE);
		Base[length()-1]=base;
	}
	inline void Detach(cBase *base)
	{ // ищет указатель элемент Base в списке и удаляет его из списка
		for(int number=0;number<length();number++)
			if(Base[number]==base) 
				break;
		if(number>=length()) return;
		if(Base[number]) Base[number]=0;
		memcpy(&Base[number],&Base[number+1],(length()-number-1)*sizeof(cBase*));
		Resize(length()-1);
	}
	inline void Delete(int number)
	{ // удаляет из списка элемент с номером number, и удаляет сам эелемент с номером
		if(Base[number]) { delete Base[number]; Base[number]=0; }
		memcpy(&Base[number],&Base[number+1],(length()-number-1)*sizeof(cBase*));
		Resize(length()-1);
	}
	inline void Resize(int NewSize)							
	{
		if(NewSize==0) { Delete(); return; }
		if(size==NewSize) return;
		int OldSize=size < NewSize ? size : NewSize;
		cBase **tmp=Base; Base=new cBase*[size=NewSize];
		for(int i=0;i<OldSize;i++)
			Base[i]=tmp[i];
		if(tmp) delete [] tmp;
	}
};

struct sAttribute
{
protected:
	int		Attribute;	
public:
	sAttribute()													{ Attribute=0; }
	
	inline int GetAttribute(int attribute=0xFFFFFFFF)	const		{ return Attribute&attribute; }
	inline void ClearAttribute(int attribute)						{ Attribute&=~attribute; }
	inline void SetAttribute(int attribute)							{ Attribute|=attribute; }

	inline void PutAttribute(int attribute,bool on){if(on)SetAttribute(attribute);else ClearAttribute(attribute);}
};

template <class cBase> class cBasePointer
{
	cBase	*Base;
public:
	cBasePointer(cBase *p=0)										{ Base=p; }
	~cBasePointer()													{ if(Base) Base->Release(); }
	__forceinline operator cBase* () const							{ return Base; }
	__forceinline cBase& operator * ()								{ return *Base;}
	__forceinline cBase* operator -> ()								{ return Base; }
	__forceinline const cBase& operator * () const					{ return *Base;}
	__forceinline const cBase* operator -> () const					{ return Base; }
	__forceinline cBasePointer& operator = (cBase* p)				{ if(Base) Base->Release(); Base=p; if(Base) Base->IncRef(); return *this; }
	__forceinline cBasePointer& operator = (cBasePointer &p)		{ if(Base) Base->Release(); Base=p.Base; if(Base) Base->IncRef(); return *this; }
	__forceinline void Set(cBase* p)								{ if(Base) Base->Release(); Base=p; if(Base) Base->IncRef(); }
	__forceinline cBase*& Get()										{ return Base; }
};

#endif //__BASECLASS_H__
