#include <SDL.h>
#include <set>
#include "stdafx.h"
#include "RunTimeCompiler.h"

class BaseParameterList : public std::vector<BaseParameter> {};
class DependencyList : public std::list<const char*> {};


RunTimeCompiler::RunTimeCompiler()
{
}

void RunTimeCompiler::addSection(ParameterSection* prm)
{ 
	if(find(files.begin(), files.end(), prm->fname()) == files.end())
		files.push_back(prm->fname());
	push_back(prm); 
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

		Compiler::reload();

		Section* sec = getSection(prm->name);
		if(!sec){
			XBuffer bout(1024, 1);
			if(parse_file(prm->fname(), bout))
				throw std::logic_error((const char*)bout);
			else
				sec = getSection(prm->name);
			}
		sec->copy(*prm);
	}
	catch(std::logic_error& exc) {
		XBuffer buf(512);
		buf < "Scripts error:\n" < (char*)exc.what() < "\n";
        
        fprintf(stderr, "XPrm runtime error: %s\n", buf.address());
		
        int err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                           "XPrm runtime error",
                                           buf,
                                           nullptr);
        if (err) {
            SDL_PRINT_ERROR("Creating error window");
        }
	}
}

//////////////////////////////////////////////////////////////////////////////////
//		Section
//////////////////////////////////////////////////////////////////////////////////
bool copy_section_var(BaseParameter* par, const Variable* var) {
    if(var && var->definible()) {
        if (par->crc) {
            unsigned int newcrc = 83838383;
            var->description(newcrc);
            if (newcrc != par->crc) {
                return false;
            }
        }
        var->copy_value(par->value);
    }
    return true;
}

void Section::copy(ParameterSection& prm)
{
	if(description() != prm.description) { 
        std::string error = std::string("Structure of section \"") + prm.name + "\" was changed: please exit and copy Scripts from Source to your game folder";
        fprintf(stderr, "%s\n", error.c_str());
		//throw std::logic_error(error);
    }
	BaseParameterList::iterator i;
    
    //First load array/structs
    std::set<std::string> skipvar;
    FOR_EACH(prm.parameters, i) {
        const ArrayVariable* arrvar = dynamic_cast<const ArrayVariable*>(find(i->name));
        if (arrvar) {
            skipvar.insert(i->name);
            bool crc = copy_section_var(&*i, arrvar);
            if (!crc) {
                //We also skip the var that controls the array size
                skipvar.insert(arrvar->get_size_var_name());
                fprintf(stderr, "Section %s array variable %s has different CRC, skipping\n", prm.name, i->name);
            }
            continue;
        }
        const StructVariable* structvar = dynamic_cast<const StructVariable*>(find(i->name));
        if (structvar) {
            skipvar.insert(i->name);
            bool crc = copy_section_var(&*i, structvar);
            if (!crc) {;
                fprintf(stderr, "Section %s struct variable %s has different CRC, skipping\n", prm.name, i->name);
            }
            continue;
        }
    }
    
	FOR_EACH(prm.parameters, i) {
        if (skipvar.count(i->name)) continue;
		const Variable* var = dynamic_cast<const Variable*>(find(i->name));
        if (var && var->definible()) {
            if (!copy_section_var(&*i, var)) {
                fprintf(stderr, "Section %s variable %s has different CRC, skipping\n", prm.name, i->name);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
//		Compiler
//////////////////////////////////////////////////////////////////////////////////
struct findSectionByName 
{ 
	std::string name2;
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

void ParameterSection::add(void* val, const char* name, unsigned int crc)
{ 
	parameters.push_back(BaseParameter(val, name, crc)); 
}

void ParameterSection::add_dependency(const char* fname)
{ 
	dependencies.push_back(fname); 
}

bool ParameterSection::needToReload() const
{
    return true;
    /*
	unsigned int crc = 83838383;
	DependencyList::const_iterator i;
	FOR_EACH(dependencies, i)
		crc = Parser(*i).CRC(crc);

	return crc != sourceCRC;
    */
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

#ifdef _WIN32
#include "windows.h"

FARPROC getPrmEditDLLFunction(const char* name)
{
    static HINSTANCE lh;
    if(!lh){
        static bool showError = true;
        lh = LoadLibraryA("PrmEdit.dll");
        if(!lh && showError){
            showError = false;
            XBuffer buf;
            buf < "PrmEdit.dll not found, error code: " <= GetLastError();
            MessageBoxA(0, buf, "PrmEdit error",  MB_TASKMODAL | MB_SETFOREGROUND | MB_OK | MB_ICONERROR);
        }
    }
    if(lh){
        FARPROC pf = GetProcAddress( lh, name);
        xassert("Inconsistent version of PrmEdit.dll" && pf);
        return pf;
    }
    return 0;
}
#endif
 
void edit_parameters()
{
#ifdef PERIMETER_EXODUS
    //getRunTimeCompiler().getFilesList();
    getRunTimeCompiler().saveAndCompileAll();
#else
	typedef int (*PF)(const StringList&);
	PF p = (PF)getPrmEditDLLFunction("edit_parameters");
	if(p){
		p(getRunTimeCompiler().getFilesList());
		getRunTimeCompiler().saveAndCompileAll();
    }
#endif
}

bool reload_parameters()
{
    getRunTimeCompiler().saveAndCompileAll();
    return true;
}

static int window_shown;
void show_debug_window(const char* text, int sx, int sy) {
#ifdef PERIMETER_EXODUS
    window_shown = 1;
#else
	typedef int (*PF)(const char* text, int sx, int sy);
	PF p = (PF)getPrmEditDLLFunction("show_debug_window");
	if(p){
		p(text, sx, sy);
		window_shown = 1;
		}
#endif
}

void hide_debug_window() {
    if(!window_shown)
        return;
#ifdef PERIMETER_EXODUS
    window_shown = 0;
#else
	typedef int (*PF)();
	PF p = (PF)getPrmEditDLLFunction("hide_debug_window");
	if(p){
		p();
		window_shown = 0;
		}
#endif
}

void show_profile(const char* text) {
#ifdef PERIMETER_EXODUS
#else
	typedef int (*PF)(const char* text);
	PF p = (PF)getPrmEditDLLFunction("show_profile");
	if(p)
		p(text);
#endif
}
