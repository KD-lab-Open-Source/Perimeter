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

#ifndef _STLP_INTERNAL_DBG_DEQUE_H
#define _STLP_INTERNAL_DBG_DEQUE_H

#include <stl/debug/_iterator.h>

# if !defined (_STLP_USE_WRAPPER_FOR_ALLOC_PARAM) && !defined (_STLP_NO_DEFAULT_NON_TYPE_PARAM)
#  undef  _DBG_deque
#  define _DBG_deque deque
# endif

# define _DEQUE_WRAPPER _DBG_deque<_Tp,_Alloc>

# define _STLP_DEQUE_SUPER __WORKAROUND_DBG_RENAME(deque) <_Tp,_Alloc>

_STLP_BEGIN_NAMESPACE

# ifdef _STLP_DEBUG_USE_DISTINCT_VALUE_TYPE_HELPERS
template <class _Tp, class _Alloc>
inline _Tp* value_type(const  _DBG_iter_base< _STLP_DEQUE_SUPER >&) {
  return (_Tp*)0;
}
template <class _Tp, class _Alloc>
inline random_access_iterator_tag iterator_category(const  _DBG_iter_base< _STLP_DEQUE_SUPER >&) {
  return random_access_iterator_tag();
}
# endif

template <class _Tp, _STLP_DBG_ALLOCATOR_SELECT(_Tp) >
class _DBG_deque : private __construct_checker<_STLP_DEQUE_SUPER >,
                   public _STLP_DEQUE_SUPER
#if defined (_STLP_USE_PARTIAL_SPEC_WORKAROUND)
                    , public __stlport_class<_DBG_deque<_Tp, _Alloc> >
#endif
{
  typedef _DBG_deque<_Tp,_Alloc> _Self;
  typedef _STLP_DEQUE_SUPER _Base;
  typedef __construct_checker<_STLP_DEQUE_SUPER > _ConstructCheck;

public:                         // Basic types

  __IMPORT_CONTAINER_TYPEDEFS(_Base)

public:                         // Iterators
  typedef _DBG_iter<_Base, _DbgTraits<_Nonconst_traits<value_type> > > iterator;
  typedef _DBG_iter<_Base, _DbgTraits<_Const_traits<value_type> > >    const_iterator;

  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;

protected:
  __owned_list _M_iter_list;
  _Base* _Get_base() { return this; }
  void _Invalidate_iterator(const iterator& __it) {
    __invalidate_iterator(&_M_iter_list,__it);
  }
  void _Invalidate_all() {
    _M_iter_list._Invalidate_all();
  }

public:                         // Basic accessors
  iterator begin() { return iterator(&_M_iter_list, _Base::begin()); }
  iterator end() { return iterator(&_M_iter_list, _Base::end()); }
  const_iterator begin() const { 
    return const_iterator(&_M_iter_list, _Base::begin()); 
  }
  const_iterator end() const { 
    return const_iterator(&_M_iter_list,  _Base::end()); 
  }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(end()); }
  const_reverse_iterator rend() const 
    { return const_reverse_iterator(begin()); }

  reference operator[](size_type __n) {
    _STLP_VERBOSE_ASSERT(__n < _Base::size(), _StlMsg_OUT_OF_BOUNDS)
    return _Base::operator[](__n);
  }
  const_reference operator[](size_type __n) const {
    _STLP_VERBOSE_ASSERT(__n < _Base::size(), _StlMsg_OUT_OF_BOUNDS)
    return _Base::operator[](__n);
  }

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

