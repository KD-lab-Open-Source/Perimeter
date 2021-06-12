
#include "Token.h"
#include "PrmEdit.h"

class RunTimeCompiler : list<ParameterSection*>, Compiler
{
	StringList files;
public:
	RunTimeCompiler();
	void addSection(ParameterSection* section);
	void saveAndCompileAll();
	void loadSection(ParameterSection* section);
	void edit();

	const StringList& getFilesList() const { return files; }
};