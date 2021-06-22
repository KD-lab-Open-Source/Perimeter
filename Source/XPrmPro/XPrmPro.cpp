#include "StdAfx.h"
#include "Token.h"

int64_t getRDTSC(void);

void help(int mode)
{
	cout << "Parameters compiler v" << _VERSION_ << "\n";
	if(mode)
		cout << "Incorrect switch\n";
	exit(mode);
}

int main(int argc, char* argv[])
{
	try 
	{
		//__int64 start_time = getRDTSC();

		char* input = 0;
		bool dontDeclare = false;
		for(int i = 1; i < argc; i++)
			if(*argv[i] == '-' || *argv[i] == '/'){
				if(!strcmp(argv[i] + 1, "delegate-"))
					dontDeclare = true;
				else
					switch(argv[i][1]){
						case '?':
						case 'h':
							help(0);
							break;
						default: 
							help(1);
						}
			}
			else
				input = argv[i];

		Compiler comp(dontDeclare);
		if(!input)
			help(0);

		int success = comp.compile(input);
		
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