public:                         // Constructor, destructor.
  explicit _DBG_deque(const allocator_type& __a = allocator_type()) :
    _STLP_DEQUE_SUPER(__a), _M_iter_list(_Get_base()) {}
  _DBG_deque(const _Self& __x) : 
    _ConstructCheck(__x), _STLP_DEQUE_SUPER(__x), _M_iter_list(_Get_base()) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_deque(size_type __n, const value_type& __x = _Tp(),
#else
  _DBG_deque(size_type __n, param_type __x,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
            const allocator_type& __a = allocator_type()) : 
    _STLP_DEQUE_SUPER(__n, __x, __a), _M_iter_list(_Get_base()) {}
#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_deque(size_type __n) :
    _STLP_DEQUE_SUPER(__n), _M_iter_list(_Get_base()) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  _DBG_deque(__move_source<_Self> src)
    : _STLP_DEQUE_SUPER(__move_source<_Base>(src.get())), _M_iter_list(_Get_base()) {
    src.get()._Invalidate_all();
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  _DBG_deque(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL)
    : _ConstructCheck(__first, __last),
      _STLP_DEQUE_SUPER(__first, __last, __a) ,
      _M_iter_list(_Get_base()) {
    }
# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  _DBG_deque(_InputIterator __first, _InputIterator __last)
    : _ConstructCheck(__first, __last),
      _STLP_DEQUE_SUPER(__first, __last), 
      _M_iter_list(_Get_base()) {
    }
# endif
#else /* _STLP_MEMBER_TEMPLATES */
  _DBG_deque(const value_type* __first, const value_type* __last,
        const allocator_type& __a = allocator_type()) 
    : _ConstructCheck(__first, __last),
      _STLP_DEQUE_SUPER(__first, __last, __a), 
      _M_iter_list(_Get_base()) {
    }

  _DBG_deque(const_iterator __first, const_iterator __last,
        const allocator_type& __a = allocator_type()) 
    : _ConstructCheck(__first, __last),
      _STLP_DEQUE_SUPER(__first._M_iterator, __last._M_iterator, __a), 
      _M_iter_list(_Get_base()) {
    }
#endif /* _STLP_MEMBER_TEMPLATES */

  _Self& operator= (const _Self& __x) {
    if (this != &__x) {
      _Invalidate_all();
      _Base::operator=((const _Base&)__x);
    }
    return *this;
  }

  void swap(_Self& __x) {
    _M_iter_list._Swap_owners(__x._M_iter_list);
    _Base::swap(__x);
  }

public: 
  void assign(size_type __n, const _Tp& __val) {
    _Invalidate_all();
    _Base::assign(__n, __val);
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
#else
  void assign(const_iterator __first, const_iterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    _Invalidate_all();
    _Base::assign(__first._M_iterator, __last._M_iterator);
  }
  void assign(const value_type *__first, const value_type *__last) {
    _STLP_DEBUG_CHECK(__check_ptr_range(__first, __last))
#endif /* _STLP_MEMBER_TEMPLATES */
    _Invalidate_all();
    _Base::assign(__first, __last);
  }

public:                         // push_* and pop_*
  
#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back(const value_type& __t = _Tp()) {
#else
  void push_back(const value_type& __t) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    _Invalidate_all();
    _Base::push_back(__t);
  }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back() {
    _Invalidate_all();
    _Base::push_back();
  }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front(const value_type& __t = _Tp()) {
#else
  void push_front(const value_type& __t) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    _Invalidate_all();
    _Base::push_front(__t);
  }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front() {
    _Invalidate_all();
    _Base::push_front();
  }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

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

public:                         // Insert

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined (_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const value_type& __x = _Tp()) {
#else
  iterator insert(iterator __pos, const value_type& __x) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _Invalidate_all();
    return iterator(&_M_iter_list, _Base::insert(__pos._M_iterator, __x));
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined (_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { 
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _Invalidate_all();
    return iterator(&_M_iter_list, _Base::insert(__pos._M_iterator));
  }
#endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void insert(iterator __pos, size_type __n, const value_type& __x) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    if (__n != 0) _Invalidate_all();
    _Base::insert(__pos._M_iterator, __n, __x);
  }

#ifdef _STLP_MEMBER_TEMPLATES  
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _AreSameUnCVTypes<_InputIterator, iterator>::_Ret _IsNonConstIterator;
    typedef typename _AreSameUnCVTypes<_InputIterator, const_iterator>::_Ret _IsConstIterator;
    typedef typename _Lor2<_IsNonConstIterator, _IsConstIterator>::_Ret _DoCheck;
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    //Sequence requirements 23.1.1 Table 67:
    _STLP_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, _DoCheck()));
    _Base::insert(__pos._M_iterator, __first, __last);
    //dums: because of self insertion iterators must be invalidated after insertion.
    if (__first != __last) _Invalidate_all();
  }
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos,
              const value_type* __first, const value_type* __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_ptr_range(__first, __last))
    _Base::insert(__pos._M_iterator, __first._M_iterator, __last._M_iterator);
    //dums: because of self insertion iterators must be invalidated after insertion.
    if (__first != __last) _Invalidate_all();
  }
  void insert(iterator __pos,
              const_iterator __first, const_iterator __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    //Sequence requirements 23.1.1 Table 67:
    _STLP_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, __true_type()));
    _Base::insert(__pos._M_iterator, __first._M_iterator, __last._M_iterator);
    //dums: because of self insertion iterators must be invalidated after insertion.
    if (__first != __last) _Invalidate_all();
  }
