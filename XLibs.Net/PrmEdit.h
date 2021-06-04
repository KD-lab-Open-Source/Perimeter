#ifndef __PRM_EDIT_H__
#define __PRM_EDIT_H__

#ifdef _FINAL_VERSION_

inline void edit_parameters(){}
inline bool reload_parameters(){ return false; }

#else // _FINAL_VERSION_

#define _PRM_EDIT_

/////////////////////////////////////////////////////////////////////////////////////////////////////
//					API
/////////////////////////////////////////////////////////////////////////////////////////////////////
void edit_parameters(); 
bool reload_parameters(); 

/////////////////////////////////////////////////////////////////////////////////////////////////////
//				Parameters
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BaseParameter
{
	friend class ParameterSection;
	friend class Section;

	const char* name;
	void* value;

public:
	BaseParameter(void* value_, const char* name_) : name(name_), value(value_) {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//					Dispatchers
/////////////////////////////////////////////////////////////////////////////////////////////////////
class ParameterSection
{
	friend class RunTimeCompiler;
	friend class Section;

	class BaseParameterList& parameters;
	class DependencyList& dependencies;

protected:

	const char* name;
	unsigned long description;
	unsigned int sourceCRC;

	ParameterSection(const char* name_);
	~ParameterSection();
	void reserve(int size);
	void add(void* val, const char* name);
	void add_dependency(const char* fname);
	const char* fname() const;
	bool needToReload() const;

	static void add_parameter_section(ParameterSection* section); 
};


///////////////////////////////////
//		Automatic linking
///////////////////////////////////

#if defined(_MT)
#define _MT_SUFFIX "Mt"
#elif defined(_MTD)
#define _MT_SUFFIX "MtDll"
#else
#define _MT_SUFFIX 
#endif//_MT

#ifdef _DEBUG
#define _DEBUG_SUFFIX "Dbg"
#else //_DEBUG
#define _DEBUG_SUFFIX 
#endif //_DEBUG

#if (_MSC_VER == 1200)

#define _VC_SUFFIX ""

#elif (_MSC_VER >= 1300)

#define _VC_SUFFIX "VC7"

#endif //(_MSC_VER >= 1300)

#define _LIB_NAME "prmedit" _VC_SUFFIX _DEBUG_SUFFIX _MT_SUFFIX ".lib"
#pragma message("Automatically linking with " _LIB_NAME) 
#pragma comment(lib, _LIB_NAME) 

#undef _VC_SUFFIX
#undef _DEBUG_SUFFIX
#undef _MT_SUFFIX
#undef _LIB_NAME

#endif // _FINAL_VERSION_
#endif // __PRM_EDIT_H__
