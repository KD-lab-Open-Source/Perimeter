#include "stdafx.h"
#include "RunTimeCompiler.h"

class BaseParameterList : public vector<BaseParameter> {};
class DependencyList : public list<const char*> {};


RunTimeCompiler::RunTimeCompiler()
{
	exe_path = get_exe_path();
}

void RunTimeCompiler::addSection(ParameterSection* prm)
{ 
	if(find(files.begin(), files.end(), prm->fname()) == files.end())
		files.push_back(prm->fname());
	push_back(prm); 
	loadSection(prm);
}

void RunTimeCompiler::saveAndCompileAll()
{
	iterator i;
	FOR_EACH(*this, i)
		loadSection(*i);
}

void RunTimeCompiler::loadSection(ParameterSection* prm)
{
	try {
		if(!prm->needToReload())
			return;

		Compiler::clear();

		Section* sec = getSection(prm->name);
		if(!sec){
			XBuffer bout(1024, 1);
			if(parse_file(prm->fname(), bout))
				throw logic_error((const char*)bout);
			else
				sec = getSection(prm->name);
			}
		sec->copy(*prm);
	}
	catch(logic_error exc) {
		XBuffer buf(512);
		buf < "Ошибка в скриптах: \r\n" < (char*)exc.what() < "\r\n";
		MessageBox ( 0, buf, "XPrm error",  MB_TASKMODAL | MB_SETFOREGROUND | MB_OK | MB_ICONERROR);
	}
}

//////////////////////////////////////////////////////////////////////////////////
//		Section
//////////////////////////////////////////////////////////////////////////////////
void Section::copy(ParameterSection& prm)
{
	if(description() != prm.description)
		throw logic_error(string("Structure of section \"") + prm.name + "\" was changed: exit and rebuild.");
	BaseParameterList::iterator i;
	FOR_EACH(prm.parameters, i){
		const Variable* var = dynamic_cast<const Variable*>(find(i->name));
		if(var && var->definible())
			var->copy_value(i->value);
		}
}

//////////////////////////////////////////////////////////////////////////////////
//		Compiler
//////////////////////////////////////////////////////////////////////////////////
struct findSectionByName 
{ 
	string name2;
	findSectionByName(const char* name) : name2(name){}
	bool operator()(const ShareHandle<Section>& s1) const { return s1->name() == name2; } 
};
Section* Compiler::getSection(const char* name)
{
	SectionList::iterator i = find_if(sections.begin(), sections.end(), findSectionByName(name));
	return i != sections.end() ? &**i : 0;
}

//////////////////////////////////////////////////////////////////////////////////
//		ParameterSection
//////////////////////////////////////////////////////////////////////////////////
ParameterSection::ParameterSection(const char* name_) 
: name(name_), 
parameters(*new BaseParameterList),
dependencies(*new DependencyList)
{}

ParameterSection::~ParameterSection()
{
	delete &parameters;
	delete &dependencies;
}

const char* ParameterSection::fname() const
{
	return dependencies.front();
}

void ParameterSection::reserve(int size)
{
	parameters.reserve(size);
}

void ParameterSection::add(void* val, const char* name)
{ 
	parameters.push_back(BaseParameter(val, name)); 
}

void ParameterSection::add_dependency(const char* fname)
{ 
	dependencies.push_back(fname); 
}

bool ParameterSection::needToReload() const
{
	unsigned int crc = 83838383;
	DependencyList::const_iterator i;
	FOR_EACH(dependencies, i)
		crc = Parser(*i).CRC(crc);

	return crc != sourceCRC;
}


//////////////////////////////////////////////////////////////////////////////////
//		Static symbol
//////////////////////////////////////////////////////////////////////////////////
static RunTimeCompiler& getRunTimeCompiler()
{
	static RunTimeCompiler compiler;
	return compiler;
}
	
void ParameterSection::add_parameter_section(ParameterSection* section)
{
	getRunTimeCompiler().addSection(section);
}

FARPROC getPrmEditDLLFunction(const char* name)
{
	static HINSTANCE lh;
	if(!lh){
		static bool showError = true;
		lh = LoadLibrary("PrmEdit.dll");
		if(!lh && showError){
			showError = false;
			XBuffer buf;
			buf < "PrmEdit.dll not found, error code: " <= GetLastError();
			MessageBox ( 0, buf, "PrmEdit error",  MB_TASKMODAL | MB_SETFOREGROUND | MB_OK | MB_ICONERROR);
			}
		}
	if(lh){
		FARPROC pf = GetProcAddress( lh, name);
		xassert("Inconsistent version of PrmEdit.dll" && pf);
		return pf;
		}
	return 0;
}
 
void edit_parameters()
{
	typedef int (*PF)(const StringList&);
	PF p = (PF)getPrmEditDLLFunction("edit_parameters");
	if(p){
		p(getRunTimeCompiler().getFilesList());
		getRunTimeCompiler().saveAndCompileAll();
		}
}

bool reload_parameters()
{
	if(MessageBox(0, "Edit scripts by external editor, than press YES to reload", "XPrm edit & reload", MB_YESNO) == IDYES){
		getRunTimeCompiler().saveAndCompileAll();
		return true;
	}
	else
		return false;
}


static int window_shown;
void show_debug_window(const char* text, int sx, int sy)
{
	typedef int (*PF)(const char* text, int sx, int sy);
	PF p = (PF)getPrmEditDLLFunction("show_debug_window");
	if(p){
		p(text, sx, sy);
		window_shown = 1;
		}
}

void hide_debug_window()
{
	if(!window_shown)
		return;
	typedef int (*PF)();
	PF p = (PF)getPrmEditDLLFunction("hide_debug_window");
	if(p){
		p();
		window_shown = 0;
		}
}

void show_profile(const char* text)
{
	typedef int (*PF)(const char* text);
	PF p = (PF)getPrmEditDLLFunction("show_profile");
	if(p)
		p(text);
}