#endif /* _STLP_MEMBER_TEMPLATES */

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const value_type& __x = _Tp()) {
#else
  void resize(size_type __new_size, const value_type& __x) {
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
    if (__new_size != this->size()) {
      if ((__new_size > this->size()) || (__new_size < this->size() - 1))
        _Invalidate_all();
      else
        _Invalidate_iterator(this->end());
    }
    _Base::resize(__new_size, __x);
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type new_size) { resize(new_size, _STLP_DEFAULT_CONSTRUCTED(_Tp)); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

public:                         // Erase
  iterator erase(iterator __pos) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _STLP_DEBUG_CHECK(_Dereferenceable(__pos))
    if (__pos == this->begin())
      _Invalidate_iterator(__pos);
    else {
      iterator __tmp = --(this->end());
      if (__pos == __tmp) 
        _Invalidate_iterator(__pos);
      else
        _Invalidate_all();
    }
    return iterator (&_M_iter_list, _Base::erase(__pos._M_iterator));
  }

  iterator erase(iterator __first, iterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last, this->begin(), this->end()))
    if (!this->empty()) {
      if (__last == (++(this->begin())))
        _Invalidate_iterator(__last);
      else {
        if (__first == (--(this->end())))
          _Invalidate_iterator(__first);
        else
          _Invalidate_all();
      }
    }
    return iterator (&_M_iter_list, _Base::erase(__first._M_iterator, __last._M_iterator));
  }
  
  void clear() {
    _Invalidate_all();
    _Base::clear();
  }
};

#define _STLP_TEMPLATE_HEADER template <class _Tp, class _Alloc>
#define _STLP_TEMPLATE_CONTAINER _DBG_deque<_Tp, _Alloc>
#define _STLP_TEMPLATE_CONTAINER_BASE _STLP_DEQUE_SUPER
#include <stl/debug/_relops_cont.h>
#undef _STLP_TEMPLATE_CONTAINER_BASE
#undef _STLP_TEMPLATE_CONTAINER
#undef _STLP_TEMPLATE_HEADER

#ifdef _STLP_CLASS_PARTIAL_SPECIALIZATION
template <class _Tp, class _Alloc>
struct __move_traits<_DBG_deque<_Tp,_Alloc> > :
  __move_traits_aux<_STLP_DEQUE_SUPER >
{};
#endif /* _STLP_CLASS_PARTIAL_SPECIALIZATION */

_STLP_END_NAMESPACE

# undef _DBG_deque
# undef _STLP_DEQUE_SUPER
  
#endif /* _STLP_INTERNAL_DEQUE_H */

// Local Variables:
// mode:C++
// End:
