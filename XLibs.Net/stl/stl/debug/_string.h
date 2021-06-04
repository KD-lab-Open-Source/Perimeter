/*
 * Copyright (c) 1997-1999
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
 */

#ifndef _STLP_DBG_STRING_H
#define _STLP_DBG_STRING_H

#ifndef _STLP_DBG_ITERATOR_H
#  include <stl/debug/_iterator.h>
#endif

#if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
#  define _STLP_NON_DBG_STRING_BASE_NAME _STLP_NON_DBG_NO_MEM_T_NAME(str)
#  define _STLP_NON_DBG_STRING_BASE _STLP_NON_DBG_STRING_BASE_NAME <_CharT, _Traits, _Alloc>
#  define basic_string _STLP_NO_MEM_T_NAME(str)
#else
#  define _STLP_NON_DBG_STRING_BASE_NAME _STLP_NON_DBG_NAME(str)
#  define _STLP_NON_DBG_STRING_BASE _STLP_NON_DBG_STRING_BASE_NAME <_CharT, _Traits, _Alloc>
#endif

_STLP_BEGIN_NAMESPACE

#ifdef _STLP_DEBUG_USE_DISTINCT_VALUE_TYPE_HELPERS
template <class _CharT,class _Traits, class _Alloc>
inline _CharT*
value_type(const  _DBG_iter_base< _STLP_NON_DBG_STRING_BASE >&) {
  return (_CharT*)0;
}
template <class _CharT, class _Traits, class _Alloc>
inline random_access_iterator_tag
iterator_category(const  _DBG_iter_base< _STLP_NON_DBG_STRING_BASE >&) {
  return random_access_iterator_tag();
}
#endif

template <class _CharT, class _Traits, class _Alloc> 
class basic_string : private _STLP_CONSTRUCT_CHECKER<_STLP_NON_DBG_STRING_BASE >
                   , public _STLP_NON_DBG_STRING_BASE
#if !defined(basic_string) && defined (_STLP_USE_PARTIAL_SPEC_WORKAROUND)
                   , public __stlport_class<basic_string<_CharT, _Traits, _Alloc> >
