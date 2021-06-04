/*
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
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
#ifndef _STLP_SLIST_C
#define _STLP_SLIST_C

#ifndef _STLP_INTERNAL_SLIST_H
#  include <stl/_slist.h>
#endif

#ifdef _STLP_DEBUG
#  undef  slist
#  define slist __WORKAROUND_DBG_RENAME(slist)
#endif

#if defined (_STLP_NESTED_TYPE_PARAM_BUG) 
#  define size_type size_t
#endif

_STLP_BEGIN_NAMESPACE

_STLP_MOVE_TO_PRIV_NAMESPACE

template <class _Tp, class _Alloc> 
_Slist_node_base*
_Slist_base<_Tp,_Alloc>::_M_erase_after(_Slist_node_base* __before_first,
                                        _Slist_node_base* __last_node) {
  _Slist_node_base* __cur = __before_first->_M_next;
  while (__cur != __last_node) {
    _Node* __tmp = __STATIC_CAST(_Node*, __cur);
    __cur = __cur->_M_next;
    _STLP_STD::_Destroy(&__tmp->_M_data);
    _M_head.deallocate(__tmp,1);
  }
  __before_first->_M_next = __last_node;
  return __last_node;
}

#if defined (_STLP_DONT_USE_PTR_SPECIALIZATIONS)
_STLP_MOVE_TO_STD_NAMESPACE
#endif

template <class _Tp, class _Alloc>
_SLIST_IMPL<_Tp,_Alloc>& _SLIST_IMPL<_Tp,_Alloc>::operator=(const _SLIST_IMPL<_Tp,_Alloc>& __x) {
  if (&__x != this) {
    _Node_base* __p1 = &this->_M_head._M_data;
    _Node_base* __n1 = this->_M_head._M_data._M_next;
    const _Node_base* __n2 = __x._M_head._M_data._M_next;
    while (__n1 && __n2) {
      __STATIC_CAST(_Node*, __n1)->_M_data = __STATIC_CAST(const _Node*, __n2)->_M_data;
      __p1 = __n1;
      __n1 = __n1->_M_next;
      __n2 = __n2->_M_next;
    }
    if (__n2 == 0)
      this->_M_erase_after(__p1, 0);
    else
      _M_insert_after_range(__p1, const_iterator(__CONST_CAST(_Node_base*, __n2)), 
                                  const_iterator(0));
  }
  return *this;
}

template <class _Tp, class _Alloc>
void _SLIST_IMPL<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp& __val) { 
  _Node_base* __prev = &this->_M_head._M_data;
  _Node_base* __node = this->_M_head._M_data._M_next;
  for ( ; __node != 0 && __n > 0 ; --__n) {
    __STATIC_CAST(_Node*, __node)->_M_data = __val;
    __prev = __node;
    __node = __node->_M_next;
  }
  if (__n > 0)
    _M_insert_after_fill(__prev, __n, __val);
  else
    this->_M_erase_after(__prev, 0);
}


template <class _Tp, class _Alloc>
void _SLIST_IMPL<_Tp,_Alloc>::resize(size_type __len, const _Tp& __x) {
  _Node_base* __cur = &this->_M_head._M_data;
  while (__cur->_M_next != 0 && __len > 0) {
    --__len;
    __cur = __cur->_M_next;
  }
  if (__cur->_M_next) 
    this->_M_erase_after(__cur, 0);
  else
    _M_insert_after_fill(__cur, __len, __x);
}

template <class _Tp, class _Alloc>
void _SLIST_IMPL<_Tp,_Alloc>::remove(const _Tp& __val) {
  _Node_base* __cur = &this->_M_head._M_data;
  while (__cur && __cur->_M_next) {
    if (__STATIC_CAST(_Node*, __cur->_M_next)->_M_data == __val)
      this->_M_erase_after(__cur);
    else
      __cur = __cur->_M_next;
  }
}

template <class _Tp, class _Alloc> 
void _SLIST_IMPL<_Tp,_Alloc>::unique() {
  _Node_base* __cur = this->_M_head._M_data._M_next;
  if (__cur) {
    while (__cur->_M_next) {
      if (__STATIC_CAST(_Node*, __cur)->_M_data == 
          __STATIC_CAST(_Node*, __cur->_M_next)->_M_data)
        this->_M_erase_after(__cur);
      else
        __cur = __cur->_M_next;
    }
  }
}

template <class _Tp, class _Alloc>
void _SLIST_IMPL<_Tp,_Alloc>::merge(_SLIST_IMPL<_Tp,_Alloc>& __x) {
  _Node_base* __n1 = &this->_M_head._M_data;
  while (__n1->_M_next && __x._M_head._M_data._M_next) {
    if (__STATIC_CAST(_Node*, __x._M_head._M_data._M_next)->_M_data < 
        __STATIC_CAST(_Node*, __n1->_M_next)->_M_data)
      _STLP_PRIV::_Sl_global_inst::__splice_after(__n1, &__x._M_head._M_data, __x._M_head._M_data._M_next);
    __n1 = __n1->_M_next;
  }
  if (__x._M_head._M_data._M_next) {
    __n1->_M_next = __x._M_head._M_data._M_next;
    __x._M_head._M_data._M_next = 0;
  }
}

template <class _Tp, class _Alloc>
void _SLIST_IMPL<_Tp,_Alloc>::sort() {
  if (this->_M_head._M_data._M_next && this->_M_head._M_data._M_next->_M_next) {
    _Self __carry;
    _Self __counter[64];
    int __fill = 0;
    while (!empty()) {
      _STLP_PRIV::_Sl_global_inst::__splice_after(&__carry._M_head._M_data, &this->_M_head._M_data, 
                                                  this->_M_head._M_data._M_next);
      int __i = 0;
      while (__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry);
        __carry.swap(__counter[__i]);
        ++__i;
      }
      __carry.swap(__counter[__i]);
      if (__i == __fill)
        ++__fill;
    }

    for (int __i = 1; __i < __fill; ++__i)
      __counter[__i].merge(__counter[__i-1]);
    this->swap(__counter[__fill-1]);
  }
}

#if defined (_STLP_NESTED_TYPE_PARAM_BUG) 
#  undef size_type
#endif

#ifdef _STLP_DEBUG
#  undef  slist
#endif

#if !defined (_STLP_DONT_USE_PTR_SPECIALIZATIONS)
_STLP_MOVE_TO_STD_NAMESPACE
#endif

_STLP_END_NAMESPACE

#endif /*  _STLP_SLIST_C */

// Local Variables:
// mode:C++
// End:
