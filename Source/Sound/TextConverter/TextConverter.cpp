// TODO: change encoding to utf-8

#include "stdafx.h"

#define _NOSTD_
#include "parse/ParamParse.h"
#include <stdlib.h>
#include <conio.h>
#include "MessageBlock.h"

char inname[_MAX_PATH]="sound.cfg";
char outname[_MAX_PATH];

int main(int argc, char* argv[])
{
	if(argc>=2 && argv[1][0]!='-')
		strcpy(inname,argv[1]);

	bool set_out_name=false;
	for(int i=0;i<argc;i++)
	{
		char* str=argv[i];
		if(str[0]=='-')
		{
			if(strcmp(str,"-W")==0)
			{
				printf_window_output=true;
			}

			if(str[1]=='O')
			{
				set_out_name=true;
				strcpy(outname,str+2);
			}
		}
	}

	int f=_open( inname, _O_RDONLY |_O_BINARY);
	if(f==-1)
	{
		message_printf("cannot open %s",inname);
		exit(1);
		return 1;
	}

	int size=_lseek(f,0,SEEK_END);_lseek(f,0,SEEK_SET);
	char* text=new char[size+1];
	text[size]=0;
	_read(f,text,size);
	_close(f);

	ParamBlock* pGetRulesConst=NULL;
	ParamBlock* paramblock;
	pGetRulesConst=ReadRulesRes("SOUND");

	int sizeparam;
	char* error=ParseText(text,&paramblock,&sizeparam,
		pGetRulesConst,NULL);

	if(error)
	{
		message_printf("file=%s\n%s",inname,error);
		exit(1);
		return 1;
	}

	delete text;

	if(!set_out_name)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(inname,drive,dir,fname,ext);

		_makepath(outname,drive,dir,fname,"dat");
	}

	remove(outname);
	f=_open( outname, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 
                      _S_IREAD | _S_IWRITE );
	if(f==-1)
	{
		message_printf("Не могу записать %s",outname);
		exit(1);
		return 1;
	}

	_write(f,paramblock,sizeparam);
	_close(f);

	dosprintf("%s - откомпилирован успешно.\n",inname);
//	dosprintf("Нажмите любую клавишу для окончания программы.\n");getch();

	exit(0);
	return 0;
}

