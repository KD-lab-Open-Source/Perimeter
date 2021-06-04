/*
 *
 * Copyright (c) 2003
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

#ifndef _STLP_SPECIALIZED_VECTOR_H
#define _STLP_SPECIALIZED_VECTOR_H

#ifndef _STLP_VOID_PTR_TRAITS_H
# include <stl/pointers/_void_ptr_traits.h>
#endif

/*
 * The general vector class.
 */
template <class _Tp, _STLP_DEFAULT_ALLOCATOR_SELECT(_Tp) >
class vector {
  typedef _Vector_impl<_Tp, _Alloc> _Base;
  typedef vector<_Tp, _Alloc> _Self;
public:
  _STLP_FORCE_ALLOCATORS(_Tp, _Alloc)
  
  __IMPORT_WITH_REVERSE_ITERATORS(_Base)
  typedef typename _Base::_Iterator_category _Iterator_category;
  
  allocator_type get_allocator() const { return _M_impl.get_allocator(); }
  
  iterator begin()             { return _M_impl.begin(); }
  const_iterator begin() const { return _M_impl.begin(); }
  iterator end()               { return _M_impl.end(); }
  const_iterator end() const   { return _M_impl.end(); }

  reverse_iterator rbegin()              { return _M_impl.rbegin(); }
  const_reverse_iterator rbegin() const  { return _M_impl.rbegin(); }
  reverse_iterator rend()                { return _M_impl.rend(); }
  const_reverse_iterator rend() const    { return _M_impl.rend(); }

  size_type size() const        { return _M_impl.size(); }
  size_type max_size() const    { return _M_impl.max_size(); }

  size_type capacity() const    { return _M_impl.capacity(); }
  bool empty() const            { return _M_impl.empty(); }

  reference operator[](size_type __n)             { return _M_impl[__n]; }
  const_reference operator[](size_type __n) const { return _M_impl[__n]; }

  reference front()             { return _M_impl.front(); }
  const_reference front() const { return _M_impl.front(); }
  reference back()              { return _M_impl.back(); }
  const_reference back() const  { return _M_impl.back(); }

  reference at(size_type __n)             { return _M_impl.at(__n); }
  const_reference at(size_type __n) const { return _M_impl.at(__n); }