#endif
{
protected:
  typedef _STLP_NON_DBG_STRING_BASE _Base;
  typedef basic_string<_CharT, _Traits, _Alloc> _Self;
  typedef _STLP_CONSTRUCT_CHECKER<_STLP_NON_DBG_STRING_BASE > _ConstructCheck;
  typedef typename _Base::_Char_Is_POD _Char_Is_POD;

#if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  typedef typename _Base::_NonDbgBase _NonDbgBase;
#endif

public:
  __IMPORT_CONTAINER_TYPEDEFS(_Base)
  typedef _DBG_iter<_Base, _DbgTraits<_Nonconst_traits<value_type> > > iterator;
  typedef _DBG_iter<_Base, _DbgTraits<_Const_traits<value_type> > > const_iterator;
  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;

public:                         // Constructor, destructor, assignment.
  typedef typename _Base::_Reserve_t _Reserve_t;

protected:
  __owned_list _M_iter_list;
  _Base* _Get_base() { return this; }
  void _Invalidate_all() {
    _M_iter_list._Invalidate_all();
  }
  void _Compare_Capacity (size_type __old_capacity) {
    if (this->capacity() > __old_capacity) {
      _Invalidate_all();
    }
  }
  void _Invalidate_iterator(const iterator& __it) {
    __invalidate_iterator(&_M_iter_list, __it); 
  }
  void _Invalidate_iterators(const iterator& __f, const iterator& __l) {
    __invalidate_range(&_M_iter_list, __f, __l);
  }

public:
  explicit basic_string(const allocator_type& __a = allocator_type()) :
    _STLP_NON_DBG_STRING_BASE(__a), _M_iter_list(_Get_base()) {}

  basic_string(_Reserve_t __r, size_t __n,
               const allocator_type& __a = allocator_type()) :
    _STLP_NON_DBG_STRING_BASE(__r, __n, __a), _M_iter_list(_Get_base()) {}

  basic_string(const _Self& __s) :
     _ConstructCheck(__s),
     _STLP_NON_DBG_STRING_BASE(__s), _M_iter_list(_Get_base()) {}

  basic_string(const _Self& __s, size_type __pos, size_type __n = _Base::npos,
               const allocator_type& __a = allocator_type()) :
    _STLP_NON_DBG_STRING_BASE(__s, __pos, __n, __a), _M_iter_list(_Get_base()) {}

  basic_string(const _CharT* __s, size_type __n,
               const allocator_type& __a = allocator_type()) :
    _ConstructCheck(__s),
    _STLP_NON_DBG_STRING_BASE(__s, __n, __a), _M_iter_list(_Get_base()) {}

  basic_string(const _CharT* __s,
               const allocator_type& __a = allocator_type()) :
    _ConstructCheck(__s),
    _STLP_NON_DBG_STRING_BASE(__s, __a), _M_iter_list(_Get_base()) {}

  basic_string(size_type __n, _CharT __c,
               const allocator_type& __a = allocator_type()) :
    _STLP_NON_DBG_STRING_BASE(__n, __c, __a), _M_iter_list(_Get_base()) {}

  basic_string(__move_source<_Self> src) :
    _STLP_NON_DBG_STRING_BASE(__move_source<_Base >(src.get())), _M_iter_list(_Get_base()) {
    src.get()._Invalidate_all();
  }

#if !defined (_STLP_MEMBER_TEMPLATES) || defined(__MRC__) || defined(__SC__)
  basic_string(const _CharT* __f, const _CharT* __l,
               const allocator_type& __a = allocator_type()) :
    _ConstructCheck(__f, __l),
    _STLP_NON_DBG_STRING_BASE(__f, __l, __a), _M_iter_list(_Get_base()) {
  }
  basic_string(const_iterator __f, const_iterator __l, 
               const allocator_type & __a = allocator_type()) :
    _ConstructCheck(__f, __l),
    _STLP_NON_DBG_STRING_BASE(__f._M_iterator, __l._M_iterator, __a), _M_iter_list(_Get_base()) {
  }
#else
#  if !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  template <class _InputIterator>
  basic_string(_InputIterator __f, _InputIterator __l,
               const allocator_type & __a _STLP_ALLOCATOR_TYPE_DFL) :
    _ConstructCheck(__f, __l), 
    _STLP_NON_DBG_STRING_BASE(__f, __l, __a), _M_iter_list(_Get_base()) {
  }
#    if defined (_STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS)
  template <class _InputIterator>
  basic_string(_InputIterator __f, _InputIterator __l) :
    _ConstructCheck(__f, __l), 
    _STLP_NON_DBG_STRING_BASE(__f, __l), _M_iter_list(_Get_base()) {
  }
#    endif
#  else
protected:
    basic_string(_Base::_CalledFromWorkaround_t __x, const allocator_type& __a)
      : _STLP_NON_DBG_STRING_BASE(__x, __a), _M_iter_list(_Get_base()) {}
public:
#  endif
#endif

  // constructor from non-debug version
  basic_string (const _Base& __x)
    : _STLP_NON_DBG_STRING_BASE(__x), _M_iter_list(_Get_base()) {}

  _Self& operator=(const _Self& __s) {
    if (this != &__s) {
      _Invalidate_all();
      _Base::operator=(__s);
    }
    return *this;
  }

  _Self& operator=(const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    _Invalidate_all();
    _Base::operator=(__s);
    return *this;
  }

  _Self& operator=(_CharT __c) {
    _Invalidate_all();
    _Base::operator=(__c);
    return *this;
  }

public:                         // Iterators.

  iterator begin() { return iterator(&_M_iter_list, this->_M_Start()); }
  const_iterator begin() const { return const_iterator(&_M_iter_list,this->_M_Start()); }
  iterator end() { return iterator(&_M_iter_list,this->_M_Finish()); }
  const_iterator end() const { return const_iterator(&_M_iter_list,this->_M_Finish()); }
  void _M_deallocate_block() {
    _Invalidate_all();
    _Base::_M_deallocate_block();
  }

  reverse_iterator rbegin()             
    { return reverse_iterator(iterator(&_M_iter_list,this->_M_Finish())); }
  reverse_iterator rend()               
    { return reverse_iterator(iterator(&_M_iter_list,this->_M_Start())); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(const_iterator(&_M_iter_list,this->_M_Finish())); }
  const_reverse_iterator rend()   const 
    { return const_reverse_iterator(const_iterator(&_M_iter_list,this->_M_Start())); }

public:                         // Size, capacity, etc.

  void resize(size_type __n, _CharT __c) {
    if (__n > this->capacity())
      _Invalidate_all();
    else if ( __n < this->size() )
      _Invalidate_iterators(this->begin() + __n, this->end());
    _Base::resize(__n, __c);
  }
  void resize(size_type __n) { resize(__n, this->_M_null()); }

  void reserve(size_type __s= 0) {
    if (__s > this->capacity()) _Invalidate_all();
    _Base::reserve(__s);
  }

  void clear() {
    _Invalidate_all();
    _Base::clear();
  } 

public:                         // Element access.

  const_reference operator[](size_type __n) const {
    _STLP_VERBOSE_ASSERT(__n < this->size(), _StlMsg_OUT_OF_BOUNDS)
    return *(begin() + __n); 
  }
  reference operator[](size_type __n) {
    _STLP_VERBOSE_ASSERT(__n < this->size(), _StlMsg_OUT_OF_BOUNDS)
    return *(begin() + __n); 
  }

  const_reference at(size_type __n) const {
    if (__n >= this->size())
      this->_M_throw_out_of_range();
    return *(begin() + __n);
  }

  reference at(size_type __n) {
    if (__n >= this->size())
      this->_M_throw_out_of_range();
    return *(begin() + __n);
  }

public:                         // Append, operator+=, push_back.

  _Self& operator+=(const _Self& __s) { return append(__s); }
  _Self& operator+=(const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return append(__s); 
  }
  _Self& operator+=(_CharT __c) { push_back(__c); return *this; }

#if defined (_STLP_MEMBER_TEMPLATES) && !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  template <class _InputIter>
  _Self& append(_InputIter __first, _InputIter __last) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    size_type __old_capacity = this->capacity();
    _Base::append(__first, __last);
    _Compare_Capacity(__old_capacity);
    return *this;
  }
