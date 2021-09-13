#ifndef __PRM_EDIT_H__
#define __PRM_EDIT_H__

#ifndef PERIMETER_DEBUG

inline void edit_parameters(){}
inline bool reload_parameters(){ return false; }

#else // PERIMETER_DEBUG

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

public:
	const char* name;
	void* value;
    unsigned int crc;

	BaseParameter(void* value_, const char* name_, unsigned int crc_) : name(name_), value(value_), crc(crc_) {}
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
	void add(void* val, const char* name, unsigned int crc = 0);
	void add_dependency(const char* fname);
	const char* fname() const;
	bool needToReload() const;

	static void add_parameter_section(ParameterSection* section); 
};

#endif // PERIMETER_DEBUG
#endif // __PRM_EDIT_H__
