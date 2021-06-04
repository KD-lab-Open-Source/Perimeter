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

#ifndef _STLP_INTERNAL_DBG_VECTOR_H
#define _STLP_INTERNAL_DBG_VECTOR_H

#include <stl/debug/_iterator.h>

// # ifndef _STLP_USE_WRAPPER_FOR_ALLOC_PARAM
#  undef  _DBG_vector
#  define _DBG_vector vector
// # endif

#  define _STLP_DBG_VECTOR_BASE __WORKAROUND_DBG_RENAME(vector) <_Tp, _Alloc>

_STLP_BEGIN_NAMESPACE

# ifdef _STLP_DEBUG_USE_DISTINCT_VALUE_TYPE_HELPERS
template <class _Tp, class _Alloc>
inline _Tp*
value_type(const  _DBG_iter_base< _STLP_DBG_VECTOR_BASE >&) {
  return (_Tp*)0;
}
template <class _Tp, class _Alloc>
inline random_access_iterator_tag
iterator_category(const  _DBG_iter_base< _STLP_DBG_VECTOR_BASE >&) {
  return random_access_iterator_tag();
}
# endif

template <class _Tp, class _NcIt>
struct _Vector_const_traits;

template <class _Tp, class _NcIt>
struct _Vector_nonconst_traits {
  typedef _Nonconst_traits<_Tp> _BaseT;
  typedef _Tp value_type;
  typedef _Tp& reference;
  typedef _Tp* pointer;
  typedef _Vector_const_traits<_Tp, _NcIt> _ConstTraits;
  typedef _Vector_nonconst_traits<_Tp, _NcIt> _NonConstTraits;
};

template <class _Tp, class _NcIt>
struct _Vector_const_traits {
  typedef _Const_traits<_Tp> _BaseT;
  typedef _Tp value_type;
  typedef const _Tp& reference;
  typedef const _Tp* pointer;
  typedef _Vector_const_traits<_Tp, _NcIt> _ConstTraits;
  typedef _Vector_nonconst_traits<_Tp, _NcIt> _NonConstTraits;
};

_STLP_TEMPLATE_NULL
struct _Vector_nonconst_traits<bool, _Bit_iterator> {
  typedef _Bit_iterator::value_type value_type;
  typedef _Bit_iterator::reference reference;
  typedef _Bit_iterator::pointer pointer;
  typedef _Vector_const_traits<bool, _Bit_iterator> _ConstTraits;
  typedef _Vector_nonconst_traits<bool, _Bit_iterator> _NonConstTraits;
};

_STLP_TEMPLATE_NULL
struct _Vector_const_traits<bool, _Bit_iterator> {
  typedef _Bit_const_iterator::value_type value_type;
  typedef _Bit_const_iterator::reference reference;
  typedef _Bit_const_iterator::pointer pointer;
  typedef _Vector_const_traits<bool, _Bit_iterator> _ConstTraits;
  typedef _Vector_nonconst_traits<bool, _Bit_iterator> _NonConstTraits;
};

/*
 * TODO: Split the vector implementation in 2 classes, one with no member templates
 * that will be exported and the other with the member templates.
 * Once done we will be able to handle the iterator range validy check on constructor
 * correctly without problem under MSVC6.
 */
template <class _Tp, _STLP_DBG_ALLOCATOR_SELECT(_Tp) >
class _DBG_vector : private __construct_checker<_STLP_DBG_VECTOR_BASE >,
                    public _STLP_DBG_VECTOR_BASE
#if defined (_STLP_USE_PARTIAL_SPEC_WORKAROUND)
                     , public __stlport_class<_DBG_vector<_Tp, _Alloc> >
