
#ifndef NATIVE_H_VP_2003_12_09
#define NATIVE_H_VP_2003_12_09

#include "rules.h"

struct native_rule_t
{
    char* name;
    argument_list* arguments;
    PARSE* procedure;
};

/* MSVC debugger gets confused unless this is provided */
typedef struct native_rule_t native_rule_t ; 

void declare_native_rule(char* module, char* rule, char** args, 
                         LIST*(*f)(PARSE*, FRAME*));



#endif