#endif

#if !defined (_STLP_MEMBER_TEMPLATES) || \
    !defined (_STLP_NO_METHOD_SPECIALIZATION) && !defined (_STLP_NO_EXTENSIONS)
  _Self& append(const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f) _STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_ptr_range(__f, __l))
    size_type __old_capacity = this->capacity();
    _Base::append(__f, __l);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& append(const_iterator __f, const_iterator __l) {
    _STLP_DEBUG_CHECK(__check_range(__f, __l))
    size_type __old_capacity = this->capacity();
    _Base::append(__f._M_iterator, __l._M_iterator);
    _Compare_Capacity(__old_capacity);
    return *this;
  }
#endif

public:
  _Self& append(const _Self& __s) {
    _M_append(__s._M_Start(), __s._M_Finish());
    return *this;
  }

  _Self& append(const _Self& __s, size_type __pos, size_type __n) {
    size_type __old_capacity = this->capacity();
    _Base::append(__s, __pos, __n);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& append(const _CharT* __s, size_type __n) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    _M_append(__s, __s + __n);
    return *this;
  }

  _Self& append(const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    _M_append(__s, __s + _Traits::length(__s));
    return *this;
  }

  _Self& append(size_type __n, _CharT __c) {
    size_type __old_capacity = this->capacity();
    _Base::append(__n, __c);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

protected:
  _Self& _M_append(const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f) _STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_ptr_range(__f, __l))
    size_type __old_capacity = this->capacity();
    _Base::_M_append(__f, __l);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

public:
  void push_back(_CharT __c) {
    size_type __old_capacity = this->capacity();
    _Base::push_back(__c);
    _Compare_Capacity(__old_capacity);
  }

  void pop_back() {
    _Invalidate_iterator(this->end());
    _Base::pop_back();
  }

public:                         // Assign  
  _Self& assign(const _Self& __s) {
    _Invalidate_all();
    _Base::assign(__s); 
    return *this; 
  }

  _Self& assign(const _Self& __s, size_type __pos, size_type __n) {
    _Invalidate_all();
    _Base::assign(__s, __pos, __n);
    return *this;
  }

  _Self& assign(const _CharT* __s, size_type __n) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    _M_assign(__s, __s + __n);
    return *this;
  }

  _Self& assign(const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    _M_assign(__s, __s + _Traits::length(__s));
    return *this;
  }

  _Self& assign(size_type __n, _CharT __c) {
    _Invalidate_all();
    _Base::assign(__n, __c);
    return *this;    
  }

