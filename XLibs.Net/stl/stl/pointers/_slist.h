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

#ifndef _STLP_SPECIALIZED_SLIST_H
#define _STLP_SPECIALIZED_SLIST_H

#ifndef _STLP_VOID_PTR_TRAITS_H
#  include <stl/pointers/_void_ptr_traits.h>
#endif

/*
 * The general slist class.
 */
template <class _Tp, _STLP_DEFAULT_ALLOCATOR_SELECT(_Tp) >
class slist {
private:
  typedef _STLP_PRIV::_Slist_impl<_Tp, _Alloc> _Base;
  typedef slist<_Tp, _Alloc> _Self;
public:
  _STLP_FORCE_ALLOCATORS(_Tp, _Alloc)
  
  __IMPORT_WITH_ITERATORS(_Base)
  typedef typename _Base::_Iterator_category _Iterator_category;
  
  allocator_type get_allocator() const { return _M_impl.get_allocator(); }

  explicit slist(const allocator_type& __a = allocator_type()) : _M_impl(__a) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit slist(size_type __n, const value_type& __x = value_type(),
#else
  slist(size_type __n, const value_type& __x,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
        const allocator_type& __a =  allocator_type()) : _M_impl(__n, __x, __a) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit slist(size_type __n) : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InputIterator>
  slist(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL) : _M_impl(__first, __last, __a) {}
# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  // VC++ needs this crazyness
  template <class _InputIterator>
  slist(_InputIterator __first, _InputIterator __last) : _M_impl(__first, __last) {}
# endif

#else /* _STLP_MEMBER_TEMPLATES */
  slist(const_iterator __first, const_iterator __last,
        const allocator_type& __a =  allocator_type() ) : _M_impl(__first, __last, __a) {}
  slist(const value_type* __first, const value_type* __last,
        const allocator_type& __a =  allocator_type()) : _M_impl(__first, __last, __a) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  slist(const _Self& __x) : _M_impl(__x._M_impl) {}

  slist(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  _Self& operator= (const _Self& __x) {
    _M_impl = __x._M_impl; return *this;
  }

  void assign(size_type __n, const value_type& __val) { _M_impl.assign(__n, __val); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
#else
  void assign(const value_type *__first, const value_type *__last) { _M_impl.assign(__first, __last); }
  void assign(const_iterator __first, const_iterator __last) {
#endif
    _M_impl.assign(__first, __last);
  }

  iterator before_begin()             { return _M_impl.before_begin(); }
  const_iterator before_begin() const { return _M_impl.before_begin(); }
  iterator begin()                    { return _M_impl.begin(); }
  const_iterator begin() const        { return _M_impl.begin(); }
  iterator end()                      { return _M_impl.end(); }
  const_iterator end() const          { return _M_impl.end(); }
  
  iterator previous(const_iterator __pos)             { return _M_impl.previous(__pos); }
  const_iterator previous(const_iterator __pos) const { return _M_impl.previous(__pos); }

  size_type size() const              { return _M_impl.size(); }
  size_type max_size() const          { return _M_impl.max_size(); }

  bool empty() const                  { return _M_impl.empty(); }

  void swap(_Self& __x) { _M_impl.swap(__x._M_impl); }

  reference front()                   { return _M_impl.front(); }
  const_reference front() const       { return _M_impl.front(); }
#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front(const value_type& __x = value_type())   {
#else
  void push_front(const value_type& __x)   {
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
    _M_impl.push_front(__x);
  }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front() { _M_impl.push_front();}
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void pop_front()  { _M_impl.pop_front(); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert_after(iterator __pos, const value_type& __x = value_type()) {
#else
  iterator insert_after(iterator __pos, const value_type& __x) {
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
    return _M_impl.insert_after(__pos, __x);
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert_after(iterator __pos) { return _M_impl.insert_after(__pos); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void insert_after(iterator __pos, size_type __n, const value_type& __x) {
    _M_impl.insert_after(__pos, __n, __x);
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InIter>
  void insert_after(iterator __pos, _InIter __first, _InIter __last) {
#else /* _STLP_MEMBER_TEMPLATES */
  void insert_after(iterator __pos,
                    const value_type* __first, const value_type* __last) {
    _M_impl.insert_after(__pos, __first, __last);
  }
  void insert_after(iterator __pos,
                    const_iterator __first, const_iterator __last) {
#endif /* _STLP_MEMBER_TEMPLATES */
   _M_impl.insert_after(__pos, __first, __last);
  }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos, const value_type& __x = value_type()) {
#else
  iterator insert(iterator __pos, const value_type& __x) {
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
    return _M_impl.insert(__pos, __x);
  }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos) { return _M_impl.insert(__pos); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void insert(iterator __pos, size_type __n, const value_type& __x) {
    _M_impl.insert(__pos, __n, __x);
  } 
    
#ifdef _STLP_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InIter>
  void insert(iterator __pos, _InIter __first, _InIter __last) {
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const value_type* __first, 
                              const value_type* __last) {
    _M_impl.insert(__pos, __first, __last);
  }
  void insert(iterator __pos, const_iterator __first, const_iterator __last) {
#endif /* _STLP_MEMBER_TEMPLATES */
    _M_impl.insert(__pos, __first, __last);
  }

  iterator erase_after(iterator __pos) 
  { return _M_impl.erase_after(__pos); }
  iterator erase_after(iterator __before_first, iterator __last) 
  { return _M_impl.erase_after(__before_first, __last); }

  iterator erase(iterator __pos) 
  { return _M_impl.erase(__pos); }
  iterator erase(iterator __first, iterator __last) 
  { return _M_impl.erase(__first, __last); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, const value_type& __x = value_type())
  { _M_impl.resize(__new_size, __x); }
#else
  void resize(size_type __new_size, const value_type& __x) 
  { _M_impl.resize(__new_size, __x); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void clear() { _M_impl.clear(); }

  void splice_after(iterator __pos, 
                    iterator __before_first, iterator __before_last)
  { _M_impl.splice_after(__pos, __before_first, __before_last); }
  void splice_after(iterator __pos, iterator __prev) 
  { _M_impl.splice_after(__pos, __prev); }
  void splice_after(iterator __pos, _Self& __x) 
  { _M_impl.splice_after(__pos, __x._M_impl); }
  void splice(iterator __pos, _Self& __x) 
  { _M_impl.splice(__pos, __x._M_impl); }
  void splice(iterator __pos, _Self& __x, iterator __i) 
  { _M_impl.splice(__pos, __x._M_impl, __i); }
  void splice(iterator __pos, _Self& __x, iterator __first, iterator __last) 
  { _M_impl.splice(__pos, __x._M_impl, __first, __last); }
  
  void reverse() { _M_impl.reverse(); }
  void remove(const value_type& __val) { _M_impl.remove(__val); }
  void unique() { _M_impl.unique(); }
  void merge(_Self& __x) { _M_impl.merge(__x._M_impl); }
  void sort() { _M_impl.sort(); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _Predicate>
  void remove_if(_Predicate __pred) { _M_impl.remove_if(__pred); }
  template <class _BinaryPredicate> 
  void unique(_BinaryPredicate __pred) { _M_impl.unique(__pred); }
  template <class _StrictWeakOrdering>
  void merge(_Self& __x, _StrictWeakOrdering __comp) { _M_impl.merge(__x._M_impl, __comp); }
  template <class _StrictWeakOrdering> 
  void sort(_StrictWeakOrdering __comp) { _M_impl.sort(__comp); }
#endif /* _STLP_MEMBER_TEMPLATES */
private:
  _Base _M_impl;
};

#if defined (_STLP_USE_TEMPLATE_EXPORT)
_STLP_EXPORT_TEMPLATE_CLASS _STLP_PRIV::_Slist_node<void*>;
typedef _STLP_PRIV::_Slist_node<void*> _VoidPtrSNode;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_alloc_proxy<_STLP_PRIV::_Slist_node_base, _VoidPtrSNode, allocator<_VoidPtrSNode> >;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_PRIV::_Slist_base<void*, allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_PRIV::_Slist_impl<void*, allocator<void*> >;
#endif

/*
 * The pointer partial specialization.
 */
template <class _Tp, class _Alloc>
class slist<_Tp*, _Alloc> {
private:
  //Member datas as a void* slist:
  typedef typename _Alloc_traits<void*, _Alloc>::allocator_type _VoidAlloc;
  typedef _STLP_PRIV::_Slist_impl<void*, _VoidAlloc> _Base;
  typedef slist<_Tp*, _Alloc> _Self;
  typedef __void_ptr_traits<_Tp> cast_traits;
  typedef _STLP_PRIV::_Slist_node_base _Node_base;

public:
  typedef _Tp*              value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;
  typedef forward_iterator_tag _Iterator_category;

  typedef _STLP_PRIV::_Slist_iterator<value_type, _Nonconst_traits<value_type> >  iterator;
  typedef _STLP_PRIV::_Slist_iterator<value_type, _Const_traits<value_type> >     const_iterator;

  _STLP_FORCE_ALLOCATORS(value_type, _Alloc)
  typedef typename _Alloc_traits<_Tp*, _Alloc>::allocator_type allocator_type;

public:
  allocator_type get_allocator() const 
  { return _STLP_CONVERT_ALLOCATOR(_M_impl.get_allocator(), value_type); }

  explicit slist(const allocator_type& __a = allocator_type())
    : _M_impl(_STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit slist(size_type __n, value_type __x = 0,
#else
  slist(size_type __n, value_type __x,
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
        const allocator_type& __a =  allocator_type())
    : _M_impl(__n, cast_traits::cast(__x), _STLP_CONVERT_ALLOCATOR(__a, void*)) {}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  explicit slist(size_type __n) : _M_impl(__n) {}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

#ifdef _STLP_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InputIte>
  slist(_InputIte __first, _InputIte __last,
        const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL)
    : _M_impl(__iterator_wrapper<_Tp, _InputIte>(__first), 
              __iterator_wrapper<_Tp, _InputIte>(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
# ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  // VC++ needs this crazyness
  template <class _InputIte>
  slist(_InputIte __first, _InputIte __last)
    : _M_impl(__iterator_wrapper<_Tp, _InputIte>(__first), 
              __iterator_wrapper<_Tp, _InputIte>(__last)) {}
# endif
#else /* _STLP_MEMBER_TEMPLATES */
  slist(const_iterator __first, const_iterator __last,
        const allocator_type& __a =  allocator_type() ) 
    : _M_impl(__first._M_node, __last._M_node, _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
  slist(const value_type* __first, const value_type* __last,
        const allocator_type& __a =  allocator_type()) 
    : _M_impl(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last), 
              _STLP_CONVERT_ALLOCATOR(__a, void*)) {}
#endif /* _STLP_MEMBER_TEMPLATES */

  slist(const _Self& __x) : _M_impl(__x._M_impl) {}
  slist(__move_source<_Self> src) : _M_impl(__move_source<_Base>(src.get()._M_impl)) {}

  _Self& operator= (const _Self& __x) { _M_impl = __x._M_impl; return *this; }

  void assign(size_type __n, value_type __val) { _M_impl.assign(__n, cast_traits::cast(__val)); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIte>
  void assign(_InputIte __first, _InputIte __last) {
    _M_impl.assign(__iterator_wrapper<_Tp, _InputIte>(__first), __iterator_wrapper<_Tp, _InputIte>(__last));
  }
#else
  void assign(const value_type *__first, const value_type *__last) {
    _M_impl.assign(cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last));
  }
  void assign(const_iterator __first, const_iterator __last) {
    _M_impl.assign(__first._M_node, __last._M_node);
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  iterator before_begin()             { return _M_impl.before_begin()._M_node; }
  const_iterator before_begin() const { return const_cast<_Node_base*>(_M_impl.before_begin()._M_node); }

  iterator begin()                    { return _M_impl.begin()._M_node; }
  const_iterator begin() const        { return const_cast<_Node_base*>(_M_impl.begin()._M_node);}

  iterator end()                      { return _M_impl.end()._M_node; }
  const_iterator end() const          { return _M_impl.end()._M_node; }

  size_type size() const      { return _M_impl.size(); }
  size_type max_size() const  { return _M_impl.max_size(); }
  bool empty() const          { return _M_impl.empty(); }

  void swap(_Self& __x) { _M_impl.swap(__x._M_impl); }

public:
  reference front()             { return *begin(); }
  const_reference front() const { return *begin(); }
#if !defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front(value_type __x = 0)
#else
  void push_front(value_type __x)
#endif /*!_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/
  { _M_impl.push_front(cast_traits::cast(__x)); }

# if defined(_STLP_DONT_SUP_DFLT_PARAM) && !defined(_STLP_NO_ANACHRONISMS)
  void push_front() { _M_impl.push_front();}
# endif /*_STLP_DONT_SUP_DFLT_PARAM && !_STLP_NO_ANACHRONISMS*/

  void pop_front() { _M_impl.pop_front(); }

  iterator previous(const_iterator __pos) { return _M_impl.previous(__pos._M_node)._M_node; }
  const_iterator previous(const_iterator __pos) const
  { return const_cast<_Node_base*>(_M_impl.previous(__pos._M_node)._M_node); }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert_after(iterator __pos, value_type __x = 0)
#else
  iterator insert_after(iterator __pos, value_type __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { return _M_impl.insert_after(__pos._M_node, cast_traits::cast(__x))._M_node; }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert_after(iterator __pos) { return _M_impl.insert_after(__pos._M_node)._M_node;}
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void insert_after(iterator __pos, size_type __n, value_type __x)
  { _M_impl.insert_after(__pos._M_node, __n, cast_traits::cast(__x)); }

#ifdef _STLP_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InIter>
  void insert_after(iterator __pos, _InIter __first, _InIter __last) {
    _M_impl.insert_after(__pos._M_node, __iterator_wrapper<_Tp, _InIter>(__first),
                                        __iterator_wrapper<_Tp, _InIter>(__last));
  }

#else /* _STLP_MEMBER_TEMPLATES */
  void insert_after(iterator __pos,
                    const_iterator __first, const_iterator __last)
  { _M_impl.insert_after(__pos._M_node, __first._M_node, __last._M_node); }
  void insert_after(iterator __pos,
                    const value_type* __first, const value_type* __last) {
    _M_impl.insert_after(__pos._M_node, cast_traits::const_ptr_cast(__first),
                                        cast_traits::const_ptr_cast(__last));
  }
#endif /* _STLP_MEMBER_TEMPLATES */

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos, value_type __x = 0)
#else
  iterator insert(iterator __pos, value_type __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { return _M_impl.insert(__pos._M_node, cast_traits::cast(__x))._M_node; }

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  iterator insert(iterator __pos)
  { return _M_impl.insert(__pos._M_node)._M_node; }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void insert(iterator __pos, size_type __n, value_type __x)
  { _M_impl.insert(__pos._M_node, __n, cast_traits::cast(__x)); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InIter>
  void insert(iterator __pos, _InIter __first, _InIter __last) {
    _M_impl.insert(__pos._M_node, __iterator_wrapper<_Tp, _InIter>(__first),
                                  __iterator_wrapper<_Tp, _InIter>(__last));
  }

#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __pos, const_iterator __first, const_iterator __last)
  { _M_impl.insert(__pos._M_node, __first._M_node, __last._M_node); }
  void insert(iterator __pos, const value_type* __first, const value_type* __last) 
  { _M_impl.insert(__pos._M_node, cast_traits::const_ptr_cast(__first), cast_traits::const_ptr_cast(__last)); }
#endif /* _STLP_MEMBER_TEMPLATES */

  iterator erase_after(iterator __pos)
  { return _M_impl.erase_after(__pos._M_node)._M_node; }
  iterator erase_after(iterator __before_first, iterator __last) 
  { return _M_impl.erase_after(__before_first._M_node, __last._M_node)._M_node; }

  iterator erase(iterator __pos) 
  { return _M_impl.erase(__pos._M_node)._M_node; }
  iterator erase(iterator __first, iterator __last) 
  { return _M_impl.erase(__first._M_node, __last._M_node)._M_node; }

#if !defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size, value_type __x = 0)
#else
  void resize(size_type __new_size, value_type __x)
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/
  { _M_impl.resize(__new_size, cast_traits::cast(__x));}

#if defined(_STLP_DONT_SUP_DFLT_PARAM)
  void resize(size_type __new_size) { _M_impl.resize(__new_size); }
#endif /*_STLP_DONT_SUP_DFLT_PARAM*/

  void clear() { _M_impl.clear(); }

  void splice_after(iterator __pos,
                    iterator __before_first, iterator __before_last) 
  { _M_impl.splice_after(__pos._M_node, __before_first._M_node, __before_last._M_node); }
  void splice_after(iterator __pos, iterator __prev) 
  { _M_impl.splice_after(__pos._M_node, __prev._M_node); }
  void splice_after(iterator __pos, _Self& __x) 
  { _M_impl.splice_after(__pos._M_node, __x._M_impl); }
  void splice(iterator __pos, _Self& __x) 
  { _M_impl.splice(__pos._M_node, __x._M_impl); }
  void splice(iterator __pos, _Self& __x, iterator __i) 
  { _M_impl.splice(__pos._M_node, __x._M_impl, __i._M_node); }
  void splice(iterator __pos, _Self& __x, iterator __first, iterator __last) 
  { _M_impl.splice(__pos._M_node, __x._M_impl, __first._M_node, __last._M_node); }

  void reverse() { _M_impl.reverse(); }

  void remove(value_type __val) { _M_impl.remove(cast_traits::cast(__val)); }
  void unique()                 { _M_impl.unique(); }
  void merge(_Self& __x)        { _M_impl.merge(__x._M_impl); }
  void sort()                   {_M_impl.sort(); }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _Predicate>
  void remove_if(_Predicate __pred)
  { _M_impl.remove_if(__unary_pred_wrapper<_Tp, _Predicate>(__pred)); }

  template <class _BinaryPredicate>
  void unique(_BinaryPredicate __pred) 
  { _M_impl.unique(__binary_pred_wrapper<_Tp, _BinaryPredicate>(__pred)); }

  template <class _StrictWeakOrdering>
  void merge(_Self& __x, _StrictWeakOrdering __comp) 
  { _M_impl.merge(__x._M_impl, __binary_pred_wrapper<_Tp, _StrictWeakOrdering>(__comp)); }

  template <class _StrictWeakOrdering>
  void sort(_StrictWeakOrdering __comp) 
  { _M_impl.sort(__binary_pred_wrapper<_Tp, _StrictWeakOrdering>(__comp)); }
#endif /* _STLP_MEMBER_TEMPLATES */

private:
  _Base _M_impl;
};

#endif /* _STLP_SPECIALIZED_SLIST_H */

// Local Variables:
// mode:C++
// End:
