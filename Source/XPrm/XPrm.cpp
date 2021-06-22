#include "StdAfx.h"
#include "Token.h"

int64_t getRDTSC(void);

void help(int mode)
{
	cout << "Parameters compiler v" << _VERSION_ << "\n";
	if(mode)
		cout << "Incorrect switch\n";
	cout << "Switches:\n";
	cout << "/Fo<file>  generate dummy output\n";
	cout << "check_update  checks for previous updates to stop compillation\n";
	exit(mode);
}

int main(int argc, char* argv[])
{
	try 
	{
		//__int64 start_time = getRDTSC();

		string updateFile = string(get_exe_path()) + "xprm.tmp";
		if(check_command_line("/check_update")){
			XStream ini(0);
			if(ini.open(updateFile.c_str())){
				int len = ini.size();
				XBuffer buffer(len + 1);
				ini.read(buffer.address(), len);
				buffer[len] = 0;
				ini.close();
				remove(updateFile.c_str());
				if(buffer.search("Updated")){
					cout << "XPrm: sources were changed, restart compilation, please.";
					if(MessageBox(0,"XPrm: sources were changed.\nStop the compilation process?", "XPrm compiler", MB_YESNO | MB_ICONQUESTION) == IDYES)
						exit(1);
					else
						exit(0);
				}
			}
			cout << "XPrm: sources were not changed.";
			exit(0);
		}

		char* input = 0;
		char* output = 0;
		for(int i = 1; i < argc; i++)
			if(*argv[i] == '-' || *argv[i] == '/')
				switch(argv[i][1]){
					case '?':
					case 'h':
						help(0);
						break;
					case 'o':
						output = argv[i] + 2;
					case 'F':
						if(argv[i][2] == 'o')
							output = argv[i] + 3;
						else
							help(1);
						break;
					default: 
						help(1);
					}
			else
				input = argv[i];

		Compiler comp;
		if(!input)
			help(0);
			//ErrH.Abort("Prm-file expected");

		bool rebuild = !(output && XStream(0).open(output));
		int success;
		if((success = comp.compile(input, rebuild)) != 0 && output){
			//cout << "Generating dummy output: " << output << endl;
			XStream f(output, XS_OUT);
			f < "This file was generated as dummy-output while compiling: " < input < "\n";
			}

		if(success && comp.sectionUpdated()){
			XStream ff(updateFile.c_str(), XS_OUT);
			ff < "Updated";
		}
		
		//cout << "Compile time: " << double(getRDTSC() - start_time)/1e9 << endl;

		return !success;
	}
	catch(...)
	{
		cout << "Internal error occured\r\n";
		return 1;
	}

	return 1;
}

bool Compiler::compile(const char* fname, bool rebuild)
{
	int errors = 0;
	sectionUpdated_ = false;
	try {
		XBuffer bout(1024, 1);
		errors = parse_file(fname, bout);
		cout << bout;
		if(!errors){
			SectionList::iterator i;
			FOR_EACH(sections, i){
				int updated = (*i)->declaration(rebuild); 
				if((*i)->definition(updated || rebuild, dependencies) || updated)
					sectionUpdated_ = true;
			}
		}
		else
			cout << fname << ": " << errors << " error(s)" << endl;
	}
	catch(const exception& exc){
		cout << exc.what() << "\r\n";
		errors++;
	}
	
	return !errors;
}				      
