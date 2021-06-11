#include "StdAfx.h"
#include "PrmEdit.h"

class BaseParameterList {};
class DependencyList {};

void edit_parameters()
{

}

bool reload_parameters()
{
    return false;
}

ParameterSection::ParameterSection(const char* name_)
    : parameters(*new BaseParameterList())
    , dependencies(* new DependencyList())
{

}
ParameterSection::~ParameterSection()
{

}

void ParameterSection::reserve(int size)
{

}
void ParameterSection::add(void* val, const char* name)
{

}
void ParameterSection::add_dependency(const char* fname)
{

}
const char* ParameterSection::fname() const
{
    return NULL;
}

bool ParameterSection::needToReload() const
{
    return false;
}

void ParameterSection::add_parameter_section(ParameterSection* section)
{

}