  explicit vector(const allocator_type& __a = allocator_type()) : _M_impl(__a) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit vector(size_type __n, const_reference __val = _STLP_DEFAULT_CONSTRUCTED(value_type),
#else
  vector(size_type __n, const_reference __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
         const allocator_type& __a = allocator_type()) 
    : _M_impl(__n, __val, __a) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit vector(size_type __n) : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  vector(const _Self& __x) : _M_impl(__x._M_impl) {}

  vector(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

#if defined (_STLP_MEMBER_TEMPLATES)
  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last,
         const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL ) :
    _M_impl(__first, __last, __a) {}
  
# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last) :
    _M_impl(__first, __last) {}
# endif /* _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS */

#else /* _STLP_MEMBER_TEMPLATES */
  vector(const_pointer __first, const_pointer __last,
         const allocator_type& __a = allocator_type())
    : _M_impl(__first, __last, __a) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  _Self& operator=(const _Self& __x) {
    _M_impl = __x._M_impl; return *this;
  }

  void reserve(size_type __n) { _M_impl.reserve(__n); }
  void assign(size_type __n, const_reference __val) { _M_impl.assign(__n, __val); }
  
#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last)
#else
  void assign(const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  { _M_impl.assign(__first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back(const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  void push_back(const_reference __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_back(__x); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const_reference __x = _STLP_DEFAULT_CONSTRUCTED(value_type))
#else
  iterator insert(iterator __pos, const_reference __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { return _M_impl.insert(__pos, __x); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back() { _M_impl.push_back(); }
  iterator insert(iterator __pos) { return _M_impl.insert(__pos); }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void swap(_Self& __x) { _M_impl.swap(__x._M_impl); }

#if defined ( _STLP_MEMBER_TEMPLATES)
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) 
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  { _M_impl.insert(__pos, __first, __last); }
  
  void insert (iterator __pos, size_type __n, const_reference __x)
  { _M_impl.insert(__pos, __n, __x); }
  
  void pop_back() { _M_impl.pop_back(); }
  
  iterator erase(iterator __pos) 
  { return _M_impl.erase(__pos); }
  iterator erase(iterator __first, iterator __last) 
  { return _M_impl.erase(__first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const_reference __x = _STLP_DEFAULT_CONSTRUCTED(_Tp))
#else
  void resize(size_type __new_size, const_reference __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { _M_impl.resize(__new_size, __x); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void clear() { _M_impl.clear(); }
  
private:
  _Base _M_impl;
};

#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS _Vector_base<void*,allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _Vector_impl<void*,allocator<void*> >;
#endif

/*
 * The pointer partial specialization.
 */
template <class _Tp, class _Alloc>
class vector<_Tp*, _Alloc> {
  typedef typename _Alloc_traits<void*, _Alloc>::allocator_type _VoidAlloc;
  typedef _Vector_impl<void*, _VoidAlloc> _Base;
  typedef vector<_Tp*, _Alloc> _Self;
  typedef __void_ptr_traits<_Tp> cast_traits;
  
public:
  typedef _Tp* value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef random_access_iterator_tag _Iterator_category;

  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;
  _STLP_FORCE_ALLOCATORS(value_type, _Alloc)
  typedef typename _Alloc_traits<value_type, _Alloc>::allocator_type allocator_type;

  allocator_type get_allocator() const 
  { return _STLP_CONVERT_ALLOCATOR(_M_impl.get_allocator(), value_type); }

  iterator begin()             { return cast_traits::ptr_cast(_M_impl.begin()); }
  const_iterator begin() const { return cast_traits::const_ptr_cast(_M_impl.begin()); }
  iterator end()               { return cast_traits::ptr_cast(_M_impl.end()); }
  const_iterator end() const   { return cast_traits::const_ptr_cast(_M_impl.end()); }

  reverse_iterator rbegin()              { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const  { return const_reverse_iterator(end()); }
  reverse_iterator rend()                { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const    { return const_reverse_iterator(begin()); }

  size_type size() const        { return _M_impl.size(); }
  size_type max_size() const    { return _M_impl.max_size(); }

  size_type capacity() const    { return _M_impl.capacity(); }
  bool empty() const            { return _M_impl.empty(); }

  reference operator[](size_type __n) { return cast_traits::ref_cast(_M_impl[__n]); }
  const_reference operator[](size_type __n) const { return cast_traits::const_ref_cast(_M_impl[__n]); }

  reference front()             { return cast_traits::ref_cast(_M_impl.front()); }
  const_reference front() const { return cast_traits::const_ref_cast(_M_impl.front()); }
  reference back()              { return cast_traits::ref_cast(_M_impl.back()); }
  const_reference back() const  { return cast_traits::const_ref_cast(_M_impl.back()); }

  reference at(size_type __n) { return cast_traits::ref_cast(_M_impl.at(__n)); }
  const_reference at(size_type __n) const { return cast_traits::const_ref_cast(_M_impl.at(__n)); }

  explicit vector(const allocator_type& __a = allocator_type()) 
    : _M_impl(_STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit vector(size_type __n, value_type __val = 0,
#else
  vector(size_type __n, value_type __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
         const allocator_type& __a = allocator_type()) 
    : _M_impl(__n, cast_traits::cast(__val), _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit vector(size_type __n)
    : _M_impl(__n, allocator_type() ) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  vector(const _Self& __x) 
    : _M_impl(__x._M_impl) {}

  explicit vector(__move_source<_Self> src)
    : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}
  
#if defined (_STLP_MEMBER_TEMPLATES)
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last,
         const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL )
  : _M_impl(__iterator_wrapper<_Tp, _InputIterator>(__first), 
            __iterator_wrapper<_Tp, _InputIterator>(__last),
            _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

 # ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last)
    : _M_impl(__iterator_wrapper<_Tp, _InputIterator>(__first), 
              __iterator_wrapper<_Tp, _InputIterator>(__last)) {}
 # endif

#else
  vector(const_iterator __first, const_iterator __last,
         const allocator_type& __a = allocator_type())
    : _M_impl(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  _Self& operator=(const _Self& __x) { _M_impl = __x._M_impl; return *this; }

  void reserve(size_type __n) {_M_impl.reserve(__n);}
  void assign(size_type __n, value_type __val) { _M_impl.assign(__n, cast_traits::cast(__val)); }
  
#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    _M_impl.assign(__iterator_wrapper<_Tp, _InputIterator>(__first), __iterator_wrapper<_Tp, _InputIterator>(__last));
  }
#else
  void assign(const_iterator __first, const_iterator __last) {
    _M_impl.assign(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last));
  }
#endif /* _STLP_MEMBER_TEMPLATES */

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back(value_type __x = 0)
#else
  void push_back(value_type __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_back(cast_traits::cast(__x)); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, value_type __x = 0)
#else
  iterator insert(iterator __pos, value_type __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { return cast_traits::ptr_cast(_M_impl.insert(cast_traits::ptr_cast(__pos), cast_traits::cast(__x))); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_back() { _M_impl.push_back(); }
  iterator insert(iterator __pos)
  { return _M_impl.insert(cast_traits::ptr_cast(__pos)); }
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void swap(_Self& __x) {_M_impl.swap(__x._M_impl);}

#if defined ( _STLP_MEMBER_TEMPLATES)
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    _M_impl.insert(cast_traits::ptr_cast(__pos), __iterator_wrapper<_Tp, _InputIterator>(__first),
                                                 __iterator_wrapper<_Tp, _InputIterator>(__last));
  }
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const_iterator __first, const_iterator __last) {
    _M_impl.insert(cast_traits::ptr_cast(__pos), cast_traits::const_ptr_cast(__first),
                                                 cast_traits::const_ptr_cast(__last));
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  void insert (iterator __pos, size_type __n, value_type __x) {
    _M_impl.insert(cast_traits::ptr_cast(__pos), __n, cast_traits::cast(__x));
  }
  
  void pop_back() {_M_impl.pop_back();}
  iterator erase(iterator __pos) {return cast_traits::ptr_cast(_M_impl.erase(cast_traits::ptr_cast(__pos)));}
  iterator erase(iterator __first, iterator __last) {
    return cast_traits::ptr_cast(_M_impl.erase(cast_traits::ptr_cast(__first), 
                                               cast_traits::ptr_cast(__last)));
  }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, value_type __x = 0)
#else
  void resize(size_type __new_size, value_type __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { _M_impl.resize(__new_size, cast_traits::cast(__x)); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void clear() { _M_impl.clear();}
  
private:
  _Base _M_impl;
};


#endif /* _STLP_SPECIALIZED_VECTOR_H */
