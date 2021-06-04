#pragma once
ParamBlock* ReadRules(char* rul,int* sizeparam=NULL,ParamBlock* pRulesX=NULL);
ParamBlock* ReadRulesRes(char* rul,int* psize=NULL,ParamBlock* pRulesX=NULL);

void InitStringID(bool beng,bool beginner);
bool StringIDByConst(LPCSTR name,LPCSTR value,int& ret);