#if defined(_STLP_MEMBER_TEMPLATES)&& !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  template <class _InputIter>
  inline _Self& assign(_InputIter __first, _InputIter __last) {
    _STLP_FIX_LITERAL_BUG(__first) _STLP_FIX_LITERAL_BUG(__last)
    _STLP_DEBUG_CHECK(__check_range(__first, __last))
    _Base::assign(__first, __last);
    return *this;    
  }
#endif

#if !defined (_STLP_MEMBER_TEMPLATES) || \
    !defined (_STLP_NO_METHOD_SPECIALIZATION) && !defined (_STLP_NO_EXTENSIONS)
  _Self& assign(const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f) _STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_ptr_range(__f, __l))
    _Invalidate_all();
    _Base::assign(__f, __l);
    return *this;
  }
  _Self& assign(const_iterator __f, const_iterator __l) {
    _STLP_DEBUG_CHECK(__check_range(__f, __l))
    _Invalidate_all();
    _Base::assign(__f._M_iterator, __l._M_iterator);
    return *this;
  }
#endif

protected:
  _Self& _M_assign(const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f) _STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_ptr_range(__f, __l))
    _Invalidate_all();
    _Base::_M_assign(__f, __l);
    return *this;
  }
    
public:                         // Insert

  _Self& insert(size_type __pos, const _Self& __s) {
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos, __s);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& insert(size_type __pos, const _Self& __s,
                size_type __beg, size_type __n) {
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos, __s, __beg, __n);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& insert(size_type __pos, const _CharT* __s, size_type __n) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos, __s, __n);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& insert(size_type __pos, const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return insert(__pos, __s, _Traits::length(__s));
  }
    
  _Self& insert(size_type __pos, size_type __n, _CharT __c) {
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos, __n, __c);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  iterator insert(iterator __p, _CharT __c) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__p))
    size_type __old_capacity = this->capacity();
    typename _Base::iterator __ret = _Base::insert(__p._M_iterator, __c);
    _Compare_Capacity(__old_capacity);
    return iterator(&_M_iter_list, __ret);
  }

  void insert(iterator __p, size_t __n, _CharT __c) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__p))
    size_type __old_capacity = this->capacity();
    _Base::insert(__p._M_iterator, __n, __c);
    _Compare_Capacity(__old_capacity);
  }

#if defined (_STLP_MEMBER_TEMPLATES) && !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
protected:
  template <class _RandomIter>
  void _M_insert_aux (iterator __p, _RandomIter __first, _RandomIter __last,
                      const __true_type& /*_IsIterator*/) {
    _Base::insert(__p._M_iterator, __first._M_iterator, __last._M_iterator);
  }
  
  template<class _InputIter>
  void _M_insert_aux (iterator __p, _InputIter __first, _InputIter __last,
                      const __false_type& /*_IsIterator*/) {
    _Base::insert(__p._M_iterator, __first, __last);
  }

public:
  template <class _InputIter>
  void insert(iterator __p, _InputIter __first, _InputIter __last) {
    _STLP_DEBUG_CHECK(__check_if_owner(&this->_M_iter_list,__p))
    _STLP_DEBUG_CHECK(__check_range(__first,__last))

    typedef typename _AreSameUnCVTypes<_InputIter, iterator>::_Ret _IsNonConstIterator;
    typedef typename _AreSameUnCVTypes<_InputIter, const_iterator>::_Ret _IsConstIterator;
    typedef typename _Lor2<_IsNonConstIterator, _IsConstIterator>::_Ret _IsIterator;

    size_type __old_capacity = this->capacity();
    _M_insert_aux(__p, __first, __last, _IsIterator());
    _Compare_Capacity(__old_capacity);
  }
#endif

