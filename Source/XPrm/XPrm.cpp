#include "stdafx.h"
#include "ParseUtil.h"
#include "Token.h"

void help(int mode)
{
	std::cout << "Parameters compiler v" << _VERSION_ << "\n";
	if(mode)
		std::cout << "Incorrect switch\n";
	std::cout << "Switches:\n";
    std::cout << "-C  causes failure if output file needs update, useful for CI checking\n";
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
        
		char* input = nullptr;
		const char* sources = nullptr;
		char* output = nullptr;
        bool fail_outdated = false;
		for(int i = 1; i < argc; i++) {
            //printf("%d %s\n", i, argv[i]);
            if (*argv[i] == '-') {
                switch (argv[i][1]) {
                    case '?':
                    case 'h':
                        help(0);
                        break;
                    case 'C':
                        fail_outdated = true;
                        break;
                    case 'S':
                        if (argv[i][2] == 'o')
                            sources = argv[i] + 3;
                        else
                            help(1);
                        break;
                    case 'o':
                        output = argv[i] + 2;
                        break;
                    case 'F':
                        if (argv[i][2] == 'o')
                            output = argv[i] + 3;
                        else
                            help(1);
                        break;
                    default:
                        help(1);
                }
            } else {
                input = argv[i];
            }
        }
		Compiler comp;
		if(!input)
			help(0);
			//ErrH.Abort("Prm-file expected");

		bool rebuild = false; //!(output && XStream(0).open(output));
		int success;
        std::string sources_path;
        if (sources) {
            sources_path = convert_path_xprm(sources);
            sources = sources_path.c_str();
        }
		if((success = comp.compile(input, sources, rebuild, fail_outdated)) != 0 && output){
			//cout << "Generating dummy output: " << output << endl;
			XStream f(output, XS_OUT);
			f < "This file was generated as dummy-output while compiling: " < input < "\n";
        }

		//cout << "Compile time: " << double(getRDTSC() - start_time)/1e9 << endl;

		return !success;
	}
    catch(const std::exception& exc)
    {
        std::cout << "Internal error occured:" << exc.what() << "\n";
        return 1;
    }
	catch(...)
	{
        std::cout << "Internal error occured\n";
		return 1;
	}

	return 1;
}	      
