/*
 *
 * Copyright (c) 2004
 * Francois Dumont
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

#ifndef _STLP_SPECIALIZED_DEQUE_H
#define _STLP_SPECIALIZED_DEQUE_H

#ifndef _STLP_VOID_PTR_TRAITS_H
# include <stl/pointers/_void_ptr_traits.h>
#endif

/*
 * The general deque class
 */
template <class _Tp, _STLP_DEFAULT_ALLOCATOR_SELECT(_Tp) >
class deque {
  typedef _Deque_impl<_Tp, _Alloc> _Base;
  typedef deque<_Tp, _Alloc> _Self;
public:
  _STLP_FORCE_ALLOCATORS(_Tp, _Alloc)
  
  __IMPORT_WITH_REVERSE_ITERATORS(_Base)
  typedef typename _Base::_Iterator_category _Iterator_category;

  allocator_type get_allocator() const { return _M_impl.get_allocator(); }
  
  iterator begin()             { return _M_impl.begin(); }
  iterator end()               { return _M_impl.end(); }
  const_iterator begin() const { return _M_impl.begin(); }
  const_iterator end() const   { return _M_impl.end(); }

  reverse_iterator rbegin()             { return _M_impl.rbegin(); }
  reverse_iterator rend()               { return _M_impl.rend(); }
  const_reverse_iterator rbegin() const { return _M_impl.rbegin(); }
  const_reverse_iterator rend() const   { return _M_impl.rend(); }

  reference operator[](size_type __n)             { return _M_impl[__n]; }
  const_reference operator[](size_type __n) const { return _M_impl[__n]; }

  reference at(size_type __n)             { return _M_impl.at(__n); }
  const_reference at(size_type __n) const { return _M_impl.at(__n); }

  reference front()             { return _M_impl.front(); }
  reference back()              { return _M_impl.back(); }
  const_reference front() const { return _M_impl.front(); }
  const_reference back() const  { return _M_impl.back(); }

  size_type size() const     { return _M_impl.size(); }
  size_type max_size() const { return _M_impl.max_size(); }
  bool empty() const         { return _M_impl.empty(); }

  explicit deque(const allocator_type& __a = allocator_type()) : _M_impl(__a) {}