#if !defined (_STLP_MEMBER_TEMPLATES) || \
    !defined (_STLP_NO_METHOD_SPECIALIZATION) && !defined (_STLP_NO_EXTENSIONS)
  void insert(iterator __p, const_iterator __f, const_iterator __l) {
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__p))
    _STLP_DEBUG_CHECK(__check_range(__f,__l))
    size_type __old_capacity = this->capacity();
    _Base::insert(__p._M_iterator, __f._M_iterator, __l._M_iterator); 
    _Compare_Capacity(__old_capacity);
  }
  void insert(iterator __p, const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f)_STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__p))
    _STLP_DEBUG_CHECK(__check_ptr_range(__f,__l))
    size_type __old_capacity = this->capacity();
    _Base::insert(__p._M_iterator, __f, __l); 
    _Compare_Capacity(__old_capacity);
  }
#endif

public:                         // Erase.
  _Self& erase(size_type __pos = 0, size_type __n = _Base::npos) {
    if (__pos < this->size()) {
      _Invalidate_iterators(begin() + __pos, end());
    }
    _Base::erase(__pos, __n);
    return *this;
  }
  iterator erase(iterator __pos) {
    _STLP_DEBUG_CHECK(_Dereferenceable(__pos))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list,__pos))
    _Invalidate_iterators(__pos, end());
    return iterator(&_M_iter_list, _Base::erase(__pos._M_iterator));
  }
  iterator erase(iterator __f, iterator __l) {
    _STLP_DEBUG_CHECK(__check_range(__f, __l, this->begin(), this->end()))
    _Invalidate_iterators(__f, end());
    return iterator(&_M_iter_list, _Base::erase(__f._M_iterator, __l._M_iterator));   
  }

public:                         // Substring.
  _Self substr(size_type __pos = 0, size_type __n = _Base::npos) const {
    return _Base::substr(__pos, __n);
  }

public:                         // Replace.  (Conceptually equivalent
                                // to erase followed by insert.)
  _Self& replace(size_type __pos, size_type __n, const _Self& __s) {
    size_type __old_capacity = this->capacity();
    _Base::replace(__pos, __n, __s);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(size_type __pos1, size_type __n1, const _Self& __s,
                 size_type __pos2, size_type __n2) {
    size_type __old_capacity = this->capacity();
    _Base::replace(__pos1, __n1, __s, __pos2, __n2);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(size_type __pos, size_type __n1, const _CharT* __s, size_type __n2) {    
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    size_type __old_capacity = this->capacity();
    _Base::replace(__pos, __n1, __s, __n2);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(size_type __pos, size_type __n1, const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    size_type __old_capacity = this->capacity();
    _Base::replace(__pos, __n1, __s);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(size_type __pos, size_type __n1, size_type __n2, _CharT __c) {
    size_type __old_capacity = this->capacity();
    _Base::replace(__pos, __n1, __n2, __c);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(iterator __f, iterator __l, const _Self& __s) {
    _STLP_DEBUG_CHECK(__check_range(__f, __l, this->begin(), this->end()))
    size_type __old_capacity = this->capacity();
    _Base::replace(__f._M_iterator, __l._M_iterator,__s);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(iterator __f, iterator __l, const _CharT* __s, size_type __n) { 
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_DEBUG_CHECK(__check_range(__f, __l, this->begin(), this->end()))
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    size_type __old_capacity = this->capacity();
    _Base::replace(__f._M_iterator, __l._M_iterator,__s, __n);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(iterator __f, iterator __l, const _CharT* __s) {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_DEBUG_CHECK(__check_range(__f, __l, this->begin(), this->end()))
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    size_type __old_capacity = this->capacity();
    _Base::replace(__f._M_iterator, __l._M_iterator,__s);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(iterator __f, iterator __l, size_type __n, _CharT __c) {
    _STLP_DEBUG_CHECK(__check_range(__f, __l, this->begin(), this->end()))
    size_type __old_capacity = this->capacity();
    _Base::replace(__f._M_iterator, __l._M_iterator, __n, __c);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

#if defined (_STLP_MEMBER_TEMPLATES) && !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
private:
  template <class _RandomIter>
  void _M_replace_aux(iterator __first, iterator __last,
                      _RandomIter __f, _RandomIter __l, __true_type const& /*_IsIterator*/) {
    _Base::replace(__first._M_iterator, __last._M_iterator, __f._M_iterator, __l._M_iterator);
  }
  
  template <class _InputIter>
  void _M_replace_aux(iterator __first, iterator __last,
                      _InputIter __f, _InputIter __l, __false_type const& /*_IsIterator*/) {
    _Base::replace(__first._M_iterator, __last._M_iterator, __f, __l);
  }  

public:
  template <class _InputIter>
  _Self& replace(iterator __first, iterator __last,
                 _InputIter __f, _InputIter __l) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last, this->begin(), this->end()))
    _STLP_DEBUG_CHECK(__check_range(__f, __l))

    typedef typename _AreSameUnCVTypes<_InputIter, iterator>::_Ret _IsNonConstIterator;
    typedef typename _AreSameUnCVTypes<_InputIter, const_iterator>::_Ret _IsConstIterator;
    typedef typename _Lor2<_IsNonConstIterator, _IsConstIterator>::_Ret _IsIterator;

    size_type __old_capacity = this->capacity();
    _M_replace_aux(__first, __last, __f, __l, _IsIterator());
    _Compare_Capacity(__old_capacity);
    return *this;
  }
#endif
 
#if !defined (_STLP_MEMBER_TEMPLATES) || \
    !defined (_STLP_NO_METHOD_SPECIALIZATION) && !defined (_STLP_NO_EXTENSIONS)
  _Self& replace(iterator __first, iterator __last,
                 const_iterator __f, const_iterator __l) {
    _STLP_DEBUG_CHECK(__check_range(__first, __last, this->begin(), this->end()))
    _STLP_DEBUG_CHECK(__check_range(__f, __l))
    size_type __old_capacity = this->capacity();
    _Base::replace(__first._M_iterator, __last._M_iterator, 
           	       __f._M_iterator, __l._M_iterator);
    _Compare_Capacity(__old_capacity);
    return *this;
  }

  _Self& replace(iterator __first, iterator __last,
                 const _CharT* __f, const _CharT* __l) {
    _STLP_FIX_LITERAL_BUG(__f)_STLP_FIX_LITERAL_BUG(__l)
    _STLP_DEBUG_CHECK(__check_range(__first, __last, this->begin(), this->end()))
    _STLP_DEBUG_CHECK(__check_ptr_range(__f, __l))
    size_type __old_capacity = this->capacity();
    _Base::replace(__first._M_iterator, __last._M_iterator, __f, __l);
    _Compare_Capacity(__old_capacity);
    return *this;
  }
#endif

public:                         // Other modifier member functions.
  void swap(_Self& __s) {
    _M_iter_list._Swap_owners(__s._M_iter_list);
    _Base::swap(__s);
  }

public:                        // Helper functions for compare.
  
  static int _STLP_CALL _M_compare(const _CharT* __f1, const _CharT* __l1,
                                   const _CharT* __f2, const _CharT* __l2) {
    return _Base::_M_compare(__f1, __l1, __f2, __l2);
  }
  static int _STLP_CALL _M_compare(const_iterator __f1, const_iterator __l1,
                                   const _CharT* __f2, const _CharT* __l2) {
    return _Base::_M_compare(__f1._M_iterator, __l1._M_iterator, __f2, __l2);
  }
  static int _STLP_CALL _M_compare(const _CharT* __f1, const _CharT* __l1,
                                   const_iterator __f2, const_iterator __l2) {
    return _Base::_M_compare(__f1, __l1, __f2._M_iterator, __l2._M_iterator);
  }
  static int _STLP_CALL _M_compare(const_iterator __f1, const_iterator __l1,
                                   const_iterator __f2, const_iterator __l2) {
    return _Base::_M_compare(__f1._M_iterator, __l1._M_iterator, 
                             __f2._M_iterator, __l2._M_iterator);
  }

  /*
   * Some of the following methods do not really have to be implemented here
   * (those without any debug support) as they already are in the base class
   * but some compilers have link trouble if they are missing so they are 
   * overloaded here.
   */
public:                         // find.
  size_type find(const _Self& __s, size_type __pos = 0) const 
  { return _Base::find(__s, __pos); }
  size_type find(const _CharT* __s, size_type __pos = 0) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find(__s, __pos); 
  }
  size_type find(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find(__s, __pos, __n); 
  }
  // WIE: Versant schema compiler 5.2.2 ICE workaround
  size_type find(_CharT __c) const { return find(__c, 0); }
  size_type find(_CharT __c, size_type __pos /* = 0 */) const 
  { return _Base::find(__c, __pos); }

public:                         // rfind.
  size_type rfind(const _Self& __s, size_type __pos = _Base::npos) const 
  { return _Base::rfind(__s, __pos); }
  size_type rfind(const _CharT* __s, size_type __pos = _Base::npos) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::rfind(__s, __pos); 
  }
  size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::rfind(__s, __pos, __n); 
  }
  size_type rfind(_CharT __c, size_type __pos = _Base::npos) const
  { return _Base::rfind(__c, __pos); }

public:                         // find_first_of
  
  size_type find_first_of(const _Self& __s, size_type __pos = 0) const 
  { return _Base::find_first_of(__s, __pos); }
  size_type find_first_of(const _CharT* __s, size_type __pos = 0) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_first_of(__s, __pos); 
  }
  size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_first_of(__s, __pos, __n); 
  }
  size_type find_first_of(_CharT __c, size_type __pos = 0) const
  { return _Base::find_first_of(__c, __pos); }

public:                         // find_last_of

  size_type find_last_of(const _Self& __s, size_type __pos = _Base::npos) const
  { return _Base::find_last_of(__s, __pos); }
  size_type find_last_of(const _CharT* __s, size_type __pos = _Base::npos) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_last_of(__s, __pos); 
  }
  size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_last_of(__s, __pos, __n);
  }
  size_type find_last_of(_CharT __c, size_type __pos = _Base::npos) const {
    return _Base::rfind(__c, __pos);
  }

public:                         // find_first_not_of

  size_type find_first_not_of(const _Self& __s, size_type __pos = 0) const 
  { return _Base::find_first_not_of(__s, __pos); }
  size_type find_first_not_of(const _CharT* __s, size_type __pos = 0) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_first_not_of(__s, __pos); 
  }
  size_type find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_first_not_of(__s, __pos, __n);
  }
  size_type find_first_not_of(_CharT __c, size_type __pos = 0) const 
  { return _Base::find_first_not_of(__c, __pos); }

public:                         // find_last_not_of

  size_type find_last_not_of(const _Self& __s, size_type __pos = _Base::npos) const
  { return _Base::find_last_not_of(__s, __pos); }
  size_type find_last_not_of(const _CharT* __s, size_type __pos = _Base::npos) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_last_not_of(__s, __pos); 
  }
  size_type find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const {
    _STLP_FIX_LITERAL_BUG(__s)
    _STLP_VERBOSE_ASSERT((__s != 0), _StlMsg_INVALID_ARGUMENT)
    return _Base::find_last_not_of(__s, __pos, __n);
  }
  size_type find_last_not_of(_CharT __c, size_type __pos = _Base::npos) const
  { return _Base::find_last_not_of(__c, __pos); }

#if defined (_STLP_USE_TEMPLATE_EXPRESSION) && !defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
#  define _STLP_STRING_SUM_BASE(__reserve, __size, __alloc) _STLP_NON_DBG_STRING_BASE(__reserve, __size, __alloc), \
          _M_iter_list(_Get_base())
#  define _STLP_STRING_BASE_SCOPE _Base::
#  include <stl/_string_sum_methods.h>
#  undef _STLP_STRING_BASE_SCOPE
#  undef _STLP_STRING_SUM_BASE
#endif /* _STLP_USE_TEMPLATE_EXPRESSION */
};


// This is a hook to instantiate STLport exports in a designated DLL
#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS _STLP_CONSTRUCT_CHECKER<_STLP_NON_DBG_STRING_BASE_NAME <char, char_traits<char>, allocator<char> > >;
_STLP_EXPORT_TEMPLATE_CLASS basic_string<char, char_traits<char>, allocator<char> >;
#  if defined (_STLP_HAS_WCHAR_T)
_STLP_EXPORT_TEMPLATE_CLASS _STLP_CONSTRUCT_CHECKER<_STLP_NON_DBG_STRING_BASE_NAME <wchar_t, char_traits<wchar_t>, allocator<wchar_t> > >;
_STLP_EXPORT_TEMPLATE_CLASS basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
#  endif
#endif /* _STLP_USE_TEMPLATE_EXPORT */


_STLP_END_NAMESPACE

#undef basic_string
#undef _STLP_NON_DBG_STRING_BASE
#undef _STLP_NON_DBG_STRING_BASE_NAME

#endif /* _STLP_DBG_STRING */

// Local Variables:
// mode:C++
// End:
