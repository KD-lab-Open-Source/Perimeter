////////////////////////////////////////////////////////////////////////////////
//
// Additional include for making STL more convenient
//
// Author: Alexandre Kotliar, K-D Lab
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __STL_ADDITION_H__
#define __STL_ADDITION_H__

// For disabling some warning 
#pragma warning( disable : 4786 4284 4800)

#ifndef FOR_EACH
#define FOR_EACH(list, iterator) \
  for(iterator = (list).begin(); iterator != (list).end(); ++iterator)
#endif

#endif // __STL_ADDITION_H__