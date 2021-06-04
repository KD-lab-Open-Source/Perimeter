@echo off

if "%3" == ""  goto usage

sx -x lower -x empty %2 >%3.xml
msxsl.exe %3.xml ..\..\..\..\..\docbook-xsl-1.50.0\html\my\%1 -o %3
del %3.xml
tidy.exe -config tidy_config -m %3
python tidy_attr.py %3
exit

:usage
echo Usage examples:
echo    msxsl_build.bat docbook_msxsl.xsl src\body.sgml index.html
echo    msxsl_build.bat docbook_msxsl.xsl paper\src\body.sgml paper\mpl_paper.html
