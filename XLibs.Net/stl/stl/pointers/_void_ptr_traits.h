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

#ifndef _STLP_VOID_PTR_TRAITS_H
#define _STLP_VOID_PTR_TRAITS_H

/*
 * Struct traits to cast any qualified pointer type to void* and reverse.
 * This struct is only defined for pointer types.
 * NOTE: We do not use the STLport cast macro as a compiler that support
 * partial template specialization can't not support the C++ cast correctly
 */

/*
 * Base struct to deal with qualifiers
 */

template <class _Tp>
struct __void_ptr_traits_cv {
  typedef void* void_cv_type;

  inline static void * cast(void *__ptr) {
    return __ptr;
  }

  inline static void ** ptr_cast(void **__ptr) {
    return __ptr;
  }

  inline static void *const* const_ptr_cast(void *const*__ptr) {
    return __ptr;
  }

  inline static void *** ptr_ptr_cast(void ***__ptr) {
    return __ptr;
  }

  inline static void *& ref_cast(void *&__ref) {
    return __ref;
  }

  inline static void *const& const_ref_cast(void *const&__ref) {
    return __ref;
  }
};

template <class _VoidCVType>
struct __void_ptr_traits_cv_base {
  typedef _VoidCVType* void_cv_type;

  inline static void * cast(void_cv_type __ptr) {
    return const_cast<void*>(__ptr);
  }

  inline static void ** ptr_cast(void_cv_type *__ptr) {
    return const_cast<void **>(__ptr);
  }

  inline static void *const* const_ptr_cast(void_cv_type const*__ptr) {
    return const_cast<void *const*>(__ptr);
  }

  inline static void *** ptr_ptr_cast( void_cv_type **__ptr) {
    return const_cast<void***>(__ptr);
  }

  inline static void *& ref_cast(void_cv_type &__ref) {
    return const_cast<void *&>(__ref);
  }

  inline static void *const& const_ref_cast(void_cv_type const&__ref) {
    return const_cast<void *const&>(__ref);
  }

  /*
   * The reverse versions
   */
  inline static void_cv_type cast(void *__ptr) {
    return const_cast<void_cv_type>(__ptr);
  }

  inline static void_cv_type * ptr_cast(void **__ptr) {
    return const_cast<void_cv_type *>(__ptr);
  }

  inline static void_cv_type const* const_ptr_cast(void *const*__ptr) {
    return const_cast<void_cv_type const*>(__ptr);
  }

  inline static void_cv_type ** ptr_ptr_cast( void ***__ptr) {
    return const_cast<void_cv_type**>(__ptr);
  }

  inline static void_cv_type & ref_cast(void *&__ref) {
    return const_cast<void_cv_type &>(__ref);
  }

  inline static void_cv_type const& const_ref_cast(void *const&__ref) {
    return const_cast<void_cv_type const&>(__ref);
  }
};

template <class _Tp>
struct __void_ptr_traits_cv<_Tp const> : __void_ptr_traits_cv_base<void const>
{};

template <class _Tp>
struct __void_ptr_traits_cv<_Tp volatile> : __void_ptr_traits_cv_base<void volatile>
{};

template <class _Tp>
struct __void_ptr_traits_cv<_Tp const volatile> : __void_ptr_traits_cv_base<void const volatile>
{};

template <class _Tp>
struct __void_ptr_traits {
  typedef __void_ptr_traits_cv<_Tp> __cv;
  typedef typename __cv::void_cv_type void_cv_type;

  inline static _Tp * cast(void *__ptr) {
    return reinterpret_cast<_Tp *>(__cv::cast(__ptr));
  }

  inline static _Tp ** ptr_cast(void **__ptr) {
    return reinterpret_cast<_Tp **>(__cv::ptr_cast(__ptr));
  }

  inline static _Tp *const* const_ptr_cast(void *const*__ptr) {
    return reinterpret_cast<_Tp *const*>(__cv::const_ptr_cast(__ptr));
  }

  inline static _Tp *** ptr_ptr_cast(void ***__ptr) {
    return reinterpret_cast<_Tp ***>(__cv::ptr_ptr_cast(__ptr));
  }

  inline static _Tp *& ref_cast(void *&__ref) {
    return reinterpret_cast<_Tp *&>(__cv::ref_cast(__ref));
  }

