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

#ifndef _STLP_INTERNAL_DBG_LIST_H
#define _STLP_INTERNAL_DBG_LIST_H

#ifndef _STLP_INTERNAL_ALGO_H
#  include <stl/_algo.h>
#endif

#include <stl/debug/_iterator.h>

#ifndef _STLP_USE_WRAPPER_FOR_ALLOC_PARAM
#  undef  _DBG_list
#  define _DBG_list list
#endif

#  define _STLP_DBG_LIST_BASE __WORKAROUND_DBG_RENAME(list) <_Tp, _Alloc>

_STLP_BEGIN_NAMESPACE

# ifdef _STLP_DEBUG_USE_DISTINCT_VALUE_TYPE_HELPERS
template <class _Tp, class _Alloc>
inline _Tp*
value_type(const  _DBG_iter_base< _STLP_DBG_LIST_BASE >&) {
  return (_Tp*)0;
}
template <class _Tp, class _Alloc>
inline bidirectional_iterator_tag
iterator_category(const _DBG_iter_base< _STLP_DBG_LIST_BASE >&) {
  return bidirectional_iterator_tag();
}
# endif

template <class _Tp, _STLP_DEFAULT_ALLOCATOR_SELECT(_Tp) >
class _DBG_list : private __construct_checker<_STLP_DBG_LIST_BASE >,
                  public _STLP_DBG_LIST_BASE
#if defined (_STLP_USE_PARTIAL_SPEC_WORKAROUND)
                  , public __stlport_class<_DBG_list<_Tp, _Alloc> >
