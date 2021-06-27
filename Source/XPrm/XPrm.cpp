#include "stdafx.h"
#include "ParseUtil.h"
#include "Token.h"

int64_t getRDTSC(void);

void help(int mode)
{
	std::cout << "Parameters compiler v" << _VERSION_ << "\n";
	if(mode)
		std::cout << "Incorrect switch\n";
	std::cout << "Switches:\n";
	std::cout << "-Fo<file>  generate dummy output\n";
    std::cout << "-So<path>  generated output root dir\n";
	std::cout << "-check_update  checks for previous updates to stop compilation\n";
	exit(mode);
}

int main(int argc, char* argv[])
{
	try 
	{
        setup_argcv(argc, argv);
		//__int64 start_time = getRDTSC();

        std::string updateFile = std::string(get_exe_path()) + "xprm.tmp";
		if(check_command_line("/check_update") || check_command_line("-check_update")){
			XStream ini(0);
			if(ini.open(updateFile.c_str())){
				int len = ini.size();
				XBuffer buffer(len + 1);
				ini.read(buffer.address(), len);
				buffer[len] = 0;
				ini.close();
				remove(updateFile.c_str());
				if(buffer.search("Updated")){
                    std::cout << "XPrm: sources were changed, restart compilation, please.";
					if(MessageBoxQuestion("XPrm compiler", "XPrm: sources were changed.\nStop the compilation process?"))
						exit(1);
					else
						exit(0);
				}
			}
            std::cout << "XPrm: sources were not changed.";
			exit(0);
		}

		char* input = nullptr;
		char* sources = nullptr;
		char* output = nullptr;
		for(int i = 1; i < argc; i++)
#ifdef _WIN32
            if(*argv[i] == '-' || *argv[i] == '/')
#else
			if(*argv[i] == '-')
#endif
                    switch(argv[i][1]){
					case '?':
					case 'h':
						help(0);
						break;
                    case 'S':
                        if(argv[i][2] == 'o')
                            sources = argv[i] + 3;
                        else
                            help(1);
                        break;
					case 'o':
						output = argv[i] + 2;
                        break;
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
		if((success = comp.compile(input, sources, rebuild)) != 0 && output){
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
    catch(const std::exception& exc)
    {
        std::cout << "Internal error occured:" << exc.what() << "\r\n";
        return 1;
    }
	catch(...)
	{
        std::cout << "Internal error occured\r\n";
		return 1;
	}

	return 1;
}

bool Compiler::compile(const char* fname, const char* sources, bool rebuild)
{
	int errors = 0;
	sectionUpdated_ = false;
	try {
		XBuffer bout(1024, 1);
		errors = parse_file(fname, bout);
		std::cout << bout;
		if(!errors){
			SectionList::iterator i;
			FOR_EACH(sections, i){
				int updated = (*i)->declaration(sources, rebuild); 
				if((*i)->definition(sources, updated || rebuild, dependencies) || updated)
					sectionUpdated_ = true;
			}
		}
		else
			std::cout << fname << ": " << errors << " error(s)" << std::endl;
	}
	catch(const std::exception& exc){
		std::cout << exc.what() << "\r\n";
		errors++;
	}
	
	return !errors;
}				      