#endif
{
private:
  typedef _STLP_DBG_VECTOR_BASE _Base;
  typedef _DBG_vector<_Tp, _Alloc> _Self;
  typedef __construct_checker<_STLP_DBG_VECTOR_BASE > _ConstructCheck;
  __owned_list _M_iter_list;

public:

  __IMPORT_CONTAINER_TYPEDEFS(_Base)

  typedef _DBG_iter<_Base,
      _DbgTraits<_Vector_nonconst_traits<value_type, typename _Base::iterator> > > iterator;
    
  typedef _DBG_iter<_Base,
      _DbgTraits<_Vector_const_traits<value_type, typename _Base::iterator> > > const_iterator;

protected:
  _Base* _Get_base() { return this; }
  void _Invalidate_all() {
    _M_iter_list._Invalidate_all();
  }
  void _Invalidate_iterator(const iterator& __it) {
    __invalidate_iterator(&_M_iter_list, __it); 
  }
  void _Invalidate_iterators(const iterator& __first, const iterator& __last) {
    __invalidate_range(&_M_iter_list, __first, __last);
  }
  void _Check_Overflow(size_type __nb) {
    if (this->size()+__nb > this->capacity())
      _Invalidate_all();  
  }
  void _Compare_Capacity (size_type __old_capacity) {
    if (this->capacity() > __old_capacity) {
      _Invalidate_all();
    }
  }

public:
  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;

  iterator begin()             { return iterator(&_M_iter_list, _Base::begin()); }
  const_iterator begin() const { return const_iterator(&_M_iter_list, _Base::begin()); }
  iterator end()               { return iterator(&_M_iter_list, _Base::end()); }
  const_iterator end() const   { return const_iterator(&_M_iter_list, _Base::end()); }

  reverse_iterator rbegin()             { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  reverse_iterator rend()               { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }

  reference operator[](size_type __n) {
    _STLP_VERBOSE_ASSERT(__n < _Base::size(), _StlMsg_OUT_OF_BOUNDS)
    return _Base::operator[](__n);
  }

  const_reference operator[](size_type __n) const {
    _STLP_VERBOSE_ASSERT(__n < _Base::size(), _StlMsg_OUT_OF_BOUNDS)
    return _Base::operator[](__n);
  }

public:
  explicit _DBG_vector(const allocator_type& __a = allocator_type())
    : _STLP_DBG_VECTOR_BASE(__a), _M_iter_list(_Get_base())  {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_vector(size_type __n, const _Tp& __x = _Tp(),
#else
  _DBG_vector(size_type __n, const _Tp& __x,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
         const allocator_type& __a = allocator_type()) 
    : _STLP_DBG_VECTOR_BASE(__n, __x, __a), _M_iter_list(_Get_base()) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit _DBG_vector(size_type __n)
    : _STLP_DBG_VECTOR_BASE(__n), _M_iter_list(_Get_base()) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  _DBG_vector(const _Self& __x) 
    : _ConstructCheck(__x), _STLP_DBG_VECTOR_BASE(__x), _M_iter_list(_Get_base()) {}

  _DBG_vector(__move_source<_Self> src)
    : _STLP_DBG_VECTOR_BASE(__move_source<_Base>(src.get())), _M_iter_list(_Get_base()) {
    src.get()._Invalidate_all();
  }

#if defined (_STLP_MEMBER_TEMPLATES)
  template <class _InputIterator>
  _DBG_vector(_InputIterator __first, _InputIterator __last,
         const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL) 
    : _STLP_DBG_VECTOR_BASE(__first, __last, __a),
      _M_iter_list(_Get_base()) {
    }


# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  _DBG_vector(_InputIterator __first, _InputIterator __last)
    : _STLP_DBG_VECTOR_BASE(__first, __last), 
      _M_iter_list(_Get_base()) {
    }
# endif
#else
  _DBG_vector(const _Tp* __first, const _Tp* __last,
              const allocator_type& __a = allocator_type())
    : _ConstructCheck(__first, __last), _STLP_DBG_VECTOR_BASE(__first, __last, __a),
      _M_iter_list(_Get_base()) {
    }

  // mysterious VC++ bug ?
  _DBG_vector(const_iterator __first, const_iterator __last , 
              const allocator_type& __a = allocator_type())
    : _ConstructCheck(__first, __last), 
      _STLP_DBG_VECTOR_BASE(__first._M_iterator, __last._M_iterator, __a), 
      _M_iter_list(_Get_base()) {
      }

#endif /* _STLP_MEMBER_TEMPLATES */

  _Self& operator=(const _Self& __x) {
    if (this != &__x) {
      _Invalidate_all();
      _Base::operator=((const _Base&)__x);
    }
    return *this;
  }

  void reserve(size_type __n) {
    if (this->capacity() < __n)
      _Invalidate_all();
    _Base::reserve(__n);
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

  void swap(_Self& __x) {
    _M_iter_list._Swap_owners(__x._M_iter_list);
    _Base::swap((_Base&)__x);
  }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos, const _Tp& __x = _Tp()) {
#else
  iterator insert(iterator __pos, const _Tp& __x) {
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _Check_Overflow(1);
    return iterator(&_M_iter_list, _Base::insert(__pos._M_iterator, __x));
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos) {
    return insert(__pos, _STLP_DEFAULT_CONSTRUCTED(_Tp));
  }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  void insert(iterator __pos, 
              _InputIterator __first, _InputIterator __last) {
    typedef typename _AreSameUnCVTypes<_InputIterator, iterator>::_Ret _IsNonConstIterator;
    typedef typename _AreSameUnCVTypes<_InputIterator, const_iterator>::_Ret _IsConstIterator;
    typedef typename _Lor2<_IsNonConstIterator, _IsConstIterator>::_Ret _DoCheck;
    _STLP_DEBUG_CHECK(__check_range(__first,__last))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    //Sequence requirements 23.1.1 Table 67:
    _STLP_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, _DoCheck()));
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos._M_iterator, __first, __last);  
    _Compare_Capacity(__old_capacity);
}
#else /* _STLP_MEMBER_TEMPLATES */
  void insert (iterator __pos, 
               const value_type *__first, const value_type *__last) {
    _STLP_DEBUG_CHECK(__check_ptr_range(__first,__last))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos._M_iterator, __first, __last);        
    _Compare_Capacity(__old_capacity);
  }

  void insert(iterator __pos,
              const_iterator __first, const_iterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first,__last))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    //Sequence requirements 23.1.1 Table 67:
    _STLP_DEBUG_CHECK(__check_if_not_owner(&_M_iter_list, __first, __true_type()));
    size_type __old_capacity = this->capacity();
    _Base::insert(__pos._M_iterator, __first._M_iterator, __last._M_iterator);  
    _Compare_Capacity(__old_capacity);
}
#endif /* _STLP_MEMBER_TEMPLATES */

  void insert (iterator __pos, size_type __n, const _Tp& __x){
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _Check_Overflow(__n);
    _Base::insert(__pos._M_iterator, __n, __x);
  }
  
  void pop_back() {
    _STLP_VERBOSE_ASSERT(!this->empty(), _StlMsg_EMPTY_CONTAINER)
    _Invalidate_iterator(this->end());
    _Base::pop_back();
  }
  iterator erase(iterator __pos) {
    _STLP_DEBUG_CHECK(_Dereferenceable(__pos))
    _STLP_DEBUG_CHECK(__check_if_owner(&_M_iter_list, __pos))
    _Invalidate_iterators(__pos, end());
    return iterator(&_M_iter_list,_Base::erase(__pos._M_iterator));
  }
  iterator erase(iterator __first, iterator __last) {
    _STLP_DEBUG_CHECK(__check_range(__first,__last, this->begin(), this->end()))
    _Invalidate_iterators(__first, end());
    return iterator(&_M_iter_list, _Base::erase(__first._M_iterator, __last._M_iterator));
  }
  void clear() { 
    _Invalidate_all();
    _Base::clear();
  }
  void push_back(const _Tp& __x) {
    _Check_Overflow(1);
    _Base::push_back(__x);
  }
};

#define _STLP_TEMPLATE_HEADER template <class _Tp, class _Alloc>
#define _STLP_TEMPLATE_CONTAINER _DBG_vector<_Tp, _Alloc>
#define _STLP_TEMPLATE_CONTAINER_BASE _STLP_DBG_VECTOR_BASE
#include <stl/debug/_relops_cont.h>
#undef _STLP_TEMPLATE_CONTAINER_BASE
#undef _STLP_TEMPLATE_CONTAINER
#undef _STLP_TEMPLATE_HEADER

#ifdef _STLP_CLASS_PARTIAL_SPECIALIZATION
template <class _Tp, class _Alloc>
struct __move_traits<vector<_Tp, _Alloc> > :
  __move_traits_aux<_STLP_DBG_VECTOR_BASE >
{};
#endif /* _STLP_CLASS_PARTIAL_SPECIALIZATION */

_STLP_END_NAMESPACE

#undef _STLP_DBG_VECTOR_BASE
#undef _DBG_vector

#endif /* _STLP_DBG_VECTOR_H */

// Local Variables:
// mode:C++
// End:
