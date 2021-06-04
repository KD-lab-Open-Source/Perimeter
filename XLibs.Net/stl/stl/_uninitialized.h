/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef _STLP_INTERNAL_UNINITIALIZED_H
#define _STLP_INTERNAL_UNINITIALIZED_H

#ifndef _STLP_CSTRING
#  include <cstring>
#endif

#ifndef _STLP_INTERNAL_ALGOBASE_H
#  include <stl/_algobase.h>
#endif

#ifndef _STLP_INTERNAL_CONSTRUCT_H
#  include <stl/_construct.h>
#endif

_STLP_BEGIN_NAMESPACE

// uninitialized_copy

inline void*
__ucopy_trivial(const void* __first, const void* __last, void* __result) {
  //dums: this version can use memcpy (__copy_trivial can't)
  return (__last == __first) ? __result : 
    ((char*)memcpy(__result, __first, ((const char*)__last - (const char*)__first))) + 
    ((const char*)__last - (const char*)__first);
}

template <class _InputIter, class _OutputIter>
inline _OutputIter __ucopy_ptrs(_InputIter __first, _InputIter __last, _OutputIter __result, 
                               const __false_type& /*IsOKToMemCpy*/) {
  return __copy(__first, __last, __result, random_access_iterator_tag(), (ptrdiff_t*)0);
}
template <class _InputIter, class _OutputIter>
inline _OutputIter __ucopy_ptrs(_InputIter __first, _InputIter __last, _OutputIter __result, 
                               const __true_type& /*IsOKToMemCpy*/) {
// we know they all pointers, so this cast is OK 
  //  return (_OutputIter)__copy_trivial(&(*__first), &(*__last), &(*__result));
  return (_OutputIter)__ucopy_trivial(__first, __last, __result);
}

template <class _InputIter, class _OutputIter>
inline _OutputIter __ucopy_aux(_InputIter __first, _InputIter __last, _OutputIter __result, 
                               const __true_type& /*BothPtrType*/) {
  return __ucopy_ptrs(__first, __last, __result, 
                      _IsOKToMemCpy(_STLP_VALUE_TYPE(__first, _InputIter),
                                    _STLP_VALUE_TYPE(__result, _OutputIter))._Answer());
}

template <class _InputIter, class _OutputIter>
inline _OutputIter __ucopy_aux(_InputIter __first, _InputIter __last, _OutputIter __result, 
                               const __false_type& /*BothPtrType*/) {
  return __copy(__first, __last, __result, 
		            _STLP_ITERATOR_CATEGORY(__first, _InputIter), 
                _STLP_DISTANCE_TYPE(__first, _InputIter));
}

/*
 * Valid if 
 *  - copy construction is trivial
 *  - assignment operator is trivial
 * Most of the time a type having a trivial copy constructor will
 * have a trivial assign operator and vise versa but in the rare case
 * where it do not happen we prefer to call the trivial copy constructor
 * to do the bit copy. Moreover if there is no trivial assign there is
 * no chance to finally call a simple memcpy so no reason to use __ucopy_aux.
 */
template <class _InputIter, class _ForwardIter>
inline _ForwardIter 
__uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result,
                     const __true_type& /*TrivialCopyAndAssign*/) {
  return __ucopy_aux(__first, __last, __result, _BothPtrType< _InputIter, _ForwardIter> :: _Ret());
}

template <class _InputIter, class _ForwardIter>
_STLP_INLINE_LOOP
_ForwardIter 
__uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result,
                     const __false_type& /*TrivialCopyAndAssign*/) {
  _ForwardIter __cur = __result;
  _STLP_TRY {
    for ( ; __first != __last; ++__first, ++__cur)
      _Param_Construct(&*__cur, *__first);
    return __cur;
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__result, __cur))
  _STLP_RET_AFTER_THROW(__cur)
}

template <class _InputIter, class _ForwardIter>
inline _ForwardIter
uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result) {
  return __uninitialized_copy(__first, __last, __result, 
                              _UseTrivialUCopy(_STLP_VALUE_TYPE(__first, _InputIter),
                                               _STLP_VALUE_TYPE(__result, _ForwardIter))._Answer());
}

inline char* 
uninitialized_copy(const char* __first, const char* __last, char* __result) {
  return  (char*)__ucopy_trivial (__first, __last, __result);
}

#  ifdef _STLP_HAS_WCHAR_T // dwa 8/15/97
inline wchar_t* 
uninitialized_copy(const wchar_t* __first, const wchar_t* __last, wchar_t* __result) {
  return  (wchar_t*)__ucopy_trivial (__first, __last, __result);
}
#  endif /* _STLP_HAS_WCHAR_T */

# ifndef _STLP_NO_EXTENSIONS
// uninitialized_copy_n (not part of the C++ standard)

template <class _InputIter, class _Size, class _ForwardIter>
_STLP_INLINE_LOOP 
pair<_InputIter, _ForwardIter>
__uninitialized_copy_n(_InputIter __first, _Size __count,
                       _ForwardIter __result,
                       const input_iterator_tag &) {
  _ForwardIter __cur = __result;
  _STLP_TRY {
    for ( ; __count > 0 ; --__count, ++__first, ++__cur) 
      _Param_Construct(&*__cur, *__first);
    return pair<_InputIter, _ForwardIter>(__first, __cur);
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__result, __cur))
  _STLP_RET_AFTER_THROW((pair<_InputIter, _ForwardIter>(__first, __cur)))
}

# if defined(_STLP_NONTEMPL_BASE_MATCH_BUG) 
template <class _InputIterator, class _Size, class _ForwardIterator>
inline pair<_InputIterator, _ForwardIterator>
__uninitialized_copy_n(_InputIterator __first, _Size __count,
                       _ForwardIterator __result,
                       const forward_iterator_tag &) {
  return __uninitialized_copy_n(__first, __count, __result, input_iterator_tag());
}

template <class _InputIterator, class _Size, class _ForwardIterator>
inline pair<_InputIterator, _ForwardIterator>
__uninitialized_copy_n(_InputIterator __first, _Size __count,
                       _ForwardIterator __result,
                       const bidirectional_iterator_tag &) {
  return __uninitialized_copy_n(__first, __count, __result, input_iterator_tag());
}
# endif


template <class _RandomAccessIter, class _Size, class _ForwardIter>
inline pair<_RandomAccessIter, _ForwardIter>
__uninitialized_copy_n(_RandomAccessIter __first, _Size __count, _ForwardIter __result, const random_access_iterator_tag &) {
  _RandomAccessIter __last = __first + __count;
  return pair<_RandomAccessIter, _ForwardIter>(__last, 
                                               __uninitialized_copy(__first, __last, __result, 
                                                                    _UseTrivialUCopy(_STLP_VALUE_TYPE(__first, _RandomAccessIter),
                                                                                     _STLP_VALUE_TYPE(__result, _ForwardIter))._Answer()));
}

// this is used internally in <rope> , which is extension itself.
template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter>
uninitialized_copy_n(_InputIter __first, _Size __count,
                     _ForwardIter __result) {
  return __uninitialized_copy_n(__first, __count, __result, _STLP_ITERATOR_CATEGORY(__first, _InputIter));
}
# endif /* _STLP_NO_EXTENSIONS */

/*
 * Valid if :
 * - we are using the copy constructor
 * - copy construction is equivalent to assignment
 * - destructor is trivial.
 * All that is guaranty by the POD types.
 */
template <class _ForwardIter, class _Tp>
inline void
__uninitialized_fill(_ForwardIter __first, _ForwardIter __last, 
                     const _Tp& __x, const __true_type& /*IsPOD*/) {
  _STLP_STD::fill(__first, __last, __x);
}

template <class _ForwardIter, class _Tp>
_STLP_INLINE_LOOP void
__uninitialized_fill(_ForwardIter __first, _ForwardIter __last, 
                     const _Tp& __x, const __false_type& /*IsPOD*/) {
  _ForwardIter __cur = __first;
  _STLP_TRY {
    for ( ; __cur != __last; ++__cur)
      _Param_Construct(&*__cur, __x);
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__first, __cur))
}

template <class _ForwardIter, class _Tp>
inline void uninitialized_fill(_ForwardIter __first, _ForwardIter __last,  const _Tp& __x) {
  __uninitialized_fill(__first, __last, __x, _STLP_IS_POD_ITER(__first, _ForwardIter));
}

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter
__uninitialized_fill_n(_ForwardIter __first, _Size __n,
                       const _Tp& __x, const __true_type& /*IsPOD*/) {
  return __fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp>
_STLP_INLINE_LOOP _ForwardIter
__uninitialized_fill_n(_ForwardIter __first, _Size __n,
                       const _Tp& __x, const __false_type& /*IsPOD*/) {
  _ForwardIter __cur = __first;
  _STLP_TRY {
    for ( ; __n > 0; --__n, ++__cur)
      _Param_Construct(&*__cur, __x);
    return __cur;
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__first, __cur))
  _STLP_RET_AFTER_THROW(__cur)
}