#endif
{
  typedef _STLP_DBG_LIST_BASE _Base;
  typedef _DBG_list<_Tp, _Alloc> _Self;
  typedef __construct_checker<_STLP_DBG_LIST_BASE > _ConstructCheck;

public:      
  __IMPORT_CONTAINER_TYPEDEFS(_Base)

public:
  typedef _DBG_iter<_Base, _DbgTraits<_Nonconst_traits<value_type> > > iterator;
  typedef _DBG_iter<_Base, _DbgTraits<_Const_traits<value_type> > >    const_iterator;

  _STLP_DECLARE_BIDIRECTIONAL_REVERSE_ITERATORS;

protected:
  __owned_list _M_iter_list;
  _Base* _Get_base() { return this; }
  void _Invalidate_iterator(const iterator& __it) { 
    __invalidate_iterator(&_M_iter_list,__it);
  }
  void _Invalidate_iterators(const iterator& __first, const iterator& __last) {
    __invalidate_range(&_M_iter_list, __first, __last);
  }
  void _Invalidate_all() {
    _M_iter_list._Invalidate_all();
  }

  typedef typename _Base::iterator _Base_iterator;

public:
  explicit _DBG_list(const allocator_type& __a = allocator_type()) :
    _STLP_DBG_LIST_BASE(__a), _M_iter_list(_Get_base()) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_list(size_type __n, const _Tp& __x = _Tp(),
#else
  _DBG_list(size_type __n, const _Tp& __x,
#endif /*!_STLP_DONT_SUP_DFLT_PARAM*/
            const allocator_type& __a = allocator_type())
    : _STLP_DBG_LIST_BASE(__n, __x, __a), _M_iter_list(_Get_base()) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_list(size_type __n)
    : _STLP_DBG_LIST_BASE(__n), _M_iter_list(_Get_base()) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  _DBG_list(__move_source<_Self> src)
    : _STLP_DBG_LIST_BASE(__move_source<_Base>(src.get())), 
      _M_iter_list(_Get_base()) {}
  
#if defined (_STLP_MEMBER_TEMPLATES)
  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.  
  template <class _InputIterator>
  _DBG_list(_InputIterator __first, _InputIterator __last,
            const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL)
    : _ConstructCheck(__first, __last), 
      _STLP_DBG_LIST_BASE(__first, __last, __a), _M_iter_list(_Get_base()) {
    }
#  if defined (_STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS)
  template <class _InputIterator>
  _DBG_list(_InputIterator __first, _InputIterator __last)
    : _ConstructCheck(__first, __last), 
      _STLP_DBG_LIST_BASE(__first, __last), _M_iter_list(_Get_base()) {
    }
#  endif
#else /* _STLP_MEMBER_TEMPLATES */

  _DBG_list(const value_type* __first, const value_type* __last,
            const allocator_type& __a = allocator_type())
    : _ConstructCheck(__first, __last), 
      _STLP_DBG_LIST_BASE(__first, __last, __a), _M_iter_list(_Get_base()) {
    }
  _DBG_list(const_iterator __first, const_iterator __last,
            const allocator_type& __a = allocator_type())
    : _ConstructCheck(__first, __last), 
      _STLP_DBG_LIST_BASE(__first._M_iterator, __last._M_iterator, __a), _M_iter_list(_Get_base()) {
    }

#endif /* _STLP_MEMBER_TEMPLATES */

  _DBG_list(const _Self& __x) : 
    _ConstructCheck(__x),
    _STLP_DBG_LIST_BASE(__x) , _M_iter_list(_Get_base()) {}

  _Self& operator= (const _Self& __x) {
    if (this != &__x) {
      //Should not invalidate end iterator
      _Invalidate_iterators(this->begin(), this->end());
      _Base::operator=((const _Base&)__x);
    }
    return *this;
  }

public:
  iterator begin()             { return iterator(&_M_iter_list, _Base::begin()); }
  const_iterator begin() const { return const_iterator(&_M_iter_list, _Base::begin()); }

  iterator end()               { return iterator(&_M_iter_list, _Base::end()); }
  const_iterator end() const   { return const_iterator(&_M_iter_list, _Base::end()); }

  reverse_iterator rbegin()    { return reverse_iterator(end()); }
  reverse_iterator rend()      { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

  // those are here to enforce checking
  reference front() {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    return *begin();
  }
  const_reference front() const {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    return *begin();
  }
  reference back() {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    return *(--end());
  }
  const_reference back() const {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    return *(--end());
  }

  void swap(_Self& __x) {
    _M_iter_list._Swap_owners(__x._M_iter_list);
    _Base::swap(__x);
  }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const _Tp& __x = _Tp()) {
#else
  iterator insert(iterator __pos, const _Tp& __x) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    return iterator(&_M_iter_list,_Base::insert(__pos._M_iterator, __x) );
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { return insert(__pos, _STLP_DEFAULT_CONSTRUCTED(_Tp)); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

#if defined (_STLP_MEMBER_TEMPLATES)
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
#  if (_STLP_DEBUG_LEVEL == _STLP_STANDARD_DBG_LEVEL)
    typedef typename _AreSameUnCVTypes<_InputIterator, iterator>::_Ret _IsListIterator;
    typedef typename _AreSameUnCVTypes<_InputIterator, const_iterator>::_Ret _IsListConstIterator;
    typedef typename _Lor2<_IsListIterator, _IsListConstIterator>::_Ret _DoCheck;
#  endif 
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    _STLP_STD_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, _DoCheck()))
    _Base::insert(__pos._M_iterator, __first, __last);
  }
#else
  void insert(iterator __pos, const _Tp* __first, const _Tp* __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _STLP_DEBUG_CHECK(__check_ptr_range(__first, __last))
    _Base::insert(__pos._M_iterator, __first, __last);
  }

  void insert(iterator __pos,
              const_iterator __first, const_iterator __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    _STLP_STD_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, _DoCheck()))
    _Base::insert(__pos._M_iterator, __first._M_iterator, __last._M_iterator);
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  void insert(iterator __pos, size_type __n, const _Tp& __x) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _Base::insert(__pos._M_iterator, __n, __x);    
  }

  void pop_back() {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    _Invalidate_iterator(this->end());
    _Base::pop_back();
  }

  void pop_front() {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    _Invalidate_iterator(this->begin());
    _Base::pop_front();
  }

  iterator erase(iterator __pos) {
    _STLP_DEBUG_CHECK(_Dereferenceable(__pos))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _Invalidate_iterator(__pos);
    return iterator(&_M_iter_list,_Base::erase(__pos._M_iterator));
  }
  iterator erase(iterator __first, iterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last, this->begin(), this->end()))
    _Invalidate_iterators(__first, __last);
    return iterator (&_M_iter_list, _Base::erase(__first._M_iterator, __last._M_iterator));
  }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const _Tp& __x = _Tp()) {
#else
  void resize(size_type __new_size, const _Tp& __x) {
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
    typename _Base::iterator __i = _Base::begin();
    size_type __len = 0;
    for ( ; __i != _Base::end() && __len < __new_size; ++__i, ++__len);
    
    if (__len == __new_size)
      erase(iterator(&_M_iter_list, __i), this->end());
    else                          // __i == end()
      _Base::insert(_Base::end(), __new_size - __len, __x);
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { resize(__new_size, _STLP_DEFAULT_CONSTRUCTED(_Tp)); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void remove(const _Tp& __x) {
    _Base_iterator __first = _Base::begin(), __last = _Base::end();
    while (__first != __last) {
      _Base_iterator __next = __first;
      ++__next;
      if (__x == *__first) {
        _Invalidate_iterator(iterator(&_M_iter_list, __first));
        _Base::erase(__first);
      }
      __first = __next;
    }
  }

  void clear() {   
    _Invalidate_iterators(this->begin(), this->end());
    _Base::clear(); 
  }

public:
  void splice(iterator __pos, _Self& __x) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _Base::splice(__pos._M_iterator, __x);
#if (_STLP_DEBUG_LEVEL == _STLP_STANDARD_DBG_LEVEL)
    // Std: 23.2.2.4:4
    __x._Invalidate_all();
#else
    __x._M_iter_list._Set_owner(_M_iter_list);
#endif
  }

  void splice(iterator __pos, _Self& __x, iterator __i) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _STLP_DEBUG_CHECK(_Dereferenceable(__i))
    _STLP_DEBUG_CHECK(__check_if_owner(&(__x._M_iter_list),__i))
    _Base::splice(__pos._M_iterator, __x, __i._M_iterator);
#if (_STLP_DEBUG_LEVEL == _STLP_STANDARD_DBG_LEVEL)
    // Std: 23.2.2.4:7
    __x._Invalidate_iterator(__i);
#else
    __change_ite_owner(__i, &_M_iter_list);
#endif
  }

  void splice(iterator __pos, _Self& __x, iterator __first, iterator __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_range(__first, __last, __x.begin(), __x.end()))
    _STLP_DEBUG_CHECK(this == &__x ? !__check_range(__pos, __first, __last) : true)
#if (_STLP_DEBUG_LEVEL == _STLP_STANDARD_DBG_LEVEL)
    // Std: 23.2.2.4:12
    __x._Invalidate_iterators(__first, __last);
#else
    __change_range_owner(__first, __last, &_M_iter_list);
#endif
    _Base::splice(__pos._M_iterator, __x, __first._M_iterator, __last._M_iterator);
  }

  void merge(_Self& __x) {
    _STLP_DEBUG_CHECK(_STLP_STD::is_sorted(this->begin()._M_iterator, this->end()._M_iterator))
    _STLP_DEBUG_CHECK(_STLP_STD::is_sorted(__x.begin()._M_iterator, __x.end()._M_iterator))
    _Base::merge(__x);
    __x._M_iter_list._Set_owner(_M_iter_list);
  }
  void reverse() {
    _Base::reverse();
  }
  void unique() {
    _Base_iterator __first = _Base::begin(), __last = _Base::end();
    if (__first == __last) return;
    _Base_iterator __next = __first;
    while (++__next != __last) {
      if (*__first == *__next) {
        _Invalidate_iterator(iterator(&_M_iter_list, __next));
        _Base::erase(__next);
      }
      else
        __first = __next;
      __next = __first;
    }
  }
  void sort() {
    _Base::sort();
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _Predicate> 
  void remove_if(_Predicate __pred) {
    _Base_iterator __first = _Base::begin(), __last = _Base::end();
    while (__first != __last) {
      _Base_iterator __next = __first;
      ++__next;
      if (__pred(*__first)) {
        _Invalidate_iterator(iterator(&_M_iter_list, __first));
        _Base::erase(__first);
      }
      __first = __next;
    }
  }

  template <class _BinaryPredicate>
  void unique(_BinaryPredicate __binary_pred) {
    _Base_iterator __first = _Base::begin(), __last = _Base::end();
    if (__first == __last) return;
    _Base_iterator __next = __first;
    while (++__next != __last) {
      if (__binary_pred(*__first, *__next)) {
        _Invalidate_iterator(iterator(&_M_iter_list, __next));
        _Base::erase(__next);
      }
      else
        __first = __next;
      __next = __first;
    }
  }

  template <class _StrictWeakOrdering>
  void merge(_Self& __x, _StrictWeakOrdering __comp) {
    _STLP_DEBUG_CHECK(_STLP_STD::is_sorted(_Base::begin(), _Base::end(), __comp))
    _STLP_DEBUG_CHECK(_STLP_STD::is_sorted(__x.begin()._M_iterator, __x.end()._M_iterator, __comp))
    _Base::merge(__x, __comp);
    __x._M_iter_list._Set_owner(_M_iter_list);
  }

  template <class _StrictWeakOrdering>
  void sort(_StrictWeakOrdering __comp) {
    _Base::sort(__comp);
  }
#endif /* _STLP_MEMBER_TEMPLATES */
};


#define _STLP_TEMPLATE_HEADER template <class _Tp, class _Alloc>
#define _STLP_TEMPLATE_CONTAINER _DBG_list<_Tp,_Alloc>
#define _STLP_TEMPLATE_CONTAINER_BASE _STLP_DBG_LIST_BASE
#include <stl/debug/_relops_cont.h>
#undef _STLP_TEMPLATE_CONTAINER_BASE
#undef _STLP_TEMPLATE_CONTAINER
#undef _STLP_TEMPLATE_HEADER

#ifdef _STLP_CLASS_PARTIAL_SPECIALIZATION
template <class _Tp, class _Alloc>
struct __move_traits<_DBG_list<_Tp,_Alloc> > :
  __move_traits_aux<_STLP_DBG_LIST_BASE >
{};
#endif /* _STLP_CLASS_PARTIAL_SPECIALIZATION */

_STLP_END_NAMESPACE

#undef _STLP_DBG_LIST_BASE
#undef _DBG_list

#endif /* _STLP_INTERNAL_LIST_H */

// Local Variables:
// mode:C++
// End:
