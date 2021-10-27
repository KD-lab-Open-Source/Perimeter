// stdafx.cpp : source file that includes just the standard includes
//	TextConverter.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

char TEXTMSG[1024];
int __cdecl AssertFail(char szErr[], char szFileName[], int nLine, char szMessage[])
{
    if (szMessage != NULL)
        wsprintf(TEXTMSG, "Assert(%s);\nFile %s, line %d.  %s", szErr, szFileName, nLine, szMessage);
    else 
        wsprintf(TEXTMSG, "Assert(%s);\nFile %s, line %d.", szErr, szFileName, nLine);
    switch (MessageBox(NULL, TEXTMSG, "MAXtoGO", MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_APPLMODAL))
        {
        case IDABORT:
			exit(1);
			return 2;
        case IDRETRY:
			return 1;
            // Fall Through //
        case IDIGNORE:
			return 0;
        } // switch
	return 0;
} // AssertFail