template <class _ForwardIter, class _Size, class _Tp>
inline void
uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x) {
  __uninitialized_fill_n(__first, __n, __x, _STLP_IS_POD_ITER(__first, _ForwardIter));
}

// Extensions: __uninitialized_copy_copy, __uninitialized_copy_fill, 
// __uninitialized_fill_copy.

// __uninitialized_copy_copy
// Copies [first1, last1) into [result, result + (last1 - first1)), and
//  copies [first2, last2) into
//  [result + (last1 - first1), result + (last1 - first1) + (last2 - first2)).

template <class _InputIter1, class _InputIter2, class _ForwardIter>
inline _ForwardIter
__uninitialized_copy_copy(_InputIter1 __first1, _InputIter1 __last1,
                          _InputIter2 __first2, _InputIter2 __last2,
                          _ForwardIter __result, const __true_type& /*TrivialCopyAndAssign*/) {
  return __uninitialized_copy(__first2, __last2, 
                              __uninitialized_copy(__first1, __last1, __result, __true_type()), __true_type());
}

template <class _InputIter1, class _InputIter2, class _ForwardIter>
inline _ForwardIter
__uninitialized_copy_copy(_InputIter1 __first1, _InputIter1 __last1,
                          _InputIter2 __first2, _InputIter2 __last2,
                          _ForwardIter __result, const __false_type& /*TrivialCopyAndAssign*/) {
  _ForwardIter __mid = __uninitialized_copy(__first1, __last1, __result, __false_type());
  _STLP_TRY {
    return __uninitialized_copy(__first2, __last2, __mid , __false_type());
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__result, __mid))
  _STLP_RET_AFTER_THROW(__mid)
}

// __uninitialized_fill_copy
// Fills [result, mid) with x, and copies [first, last) into
//  [mid, mid + (last - first)).
template <class _ForwardIter, class _Tp, class _InputIter>
inline _ForwardIter 
__uninitialized_fill_copy(_ForwardIter __result, _ForwardIter __mid, const _Tp& __x,
                          _InputIter __first, _InputIter __last) {
  typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
  typedef typename _TrivialUCopy<_Tp>::_Ret _Trivial_UCopy;
  __uninitialized_fill(__result, __mid, __x, _Is_POD());
  _STLP_TRY {
    return __uninitialized_copy(__first, __last, __mid, _Trivial_UCopy());
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__result, __mid))
  _STLP_RET_AFTER_THROW(__result)
}

// __uninitialized_copy_fill
// Copies [first1, last1) into [first2, first2 + (last1 - first1)), and
//  fills [first2 + (last1 - first1), last2) with x.
template <class _InputIter, class _ForwardIter, class _Tp>
inline void
__uninitialized_copy_fill(_InputIter __first1, _InputIter __last1,
                          _ForwardIter __first2, _ForwardIter __last2,
                          const _Tp& __x) {
  typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
  typedef typename _TrivialUCopy<_Tp>::_Ret _Trivial_UCopy;
  _ForwardIter __mid2 = __uninitialized_copy(__first1, __last1, __first2, _Trivial_UCopy());
  _STLP_TRY {
    __uninitialized_fill(__mid2, __last2, __x, _Is_POD());
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__first2, __mid2))
}

// __uninitialized_move
template <class _InputIter, class _ForwardIter, class _TrivialUCpy>
inline _ForwardIter 
__uninitialized_move(_InputIter __first, _InputIter __last, _ForwardIter __result, _TrivialUCpy __trivial_ucpy,
                     const __false_type& /*_Movable*/) {
  return __uninitialized_copy(__first, __last, __result, __trivial_ucpy);
}

template <class _InputIter, class _ForwardIter, class _TrivialUCpy>
_STLP_INLINE_LOOP
_ForwardIter 
__uninitialized_move(_InputIter __first, _InputIter __last, _ForwardIter __result,
                     _TrivialUCpy , const __true_type& /*_Movable*/) {
  _ForwardIter __cur = __result;
  _STLP_TRY {
    for ( ; __first != __last; ++__first, ++__cur)
      _Move_Construct(&*__cur, *__first);
    return __cur;
  }
  _STLP_UNWIND(_STLP_STD::_Destroy_Range(__result, __cur))
  _STLP_RET_AFTER_THROW(__cur)
}

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_UNINITIALIZED_H */

// Local Variables:
// mode:C++
// End:
