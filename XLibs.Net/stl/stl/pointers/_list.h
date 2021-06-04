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

#ifndef _STLP_SPECIALIZED_LIST_H
#define _STLP_SPECIALIZED_LIST_H

#ifndef _STLP_VOID_PTR_TRAITS_H
# include <stl/pointers/_void_ptr_traits.h>
#endif

/*
 * The general list class.
 */
template <class _Tp, _STLP_DEFAULT_ALLOCATOR_SELECT(_Tp)>
class list {
  typedef _List_impl<_Tp, _Alloc> _Base;
  typedef list<_Tp, _Alloc> _Self;
public:
  _STLP_FORCE_ALLOCATORS(_Tp, _Alloc)
  
  __IMPORT_WITH_REVERSE_ITERATORS(_Base)
  typedef typename _Base::_Iterator_category _Iterator_category;
  
  allocator_type get_allocator() const { return _M_impl.get_allocator(); }
  
#if !(defined(__MRC__)||(defined(__SC__) && !defined(__DMC__)))
  explicit
#endif
  list(const allocator_type& __a = allocator_type()) :
    _M_impl(__a) {}
    
#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit list(size_type __n, const value_type& __val = value_type(),
#else
  list(size_type __n, const value_type& __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
       const allocator_type& __a = allocator_type())
    : _M_impl(__n, __val, __a) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit list(size_type __n)
    : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL)
    : _M_impl(__first, __last, __a) {}
  
#  ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last)
    : _M_impl(__first, __last) {}
#  endif

#else /* _STLP_MEMBER_TEMPLATES */
  list(const value_type* __first, const value_type* __last,
       const allocator_type& __a = allocator_type())
    : _M_impl(__first, __last, __a) {}
  list(const_iterator __first, const_iterator __last,
       const allocator_type& __a = allocator_type())
    : _M_impl(__first, __last, __a) {}

#endif /* _STLP_MEMBER_TEMPLATES */
  list(const _Self& __x) :         _M_impl(__x._M_impl) {}
  list(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  iterator begin()                      { return _M_impl.begin(); }
  const_iterator begin() const          { return _M_impl.begin(); }
  iterator end()                        { return _M_impl.end(); }
  const_iterator end() const            { return _M_impl.end(); }

  reverse_iterator rbegin()             { return _M_impl.rbegin(); }
  const_reverse_iterator rbegin() const { return _M_impl.rbegin(); }
  reverse_iterator rend()               { return _M_impl.rend(); }
  const_reverse_iterator rend() const   { return _M_impl.rend(); }

  bool empty() const                    { return _M_impl.empty(); }
  size_type size() const                { return _M_impl.size(); }
  size_type max_size() const            { return _M_impl.max_size(); }

  reference front()                     { return _M_impl.front(); }
  const_reference front() const         { return _M_impl.front(); }
  reference back()                      { return _M_impl.back(); }
  const_reference back() const          { return _M_impl.back(); }

  _Self& operator=(const _Self& __x)    { _M_impl = __x._M_impl; return *this; }
  void swap(_Self& __x)                 { _M_impl.swap(__x._M_impl); }
  void clear()                          { _M_impl.clear(); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const value_type& __x = value_type()) {
#else
  iterator insert(iterator __pos, const value_type& __x) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    return _M_impl.insert(__pos, __x);
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const value_type* __first, const value_type* __last) {
    _M_impl.insert(__pos, __first, __last);
  }
  void insert(iterator __pos, const_iterator __first, const_iterator __last) {
#endif /* _STLP_MEMBER_TEMPLATES */
    _M_impl.insert(__pos, __first, __last);
  }
  
  void insert(iterator __pos, size_type __n, const value_type& __x) { _M_impl.insert(__pos, __n, __x); }
  
  void push_front(const value_type& __x) { _M_impl.push_front(__x); }
  void push_back (const value_type& __x) { _M_impl.push_back(__x); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { return _M_impl.insert(__pos); }
  void push_front() { _M_impl.push_front(); }
  void push_back()  { _M_impl.push_back(); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  iterator erase(iterator __pos)                    { return _M_impl.erase(__pos); }
  iterator erase(iterator __first, iterator __last) { return _M_impl.erase(__first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const value_type& __x = value_type()) {
#else
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
  void resize(size_type __new_size, const value_type& __x) {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM*/
    _M_impl.resize(__new_size, __x);
  }

  void pop_front()  { _M_impl.pop_front(); }
  void pop_back()   { _M_impl.pop_back(); }
  
  void assign(size_type __n, const value_type& __val) { _M_impl.assign(__n, __val); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last)
#else
  void assign(const value_type *__first, const value_type *__last) 
  { _M_impl.assign(__first, __last); }
  void assign(const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  { _M_impl.assign(__first, __last); }

  void splice(iterator __pos, _Self& __x)               
  { _M_impl.splice(__pos, __x._M_impl); }
  void splice(iterator __pos, _Self& __x, iterator __i) 
  { _M_impl.splice(__pos, __x._M_impl, __i); }
  void splice(iterator __pos, _Self& __x, iterator __first, iterator __last) 
  { _M_impl.splice(__pos, __x._M_impl, __first, __last); }

  void remove(const value_type& __val) { _M_impl.remove(__val); }
  void unique()                        { _M_impl.unique(); }
  void merge(_Self& __x)               { _M_impl.merge(__x._M_impl); }
  void reverse()                       { _M_impl.reverse(); }
  void sort()                          { _M_impl.sort(); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _Predicate> 
  void remove_if(_Predicate __pred)           
  { _M_impl.remove_if(__pred); }
  template <class _BinaryPredicate>
  void unique(_BinaryPredicate __binary_pred) 
  { _M_impl.unique(__binary_pred); }
  template <class _StrictWeakOrdering>
  void merge(_Self& __x, _StrictWeakOrdering __comp) 
  { _M_impl.merge(__x._M_impl, __comp); }
  template <class _StrictWeakOrdering>
  void sort(_StrictWeakOrdering __comp) 
  { _M_impl.sort(__comp); }
#endif /* _STLP_MEMBER_TEMPLATES */
private:
  _Base _M_impl;
};

#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS _List_node<void*>;
typedef _List_node<void*> _VoidPtr_Node;
_STLP_EXPORT_TEMPLATE_CLASS allocator<_VoidPtr_Node>;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_alloc_proxy<_List_node_base, _VoidPtr_Node, allocator<_VoidPtr_Node> >;
_STLP_EXPORT_TEMPLATE_CLASS _List_base<void*, allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _List_impl<void*, allocator<void*> >;
#endif

/*
 * The pointer partial specialization.
 */
template <class _Tp, class _Alloc>
class list<_Tp*, _Alloc> {
  typedef typename _Alloc_traits<void*, _Alloc>::allocator_type _VoidAlloc;
  typedef _List_impl<void*, _VoidAlloc> _Base;
  typedef __void_ptr_traits<_Tp> cast_traits;
  typedef list<_Tp*, _Alloc> _Self;

public:
  typedef _Tp* value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  _STLP_FORCE_ALLOCATORS(value_type, _Alloc)
  typedef typename _Alloc_traits<_Tp*, _Alloc>::allocator_type allocator_type;
  typedef bidirectional_iterator_tag _Iterator_category;

  typedef _List_iterator<_Tp*, _Nonconst_traits<_Tp*> > iterator;
  typedef _List_iterator<_Tp*, _Const_traits<_Tp*> >    const_iterator;
  _STLP_DECLARE_BIDIRECTIONAL_REVERSE_ITERATORS;
  
  allocator_type get_allocator() const { return _STLP_CONVERT_ALLOCATOR(_M_impl.get_allocator(), value_type); }

# if !(defined(__MRC__)||(defined(__SC__) && !defined(__DMC__)))
  explicit
# endif
  list(const allocator_type& __a = allocator_type())
    : _M_impl(_STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit list(size_type __n, value_type __val = 0,
#else
  list(size_type __n, value_type __val,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
       const allocator_type& __a = allocator_type())
    : _M_impl(__n, cast_traits::cast(__val), _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit list(size_type __n)
    : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.
# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last)
    : _M_impl(__iterator_wrapper<_Tp, _InputIterator>(__first), 
              __iterator_wrapper<_Tp, _InputIterator>(__last)) {}
# endif
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL)
    : _M_impl(__iterator_wrapper<_Tp, _InputIterator>(__first), 
              __iterator_wrapper<_Tp, _InputIterator>(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#else /* _STLP_MEMBER_TEMPLATES */

  list(const value_type *__first, const value_type *__last,
       const allocator_type& __a = allocator_type())
    : _M_impl(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last),
               _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
  list(const_iterator __first, const_iterator __last,
       const allocator_type& __a = allocator_type())
    : _M_impl(__first._M_node, __last._M_node, _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#endif /* _STLP_MEMBER_TEMPLATES */
  list(const _Self& __x) : _M_impl(__x._M_impl) {}

  list(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  iterator begin()             { return _M_impl.begin()._M_node; }
  const_iterator begin() const { return _M_impl.begin()._M_node; }

  iterator end()               { return _M_impl.end()._M_node; }
  const_iterator end() const   { return _M_impl.end()._M_node; }

  reverse_iterator rbegin()             { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

  reverse_iterator rend()               { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }

  bool empty() const         { return _M_impl.empty(); }
  size_type size() const     { return _M_impl.size(); }
  size_type max_size() const { return _M_impl.max_size(); }

  reference front()             { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back()              { return *(--end()); }
  const_reference back() const  { return *(--end()); }

  void swap(_Self &__x) { _M_impl.swap(__x._M_impl); }
  void clear() { _M_impl.clear(); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos, const_reference __x = 0)
#else
  iterator insert(iterator __pos, const_reference __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { return _M_impl.insert(__pos._M_node, cast_traits::cast(__x))._M_node; }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    _M_impl.insert(__pos._M_node, __iterator_wrapper<_Tp, _InputIterator>(__first), 
                                  __iterator_wrapper<_Tp, _InputIterator>(__last));
  }
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const value_type *__first, const value_type *__last)
  { _M_impl.insert(__pos._M_node, cast_traits::const_ptr_cast(__first), 
                                  cast_traits::const_ptr_cast(__last)); }
  void insert(iterator __pos, const_iterator __first, const_iterator __last)
  { _M_impl.insert(__pos._M_node, __first._M_node, __last._M_node); }
#endif /* _STLP_MEMBER_TEMPLATES */

  void insert(iterator __pos, size_type __n, value_type __x)
  { _M_impl.insert(__pos._M_node, __n, cast_traits::cast(__x)); }

  void push_front(value_type __x) { _M_impl.push_front(cast_traits::cast(__x)); }
  void push_back(value_type __x)  { _M_impl.push_back(cast_traits::cast(__x)); }

#if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  iterator insert(iterator __pos) { return _M_impl.insert(__pos._M_node)._M_node; }
  void push_front() { _M_impl.push_front();}
  void push_back()  { _M_impl.push_back();}
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  iterator erase(iterator __pos) { return _M_impl.erase(__pos._M_node)._M_node; }
  iterator erase(iterator __first, iterator __last) { return _M_impl.erase(__first._M_node, __last._M_node)._M_node; }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, value_type __x = 0)
#else
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
  void resize(size_type __new_size, value_type __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM*/
  {_M_impl.resize(__new_size, cast_traits::cast(__x));}

  void pop_front() { _M_impl.pop_front(); }
  void pop_back()  { _M_impl.pop_back(); }

  _Self& operator=(const _Self& __x) {_M_impl = __x._M_impl; return *this;}
  void assign(size_type __n, value_type __val) { _M_impl.assign(__n, cast_traits::cast(__val)); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    _M_impl.assign(__iterator_wrapper<_Tp, _InputIterator>(__first),
                   __iterator_wrapper<_Tp, _InputIterator>(__last));
  }
#else
  void assign(const value_type *__first, const value_type *__last) {
    _M_impl.assign(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last));
  }
  void assign(const_iterator __first, const_iterator __last) {
    _M_impl.assign(__first._M_node, __last._M_node);
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  void splice(iterator __pos, _Self& __x) { _M_impl.splice(__pos._M_node, __x._M_impl); }
  void splice(iterator __pos, _Self& __x, iterator __i) { _M_impl.splice(__pos._M_node, __x._M_impl, __i._M_node); }
  void splice(iterator __pos, _Self& __x, iterator __first, iterator __last) 
  { _M_impl.splice(__pos._M_node, __x._M_impl, __first._M_node, __last._M_node); }

  void remove(const_reference __val) { _M_impl.remove(cast_traits::cast(__val)); }
  void unique() { _M_impl.unique(); }
  void merge(_Self& __x) { _M_impl.merge(__x._M_impl); }
  void reverse() { _M_impl.reverse(); }
  void sort() { _M_impl.sort(); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _Predicate>
  void remove_if(_Predicate __pred)  { _M_impl.remove_if(__unary_pred_wrapper<_Tp, _Predicate>(__pred)); }
  template <class _BinaryPredicate>
  void unique(_BinaryPredicate __bin_pred) { _M_impl.unique(__binary_pred_wrapper<_Tp, _BinaryPredicate>(__bin_pred)); }

  template <class _StrictWeakOrdering>
  void merge(_Self &__x, _StrictWeakOrdering __comp) {
    _M_impl.merge(__x._M_impl, __binary_pred_wrapper<_Tp, _StrictWeakOrdering>(__comp));
  }

  template <class _StrictWeakOrdering>
  void sort(_StrictWeakOrdering __comp) { _M_impl.sort(__binary_pred_wrapper<_Tp, _StrictWeakOrdering>(__comp)); }
#endif /* _STLP_MEMBER_TEMPLATES */

private:
  _Base _M_impl;
};

#endif /* _STLP_SPECIALIZED_LIST_H */

// Local Variables:
// mode:C++
// End:
