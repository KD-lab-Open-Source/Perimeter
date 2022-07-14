#ifndef __SAFE_CAST_H__
#define __SAFE_CAST_H__

#ifdef PERIMETER_DEBUG

template <class DestinationType, class SourceType> 
DestinationType safe_cast(SourceType* source) { DestinationType p = dynamic_cast<DestinationType>(source); xassert((p || !source) && "Incorrect cast"); return p; }

template <class DestinationType, class SourceType> 
DestinationType safe_cast_ref(SourceType& source) { DestinationType p = dynamic_cast<DestinationType>(source); return p; }

#else

template <class DestinationType, class SourceType> 
DestinationType safe_cast(SourceType source) { return static_cast<DestinationType>(source); }

template <class DestinationType, class SourceType> 
DestinationType safe_cast_ref(SourceType& source) { return static_cast<DestinationType>(source); }

#endif

#endif //__SAFE_CAST_H__