  inline static _Tp *const& const_ref_cast(void *const&__ref) {
    return reinterpret_cast<_Tp *const&>(__cv::const_ref_cast(__ref));
  }

  /*
   * Reverse versions
   */
  inline static void * cast(_Tp *__ptr) {
    return __cv::cast(reinterpret_cast<void_cv_type>(__ptr));
  }

  inline static void ** ptr_cast(_Tp **__ptr) {
    return __cv::ptr_cast(reinterpret_cast<void_cv_type *>(__ptr));
  }

  inline static void *const* const_ptr_cast(_Tp *const*__ptr) {
    return __cv::const_ptr_cast(reinterpret_cast<void_cv_type const*>(__ptr));
  }

  inline static void *** ptr_ptr_cast(_Tp ***__ptr) {
    return __cv::ptr_ptr_cast(reinterpret_cast<void_cv_type **>(__ptr));
  }

  inline static void *& ref_cast(_Tp *&__ref) {
    return __cv::ref_cast(reinterpret_cast<void_cv_type &>(__ref));
  }

  inline static void *const& ref_cast(_Tp *const&__ref) {
    return __cv::const_ref_cast(reinterpret_cast<void_cv_type const&>(__ref));
  }
};

template <>
struct __void_ptr_traits<void> {
  inline static void* cast(void *__ptr) { return __ptr; }
  inline static void** ptr_cast(void **__ptr) { return __ptr; }
  inline static void*const* const_ptr_cast(void *const*__ptr) { return __ptr; }
  inline static void*** ptr_ptr_cast(void ***__ptr) { return __ptr; }
  inline static void*& ref_cast(void *&__ref) { return __ref; }
  inline static void*const& const_ref_cast(void *const&__ref) { return __ref; }
};

template <class _Tp, class _Iterator>
struct __iterator_wrapper {
  typedef __void_ptr_traits<_Tp> cast_traits;
  typedef iterator_traits<_Iterator> _IteTraits;
  
  typedef typename _IteTraits::iterator_category iterator_category;
  typedef void*  value_type;
  typedef typename _IteTraits::difference_type difference_type;
  typedef void** pointer;
  typedef void*  reference;

  typedef __iterator_wrapper<_Tp, _Iterator> _Self;

  __iterator_wrapper(_Iterator & __ite) : _M_ite(__ite) {}

  reference operator*() const { return cast_traits::cast(*_M_ite); }

  _Self& operator= (_Self const& __rhs) {
    _M_ite = __rhs._M_ite;
    return *this;
  }

  _Self& operator++() {
    ++_M_ite;
    return *this;
  }

  _Self& operator--() {
    --_M_ite;
    return *this;
  }

  _Self& operator += (difference_type __offset) {
    _M_ite += __offset;
    return *this;
  }
  difference_type operator -(_Self const& __other) const {
    return _M_ite - __other._M_ite;
  }

  bool operator == (_Self const& __other) const {
    return _M_ite == __other._M_ite;
  }

  bool operator != (_Self const& __other) const {
    return _M_ite != __other._M_ite;
  }

  bool operator < (_Self const& __rhs) const {
    return _M_ite < __rhs._M_ite;
  }

private:
  _Iterator &_M_ite;
};

template <class _Tp, class _UnaryPredicate>
struct __unary_pred_wrapper {
  typedef __void_ptr_traits<_Tp> cast_traits;
  
  __unary_pred_wrapper (_UnaryPredicate const& __pred) : _M_pred(__pred) {}
  
  bool operator () (void *__ptr) const {
    return _M_pred(cast_traits::cast(__ptr));
  }

private:
  _UnaryPredicate _M_pred;  
};


template <class _Tp, class _BinaryPredicate>
struct __binary_pred_wrapper {
  typedef __void_ptr_traits<_Tp> cast_traits;

  __binary_pred_wrapper () {}
  __binary_pred_wrapper (_BinaryPredicate const& __pred) : _M_pred(__pred) {}

  _BinaryPredicate get_pred() const { return _M_pred; }

  bool operator () (void *__first, void *__second) const {
    return _M_pred(cast_traits::cast(__first), cast_traits::cast(__second));
  }

private:
  _BinaryPredicate _M_pred;
};

#endif /* _STLP_VOID_PTR_TRAITS_H */