  deque(const _Self& __x) : _M_impl(__x._M_impl) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit deque(size_type __n, const value_type& __val = _STLP_DEFAULT_CONSTRUCTED(value_type),
#else
  deque(size_type __n, const value_type& __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
        const allocator_type& __a = allocator_type()) : _M_impl(__n, __val, __a) {}
        
#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit deque(size_type __n) : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL) : _M_impl(__first, __last, __a) {}

#  ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last) : _M_impl(__first, __last) {}
#  endif

# else
  deque(const value_type* __first, const value_type* __last,
        const allocator_type& __a = allocator_type() ) : _M_impl(__first, __last, __a) {}
  deque(const_iterator __first, const_iterator __last,
        const allocator_type& __a = allocator_type() ) : _M_impl(__first, __last, __a) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  deque(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  _Self& operator= (const _Self& __x) { _M_impl = __x._M_impl; return *this; }

  void swap(_Self& __x) { _M_impl.swap(__x._M_impl); }

  void assign(size_type __n, const_reference __val) { _M_impl.assign(__n, __val); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last)
#else
  void assign(const value_type *__first, const value_type *__last)
  { _M_impl.assign(__first, __last); }
  void assign(const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  { _M_impl.assign(__first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back(const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  void push_back(const value_type& __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_back(__x); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front(const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  void push_front(const_reference __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_front(__x); }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back()  { _M_impl.push_back(); }
  void push_front() { _M_impl.push_front(); }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void pop_back()  { _M_impl.pop_back(); }
  void pop_front() { _M_impl.pop_front(); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  iterator insert(iterator __pos, const_reference __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { return _M_impl.insert(__pos, __x); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { return _M_impl.insert(__pos); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void insert(iterator __pos, size_type __n, const_reference __x) 
  { _M_impl.insert(__pos, __n, __x); }

#ifdef _STLP_MEMBER_TEMPLATES  
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last)
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos,
              const value_type* __first, const value_type* __last)
  { _M_impl.insert(__pos, __first, __last); }
  void insert(iterator __pos,
              const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  { _M_impl.insert(__pos, __first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  void resize(size_type __new_size, const_reference __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { _M_impl.resize(__new_size, __x); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  iterator erase(iterator __pos)                    { return _M_impl.erase(__pos); }
  iterator erase(iterator __first, iterator __last) { return _M_impl.erase(__first, __last); }
  void clear()                                      { _M_impl.clear(); }
  
private:
  _Base _M_impl;  
};

/*
 * The pointer partial specialization.
 */

 /*
  * struct helper to cast deque iterators:
  */
template <class _Tp>
struct _Ite_cast {
  typedef _Deque_iterator<_Tp*, _Nonconst_traits<_Tp*> > iterator;
  typedef _Deque_iterator<_Tp*, _Const_traits<_Tp*> >    const_iterator;
  typedef _Deque_iterator<void*, _Nonconst_traits<void*> > void_iterator;
  typedef _Deque_iterator<void*, _Const_traits<void*> > void_const_iterator;
  typedef __void_ptr_traits<_Tp> cast_traits;

  static iterator _M_cast (void_iterator const&__void_ite) {
    iterator tmp;
    tmp._M_cur = cast_traits::ptr_cast(__void_ite._M_cur);
    tmp._M_first = cast_traits::ptr_cast(__void_ite._M_first);
    tmp._M_last = cast_traits::ptr_cast(__void_ite._M_last);
    tmp._M_node = cast_traits::ptr_ptr_cast(__void_ite._M_node);
    return tmp;
  }
  static void_iterator _M_cast (iterator const&__t_ite) {
    void_iterator tmp;
    tmp._M_cur = cast_traits::ptr_cast(__t_ite._M_cur);
    tmp._M_first = cast_traits::ptr_cast(__t_ite._M_first);
    tmp._M_last = cast_traits::ptr_cast(__t_ite._M_last);
    tmp._M_node = cast_traits::ptr_ptr_cast(__t_ite._M_node);
    return tmp;
  }

  static const_iterator _M_ccast (void_const_iterator const&__void_ite) {
    const_iterator tmp;
    tmp._M_cur = cast_traits::ptr_cast(__void_ite._M_cur);
    tmp._M_first = cast_traits::ptr_cast(__void_ite._M_first);
    tmp._M_last = cast_traits::ptr_cast(__void_ite._M_last);
    tmp._M_node = cast_traits::ptr_ptr_cast(__void_ite._M_node);
    return tmp;
  }

  static void_const_iterator _M_ccast (const_iterator const&__t_ite) {
    void_const_iterator tmp;
    tmp._M_cur = cast_traits::ptr_cast(__t_ite._M_cur);
    tmp._M_first = cast_traits::ptr_cast(__t_ite._M_first);
    tmp._M_last = cast_traits::ptr_cast(__t_ite._M_last);
    tmp._M_node = cast_traits::ptr_ptr_cast(__t_ite._M_node);
    return tmp;
  }
};

template <>
struct _Ite_cast<void*> {
  typedef _Deque_iterator<void*, _Nonconst_traits<void*> > iterator;
  typedef _Deque_iterator<void*, _Const_traits<void*> > const_iterator;

  static iterator _M_cast (iterator const&ite) {
    return ite;
  }

  static const_iterator _M_ccast (const_iterator const&ite) {
    return ite;
  }
};

#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS _STLP_alloc_proxy<size_t, void*,  allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_alloc_proxy<void***, void**,  allocator<void**> >;
_STLP_EXPORT template struct _STLP_CLASS_DECLSPEC _Deque_iterator<void*, _Nonconst_traits<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _Deque_base<void*,allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _Deque_impl<void*,allocator<void*> >;
#endif

template <class _Tp, class _Alloc>
class deque<_Tp*, _Alloc> {
  typedef typename _Alloc_traits<void*, _Alloc>::allocator_type _VoidAlloc;
  typedef _Deque_impl<void*, _VoidAlloc> _Base;
  typedef deque<_Tp*, _Alloc> _Self;
  typedef __void_ptr_traits<_Tp> cast_traits;
  typedef _Ite_cast<_Tp> ite_cast_traits;

public:
  typedef _Tp* value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef random_access_iterator_tag _Iterator_category;
  _STLP_FORCE_ALLOCATORS(value_type, _Alloc)
  typedef typename _Alloc_traits<value_type, _Alloc>::allocator_type allocator_type;
  typedef _Deque_iterator<value_type, _Nonconst_traits<value_type> > iterator;
  typedef _Deque_iterator<value_type, _Const_traits<value_type> >    const_iterator;

  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;

public:                         // Basic accessors
  iterator begin() { return ite_cast_traits::_M_cast(_M_impl.begin()); }
  iterator end()   { return ite_cast_traits::_M_cast(_M_impl.end()); }
  const_iterator begin() const { return ite_cast_traits::_M_ccast(_M_impl.begin()); }
  const_iterator end() const   { return ite_cast_traits::_M_ccast(_M_impl.end()); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend()   { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const 
  { return const_reverse_iterator(end()); }
  const_reverse_iterator rend() const   
  { return const_reverse_iterator(begin()); }

  reference operator[](size_type __n)
  { return cast_traits::ref_cast(_M_impl[__n]); }
  const_reference operator[](size_type __n) const 
  { return cast_traits::const_ref_cast(_M_impl[__n]); }

  reference at(size_type __n)
  { return cast_traits::ref_cast(_M_impl.at(__n)); }
  const_reference at(size_type __n) const
  { return cast_traits::const_ref_cast(_M_impl.at(__n)); }

  reference front() { return cast_traits::ref_cast(_M_impl.front()); }
  reference back()  { return cast_traits::ref_cast(_M_impl.back()); }
  const_reference front() const { return cast_traits::const_ref_cast(_M_impl.front()); }
  const_reference back() const  { return cast_traits::const_ref_cast(_M_impl.back()); }

  size_type size() const     { return _M_impl.size(); }
  size_type max_size() const { return _M_impl.max_size(); }
  bool empty() const         { return _M_impl.empty(); }
  allocator_type get_allocator() const { return _STLP_CONVERT_ALLOCATOR(_M_impl.get_allocator(), value_type); }

  explicit deque(const allocator_type& __a = allocator_type()) 
    : _M_impl(_STLP_CONVERT_ALLOCATOR(__a, void*)) {}

  deque(const _Self& __x) : _M_impl(__x._M_impl) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit deque(size_type __n, value_type __val = 0,
#else
  deque(size_type __n, value_type __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
        const allocator_type& __a = allocator_type())
    : _M_impl(__n, cast_traits::cast(__val), _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
  // int,long variants may be needed 
#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit deque(size_type __n) : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES

# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last)
    : _M_impl(__first, __last) {}
# endif

  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL) 
    : _M_impl(__iterator_wrapper<_Tp, _InputIterator>(__first),
              __iterator_wrapper<_Tp, _InputIterator>(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
# else
  deque(const_pointer __first, const_pointer __last,
        const allocator_type& __a = allocator_type() ) 
    : _M_impl(cast_traits::const_ptr_cast(__first), 
              cast_traits::const_ptr_cast(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

  deque(const_iterator __first, const_iterator __last,
        const allocator_type& __a = allocator_type() ) 
    : _M_impl(ite_cast_traits::_M_ccast(__first), ite_cast_traits::_M_ccast(__last),
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  deque(__move_source<_Self> src)
    : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  _Self& operator= (const _Self& __x) { _M_impl = __x._M_impl; return *this; }

  void swap(_Self& __x) { _M_impl.swap(__x._M_impl); }

  void assign(size_type __n, value_type __val) {
    _M_impl.assign(__n, cast_traits::cast(__val));
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    _M_impl.assign(__iterator_wrapper<_Tp, _InputIterator>(__first),
                   __iterator_wrapper<_Tp, _InputIterator>(__last));
  }
#else
  void assign(const value_type *__first, const value_type *__last) 
  { _M_impl.assign(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last)); }
  void assign(const_iterator __first, const_iterator __last) 
  { _M_impl.assign(ite_cast_traits::_M_ccast(__first), ite_cast_traits::_M_ccast(__last)); }
#endif /* _STLP_MEMBER_TEMPLATES */

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back(value_type __t = 0)
#else
  void push_back(value_type __t)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_back(cast_traits::cast(__t)); }
#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front(value_type __t = 0)
#else
  void push_front(value_type __t)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_front(cast_traits::cast(__t)); }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back()  { _M_impl.push_back(); }
  void push_front() { _M_impl.push_front(); }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void pop_back()  { _M_impl.pop_back(); }
  void pop_front() { _M_impl.pop_front(); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, value_type __x = 0)
#else
  iterator insert(iterator __pos, value_type __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { return ite_cast_traits::_M_cast(_M_impl.insert(ite_cast_traits::_M_cast(__pos), cast_traits::cast(__x))); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { return insert(__pos, 0); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void insert(iterator __pos, size_type __n, value_type __x) 
  { _M_impl.insert(ite_cast_traits::_M_cast(__pos), __n, cast_traits::cast(__x)); }

#ifdef _STLP_MEMBER_TEMPLATES  
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    _M_impl.insert(ite_cast_traits::_M_cast(__pos), __iterator_wrapper<_Tp, _InputIterator>(__first),
                                                    __iterator_wrapper<_Tp, _InputIterator>(__last));
  }

#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos,
              const_pointer __first, const_pointer __last) {
    _M_impl.insert(ite_cast_traits::_M_cast(__pos), cast_traits::const_ptr_cast(__first),
                                       cast_traits::const_ptr_cast(__last));
  }
  void insert(iterator __pos,
              const_iterator __first, const_iterator __last) {
    _M_impl.insert(ite_cast_traits::_M_cast(__pos), ite_cast_traits::_M_ccast(__first),
                                                    ite_cast_traits::_M_ccast(__last));
  }

#endif /* _STLP_MEMBER_TEMPLATES */

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, value_type __x = 0)
#else
  void resize(size_type __new_size, value_type __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { _M_impl.resize(__new_size, cast_traits::cast(__x)); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  iterator erase(iterator __pos) 
  { return ite_cast_traits::_M_cast(_M_impl.erase(ite_cast_traits::_M_cast(__pos))); }

  iterator erase(iterator __first, iterator __last) 
  { return ite_cast_traits::_M_cast(_M_impl.erase(ite_cast_traits::_M_cast(__first), 
                                                  ite_cast_traits::_M_cast(__last))); }
  void clear() { _M_impl.clear(); }
  
private:
  _Base _M_impl;
};

#endif /* _STLP_SPECIALIZED_DEQUE_H */

// Local Variables:
// mode:C++
// End:
