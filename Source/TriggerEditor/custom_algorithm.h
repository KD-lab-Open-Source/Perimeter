#pragma once
/********************************************************************
	created:	2003/06/30
	created:	30:6:2003   16:52
	filename: 	D:\PROJECTS\ShurasScriptEditor\custom_algorithm.h
	file path:	D:\PROJECTS\ShurasScriptEditor
	file base:	custom_algorithm
	file ext:	h
	author:		Илюха
	
	purpose:	Реализация своих алгоритмов
*********************************************************************/
#include <utility>

namespace c_algo
{
template<class InputIterator, class Function> inline
bool do_while(
				  InputIterator First,
				  InputIterator Last,
				  Function F
				  )
{
	for(; First != Last; ++First)
		if (!F(*First)) 
			return true;
	return false;
}

template<class InputIterator, class Function> inline
std::pair<Function, bool> do_while2(
								   InputIterator First,
								   InputIterator Last,
								   Function F
								   )
{
	for(; First != Last; ++First)
		if (!F(*First)) 
			return std::make_pair(F, true);
		return std::make_pair(F, false);
}

template<class InputIterator, class Function> inline
std::pair<Function, InputIterator> do_while3(
								   InputIterator First,
								   InputIterator Last,
								   Function F
								   )
{
	for(; First != Last; ++First)
		if (!F(*First)) 
			break;
	return std::make_pair(F, First);
}

}