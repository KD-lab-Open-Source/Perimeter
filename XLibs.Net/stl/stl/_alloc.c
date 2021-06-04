/*
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
#ifndef _STLP_ALLOC_C
#define _STLP_ALLOC_C

#ifdef __WATCOMC__
#pragma warning 13 9
#pragma warning 367 9
#pragma warning 368 9
#endif

#ifndef _STLP_INTERNAL_ALLOC_H
#  include <stl/_alloc.h>
#endif

# if defined (_STLP_EXPOSE_GLOBALS_IMPLEMENTATION)

# ifdef _STLP_SGI_THREADS
  // We test whether threads are in use before locking.
  // Perhaps this should be moved into stl_threads.h, but that
  // probably makes it harder to avoid the procedure call when
  // it isn't needed.
    extern "C" {
      extern int __us_rsthread_malloc;
    }
# endif

// Specialised debug form of malloc which does not provide "false"
// memory leaks when run with debug CRT libraries.
#if defined(_STLP_MSVC) && (_STLP_MSVC>=1020 && defined(_STLP_DEBUG_ALLOC)) && !defined (_STLP_WINCE) && !defined (_STLP_WCE)
#  include <crtdbg.h>
inline void* __stlp_chunk_malloc(size_t __bytes) { _STLP_CHECK_NULL_ALLOC(_malloc_dbg(__bytes, _CRT_BLOCK, __FILE__, __LINE__)); }
inline void __stlp_chunck_free(void* __p) { _free_dbg(__p, _CRT_BLOCK); }
#else	// !_DEBUG
# ifdef _STLP_NODE_ALLOC_USE_MALLOC
#  include <cstdlib>
inline void* __stlp_chunk_malloc(size_t __bytes) { _STLP_CHECK_NULL_ALLOC(_STLP_VENDOR_CSTD::malloc(__bytes)); }
inline void __stlp_chunck_free(void* __p) { _STLP_VENDOR_CSTD::free(__p); }
# else
inline void* __stlp_chunk_malloc(size_t __bytes) { return _STLP_STD::__stl_new(__bytes); }
inline void __stlp_chunck_free(void* __p) { _STLP_STD::__stl_delete(__p); }
# endif
#endif	// !_DEBUG


#define _S_FREELIST_INDEX(__bytes) ((__bytes - size_t(1)) >> (int)_ALIGN_SHIFT)

_STLP_BEGIN_NAMESPACE

template <int __inst>
void * _STLP_CALL __malloc_alloc<__inst>::_S_oom_malloc(size_t __n) {
  __oom_handler_type __my_malloc_handler;
  void * __result;

  for (;;) {
    __my_malloc_handler = __oom_handler;
    if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
    (*__my_malloc_handler)();
    __result = malloc(__n);
    if (__result) return(__result);
  }
#if defined(_STLP_NEED_UNREACHABLE_RETURN)
  return 0;
#endif
}

template <class _Alloc>
void *  _STLP_CALL __debug_alloc<_Alloc>::allocate(size_t __n) {
  size_t __real_n = __n + __extra_before_chunk() + __extra_after_chunk();
  __alloc_header *__result = (__alloc_header *)__allocator_type::allocate(__real_n);
  memset((char*)__result, __shred_byte, __real_n * sizeof(value_type));
  __result->__magic = __magic;
  __result->__type_size = sizeof(value_type);
  __result->_M_size = (_STLP_UINT32_T)__n;
  return ((char*)__result) + (long)__extra_before;
}

template <class _Alloc>
void  _STLP_CALL
__debug_alloc<_Alloc>::deallocate(void *__p, size_t __n) {
  __alloc_header * __real_p = (__alloc_header*)((char *)__p -(long)__extra_before);
  // check integrity
  _STLP_VERBOSE_ASSERT(__real_p->__magic != __deleted_magic, _StlMsg_DBA_DELETED_TWICE)
  _STLP_VERBOSE_ASSERT(__real_p->__magic == __magic, _StlMsg_DBA_NEVER_ALLOCATED)
  _STLP_VERBOSE_ASSERT(__real_p->__type_size == 1,_StlMsg_DBA_TYPE_MISMATCH)
  _STLP_VERBOSE_ASSERT(__real_p->_M_size == __n, _StlMsg_DBA_SIZE_MISMATCH)
  // check pads on both sides
  unsigned char* __tmp;
  for (__tmp= (unsigned char*)(__real_p+1); __tmp < (unsigned char*)__p; __tmp++) {  
    _STLP_VERBOSE_ASSERT(*__tmp==__shred_byte, _StlMsg_DBA_UNDERRUN)
      }
  
  size_t __real_n= __n + __extra_before_chunk() + __extra_after_chunk();
  
  for (__tmp= ((unsigned char*)__p)+__n*sizeof(value_type); 
       __tmp < ((unsigned char*)__real_p)+__real_n ; __tmp++) {
    _STLP_VERBOSE_ASSERT(*__tmp==__shred_byte, _StlMsg_DBA_OVERRUN)
  }
  
  // that may be unfortunate, just in case
  __real_p->__magic=__deleted_magic;
  memset((char*)__p, __shred_byte, __n * sizeof(value_type));
  __allocator_type::deallocate(__real_p, __real_n);
}

// # ifdef _STLP_THREADS
#if !defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)

template <bool __threads, int __inst>
class _Node_Alloc_Lock {
public:
  _Node_Alloc_Lock() { 
    
#  ifdef _STLP_SGI_THREADS
    if (__threads && __us_rsthread_malloc)
#  else /* !_STLP_SGI_THREADS */
    if (__threads) 
#  endif
    	_S_lock._M_acquire_lock(); 
  }
  
  ~_Node_Alloc_Lock() {
#  ifdef _STLP_SGI_THREADS
    if (__threads && __us_rsthread_malloc)
#  else /* !_STLP_SGI_THREADS */
    if (__threads)
#  endif
      _S_lock._M_release_lock(); 
  }
  
  static _STLP_STATIC_MUTEX _S_lock;
};

#  ifdef _STLP_CLASS_PARTIAL_SPECIALIZATION
template <int __inst>
class _Node_Alloc_Lock<true,__inst> {
public:
  _Node_Alloc_Lock() { 
#    ifdef _STLP_SGI_THREADS
    if (__us_rsthread_malloc)
#    endif /* !_STLP_SGI_THREADS */
      _S_lock._M_acquire_lock(); 
  }
  
  ~_Node_Alloc_Lock() {
#    ifdef _STLP_SGI_THREADS
    if (__us_rsthread_malloc)
#    endif /* !_STLP_SGI_THREADS */
        _S_lock._M_release_lock(); 
  }
  
  static _STLP_STATIC_MUTEX _S_lock;
};

template <int __inst>
class _Node_Alloc_Lock<false,__inst> {
public:
  _Node_Alloc_Lock() { }  
  ~_Node_Alloc_Lock() { }
};

#  endif // _STLP_CLASS_PARTIAL_SPECIALIZATION

template <bool __threads, int __inst>
void* _STLP_CALL
__node_alloc<__threads, __inst>::_M_allocate(size_t __n) {
  _Obj * _STLP_VOLATILE * __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);
  _Obj *__r;

  // Acquire the lock here with a constructor call.
  // This ensures that it is released in exit or during stack
  // unwinding.
  _Node_Alloc_Lock<__threads, __inst> __lock_instance;

  if ( (__r  = *__my_free_list) != 0 ) {
    *__my_free_list = __r->_M_next;
  } else {
    __r = _S_refill(__n);
  }
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  _S_alloc_call();
#  endif
  // lock is released here
  return __r;
}

template <bool __threads, int __inst>
void _STLP_CALL
__node_alloc<__threads, __inst>::_M_deallocate(void *__p, size_t __n) {
  _Obj * _STLP_VOLATILE * __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);
  _Obj * __pobj = __STATIC_CAST(_Obj*, __p);

  // acquire lock
  _Node_Alloc_Lock<__threads, __inst> __lock_instance;
  __pobj->_M_next = *__my_free_list;
  *__my_free_list = __pobj;

#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  _S_dealloc_call();
#  endif
  // lock is released here
}

/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */
template <bool __threads, int __inst>
char* _STLP_CALL
__node_alloc<__threads, __inst>::_S_chunk_alloc(size_t _p_size, 
                                                int& __nobjs) {
  char* __result;
  size_t __total_bytes = _p_size * __nobjs;
  size_t __bytes_left = _S_end_free - _S_start_free;

  if (__bytes_left > 0) {
    if (__bytes_left >= __total_bytes) {
      __result = _S_start_free;
      _S_start_free += __total_bytes;
      return __result;
    }
    
    if (__bytes_left >= _p_size) {
      __nobjs = (int)(__bytes_left/_p_size);
      __total_bytes = _p_size * __nobjs;
      __result = _S_start_free;
      _S_start_free += __total_bytes;
      return __result;
    }

    // Try to make use of the left-over piece.
    _Obj* _STLP_VOLATILE* __my_free_list = _S_free_list + _S_FREELIST_INDEX(__bytes_left);
    ((_Obj*)_S_start_free) -> _M_next = *__my_free_list;
    *__my_free_list = (_Obj*)_S_start_free;
  }

  size_t __bytes_to_get = 
    2 * __total_bytes + _S_round_up(_S_heap_size >> 4)
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
    + sizeof(_Obj)
#  endif
    ;

  _S_start_free = (char*)__stlp_chunk_malloc(__bytes_to_get);
  if (0 == _S_start_free) {
    size_t __i;
    _Obj* _STLP_VOLATILE* __my_free_list;
    _Obj* __p;
    // Try to do with what we have.  That can't hurt.
    // We do not try smaller requests, since that tends
    // to result in disaster on multi-process machines.
    for (__i = _p_size; __i <= (size_t)_MAX_BYTES; __i += (size_t)_ALIGN) {
      __my_free_list = _S_free_list + _S_FREELIST_INDEX(__i);
      __p = *__my_free_list;
      if (0 != __p) {
        *__my_free_list = __p -> _M_next;
        _S_start_free = (char*)__p;
        _S_end_free = _S_start_free + __i;
        return _S_chunk_alloc(_p_size, __nobjs);
        // Any leftover piece will eventually make it to the
        // right free list.
      }
    }
    _S_end_free = 0;	// In case of exception.
    _S_start_free = (char*)__stlp_chunk_malloc(__bytes_to_get);
    /*
    (char*)malloc_alloc::allocate(__bytes_to_get);
    */

    // This should either throw an
    // exception or remedy the situation.  Thus we assume it
    // succeeded.
  }

  _S_heap_size += __bytes_to_get;
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  ((_Obj*)_S_start_free)->_M_next = _S_chunks;
  _S_chunks = ((_Obj*)_S_start_free);
#  endif
  _S_end_free = _S_start_free + __bytes_to_get;
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  _S_start_free += sizeof(_Obj);
#  endif
  return _S_chunk_alloc(_p_size, __nobjs);
}

/* Returns an object of size __n, and optionally adds to size __n free list.*/
/* We assume that __n is properly aligned.                                  */
/* We hold the allocation lock.                                             */
template <bool __threads, int __inst>
_Node_alloc_obj* _STLP_CALL
__node_alloc<__threads, __inst>::_S_refill(size_t __n) {
  int __nobjs = 20;
  __n = _S_round_up(__n);
  char* __chunk = _S_chunk_alloc(__n, __nobjs);

  if (1 == __nobjs) return (_Obj*)__chunk;

  _Obj* _STLP_VOLATILE* __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);
  _Obj* __result;
  _Obj* __current_obj;
  _Obj* __next_obj;

  /* Build free list in chunk */
  __result = (_Obj*)__chunk;
  *__my_free_list = __next_obj = (_Obj*)(__chunk + __n);
  for (--__nobjs; --__nobjs; ) {
    __current_obj = __next_obj;
    __next_obj = (_Obj*)((char*)__next_obj + __n);
    __current_obj->_M_next = __next_obj;
  }
  __next_obj->_M_next = 0;
  return __result;
}

#  if defined (_STLP_DO_CLEAN_NODE_ALLOC)

template <bool __threads, int __inst>
struct __node_alloc_cleaner {
  ~__node_alloc_cleaner() {
    __node_alloc<__threads, __inst>::_S_dealloc_call();
  }
};

template <bool __threads, int __inst>
size_t& _STLP_CALL
__node_alloc<__threads, __inst>::_S_alloc_call(size_t incr) {
  static __node_alloc_cleaner<__threads, __inst> _S_node_alloc_cleaner;
  static size_t _S_counter = 1;
  _S_counter += incr;
  return _S_counter;
}

template <bool __threads, int __inst>
void _STLP_CALL
__node_alloc<__threads, __inst>::_S_dealloc_call() {
  size_t &counter = _S_alloc_call(0);
  if (--counter == 0) {
    _S_chunk_dealloc();
  }
}
#  endif /* _STLP_DO_CLEAN_NODE_ALLOC */

#else /* _STLP_USE_LOCK_FREE_IMPLEMENTATION */

template <class _ListElem>
inline void __cas_new_head (_ListElem * _STLP_VOLATILE *__free_list, _ListElem *__new_head) {
  _ListElem *__old_head;
  do {
    __old_head = *__free_list;
    __new_head->_M_next = __old_head;
  } while (!_STLP_ATOMIC_CAS(__free_list, __new_head, __old_head));
}

template <bool __threads, int __inst>
void* _STLP_CALL
__node_alloc<__threads, __inst>::_M_allocate(size_t __n) {
  _Obj *__r;
  _Obj * _STLP_VOLATILE * __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);

  do {
    __r = *__my_free_list;
    if (__r  == 0) {
      __r = _S_refill(__n);
      break;
    }
  }
  while (!_STLP_ATOMIC_CAS(__my_free_list, __r->_M_next, __r));

#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  _S_alloc_call();
#  endif
  return __r;
}

template <bool __threads, int __inst>
void _STLP_CALL
__node_alloc<__threads, __inst>::_M_deallocate(void *__p, size_t __n) {
  _Obj * _STLP_VOLATILE * __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);
  _Obj *__pobj = __STATIC_CAST(_Obj*, __p);
  __cas_new_head(__my_free_list, __pobj);

#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  _S_dealloc_call();
#  endif
}

/* Returns an object of size __n, and optionally adds to size __n free list.*/
/* We assume that __n is properly aligned.                                  */
/* We hold the allocation lock.                                             */
template <bool __threads, int __inst>
_Node_alloc_obj* _STLP_CALL
__node_alloc<__threads, __inst>::_S_refill(size_t __n) {
  int __nobjs = 20;
  __n = _S_round_up(__n);
  char* __chunk = _S_chunk_alloc(__n, __nobjs);

  if (1 == __nobjs) return (_Obj*)__chunk;

  _Obj* _STLP_VOLATILE* __my_free_list = _S_free_list + _S_FREELIST_INDEX(__n);
  _Obj* __result;
  _Obj* __current_obj;
  _Obj* __new_head;
  _Obj* __next_obj;

  /* Build free list in chunk */
  __result = (_Obj*)__chunk;
  __new_head = __next_obj = (_Obj*)(__chunk + __n);
  for (--__nobjs; --__nobjs; ) {
    __current_obj = __next_obj;
    __next_obj = (_Obj*)((char*)__next_obj + __n);
    __current_obj->_M_next = __next_obj;
  }

  //Link the new free list to the global free list
  do {
    __next_obj->_M_next = *__my_free_list;
  } while (!_STLP_ATOMIC_CAS(__my_free_list, __new_head, __next_obj->_M_next));

  return __result;
}

/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */
template <bool __threads, int __inst>
char* _STLP_CALL
__node_alloc<__threads, __inst>::_S_chunk_alloc(size_t _p_size,
                                                int& __nobjs) {
  char* __result = 0;
  size_t __total_bytes = _p_size * __nobjs;

  //We get the first available free block:
  _Mem_block *__pblock;
  do {
    __pblock = _S_free_mem_blocks;
    if (__pblock == 0) break;
  }
  while (!_STLP_ATOMIC_CAS(&_S_free_mem_blocks, __pblock->_M_next, __pblock));

  if (__pblock != 0) {
    size_t __bytes_left = __pblock->_M_end - (char*)__pblock;
    char *__new_buf_pos = (char*)__pblock;

    if ((__bytes_left < __total_bytes) && (__bytes_left >= _p_size)) {
      //We won't get all the requested objs:
      __nobjs = (int)(__bytes_left/_p_size);
      __total_bytes = _p_size * __nobjs;
    }

    if (__bytes_left >= __total_bytes) {
      __result = (char*)__pblock;
      __bytes_left -= __total_bytes;
      __new_buf_pos += __total_bytes;
    }

    //We get rid of the leaving part of the mem block:
    if (__bytes_left != 0) {
      if ((__result != 0) && (__bytes_left > sizeof(_Mem_block))) {
        //We use part of the block and there is enough mem left to put the block back to 
        //the free mem blocks:
        _Mem_block *__pnew_block = (_Mem_block*)(__new_buf_pos);
        __pnew_block->_M_end = __pblock->_M_end;
        __cas_new_head(&_S_free_mem_blocks, __pnew_block);
      }
      else {
        //A too small block, we put it in the main free list elements:
        _Obj* _STLP_VOLATILE* __my_free_list = _S_free_list + _S_FREELIST_INDEX(__bytes_left);
        _Obj* __pobj = __REINTERPRET_CAST(_Obj*, __new_buf_pos);
        __cas_new_head(__my_free_list, __pobj);
      }
    }

    if (__result != 0) {
      return __result;
    }
  }

  //We haven't found a free block or not enough place, we ask memory to the the system:
  size_t __bytes_to_get = 
    2 * __total_bytes + _S_round_up(_S_heap_size >> 4)
#ifdef _STLP_DO_CLEAN_NODE_ALLOC
    + sizeof(_Obj)
#endif
    ;

  __result = (char*)__stlp_chunk_malloc(__bytes_to_get);
  if (0 == __result) {
    size_t __i;
    _Obj* _STLP_VOLATILE* __my_free_list;
    _Obj* __p;
    // Try to do with what we have.  That can't hurt.
    // We do not try smaller requests, since that tends
    // to result in disaster on multi-process machines.
    for (__i = _p_size; __i <= (size_t)_MAX_BYTES; __i += (size_t)_ALIGN) {
      __my_free_list = _S_free_list + _S_FREELIST_INDEX(__i);
      do {
        __p = *__my_free_list;
        if (__p == 0) break;
      } while (_STLP_ATOMIC_CAS(__my_free_list, __p->_M_next, __p));

      if (0 != __p) {
        if (__i < sizeof(_Mem_block)) {
          __nobjs = (int)(__i/_p_size);
          __total_bytes = __nobjs * _p_size;
          size_t __bytes_left = __i - __total_bytes;
          if (__bytes_left >= sizeof(_Obj)) {
            __my_free_list = _S_free_list + _S_FREELIST_INDEX(__bytes_left);
            __cas_new_head(__my_free_list, (_Obj*)((char*)__p + __total_bytes));
          }
          return (char*)__p;
        }
        else {
          _Mem_block *__pnewBlock = (_Mem_block*)(__p);
          __pnewBlock->_M_end = (char*)__p + __i;
          __cas_new_head(&_S_free_mem_blocks, __pnewBlock);
          return _S_chunk_alloc(_p_size, __nobjs);
        }
        // Any leftover piece will eventually make it to the
        // right free list.
      }
    }
    __result = (char*)__stlp_chunk_malloc(__bytes_to_get);

    // This should either throw an
    // exception or remedy the situation.  Thus we assume it
    // succeeded.
  }

  {
    size_t newHeapSize, oldHeapSize;
    do {
      oldHeapSize = _S_heap_size;
      newHeapSize = oldHeapSize + __bytes_to_get;
    } while (!_STLP_ATOMIC_CAS(&_S_heap_size, newHeapSize, oldHeapSize));
  }

#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
  __cas_new_head(&_S_chunks, (_Obj*)__result);
  __result += sizeof(_Obj);
  __bytes_to_get -= sizeof(_Obj);
#  endif
  _Mem_block *__pnewBlock = (_Mem_block*)(__result + __total_bytes);
  __pnewBlock->_M_end = __result + __bytes_to_get;
  __cas_new_head(&_S_free_mem_blocks, __pnewBlock);
  return __result;
}

#  if defined (_STLP_DO_CLEAN_NODE_ALLOC)
template <bool __threads, int __inst>
size_t& _STLP_CALL
__node_alloc<__threads, __inst>::_S_alloc_call(size_t incr) {
  static size_t _S_counter = 0;
  if (incr != 0) {
    _STLP_ATOMIC_INCREMENT(&_S_counter);
  }
  return _S_counter;
}

template <bool __threads, int __inst>
void _STLP_CALL
__node_alloc<__threads, __inst>::_S_dealloc_call() {
  size_t *pcounter = &_S_alloc_call(0);
  _STLP_ATOMIC_DECREMENT(pcounter);
  //As we are only releasing memory on shared library unload, counter
  //can only reach 0 once threads has been stopped so we do not have to
  //check atomic_decrement result.
  if (*pcounter == 0) {
    _S_chunk_dealloc();
  }
}
#  endif /* _STLP_DO_CLEAN_NODE_ALLOC */

#endif /* _STLP_USE_LOCK_FREE_IMPLEMENTATION */

#if defined (_STLP_DO_CLEAN_NODE_ALLOC)
/* We deallocate all the memory chunks      */
template <bool __threads, int __inst>
void _STLP_CALL
__node_alloc<__threads, __inst>::_S_chunk_dealloc() {
  _Obj *__pcur = _S_chunks, *__pnext;
  while (__pcur != 0) {
    __pnext = __pcur->_M_next;
    __stlp_chunck_free(__pcur);
    __pcur = __pnext;
  }
  _S_chunks = 0;
#if defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
  _S_free_mem_blocks = 0;
#else
  _S_start_free = _S_end_free = 0;
#endif
  _S_heap_size = 0;
  memset((char*)_S_free_list, 0, _STLP_NFREELISTS * sizeof(_Obj*));
}
#endif /* _STLP_DO_CLEAN_NODE_ALLOC */


#if (_STLP_STATIC_TEMPLATE_DATA > 0)
// malloc_alloc out-of-memory handling
template <int __inst>
__oom_handler_type __malloc_alloc<__inst>::__oom_handler = (__oom_handler_type)0;

#  if !defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
template <bool __threads, int __inst>
_STLP_STATIC_MUTEX
_Node_Alloc_Lock<__threads, __inst>::_S_lock _STLP_MUTEX_INITIALIZER;
#    ifdef _STLP_CLASS_PARTIAL_SPECIALIZATION
template <int __inst>
_STLP_STATIC_MUTEX
_Node_Alloc_Lock<true, __inst>::_S_lock _STLP_MUTEX_INITIALIZER;
#    endif
#  endif

template <bool __threads, int __inst>
_Node_alloc_obj * _STLP_VOLATILE
__node_alloc<__threads, __inst>::_S_free_list[_STLP_NFREELISTS]
= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// The 16 zeros are necessary to make version 4.1 of the SunPro
// compiler happy.  Otherwise it appears to allocate too little
// space for the array.

#  if defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
template <bool __threads, int __inst>
_Node_alloc_Mem_block* __node_alloc<__threads, __inst>::_S_free_mem_blocks = 0;
#  else
template <bool __threads, int __inst>
char *__node_alloc<__threads, __inst>::_S_start_free = 0;

template <bool __threads, int __inst>
char *__node_alloc<__threads, __inst>::_S_end_free = 0;
#  endif

template <bool __threads, int __inst>
size_t __node_alloc<__threads, __inst>::_S_heap_size = 0;

#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
template <bool __threads, int __inst>
_Node_alloc_obj *__node_alloc<__threads, __inst>::_S_chunks = 0;
#  endif

#else /* ( _STLP_STATIC_TEMPLATE_DATA > 0 ) */

__DECLARE_INSTANCE(__oom_handler_type, __malloc_alloc<0>::__oom_handler, =0);

#  define _STLP_ALLOC_NOTHREADS __node_alloc<false, 0>
#  define _STLP_ALLOC_THREADS   __node_alloc<true, 0>

#  if !defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
#    define _STLP_ALLOC_NOTHREADS_LOCK _Node_Alloc_Lock<false, 0>
#    define _STLP_ALLOC_THREADS_LOCK   _Node_Alloc_Lock<true, 0>
#  endif

#  if defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
__DECLARE_INSTANCE(_Node_alloc_Mem_block *, _STLP_ALLOC_NOTHREADS::_S_free_mem_blocks,=0);
#  else
__DECLARE_INSTANCE(char *, _STLP_ALLOC_NOTHREADS::_S_start_free,=0);
__DECLARE_INSTANCE(char *, _STLP_ALLOC_NOTHREADS::_S_end_free,=0);
#  endif
__DECLARE_INSTANCE(size_t, _STLP_ALLOC_NOTHREADS::_S_heap_size,=0);
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
__DECLARE_INSTANCE(_Node_alloc_obj *, _STLP_ALLOC_NOTHREADS::_S_chunks,=0);
#  endif
__DECLARE_INSTANCE(_Node_alloc_obj * _STLP_VOLATILE,
                   _STLP_ALLOC_NOTHREADS::_S_free_list[_STLP_NFREELISTS],
                   ={0});
#  if defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
__DECLARE_INSTANCE(_Node_alloc_Mem_block *, _STLP_ALLOC_THREADS::_S_free_mem_blocks,=0);
#  else
__DECLARE_INSTANCE(char *, _STLP_ALLOC_THREADS::_S_start_free,=0);
__DECLARE_INSTANCE(char *, _STLP_ALLOC_THREADS::_S_end_free,=0);
#  endif
__DECLARE_INSTANCE(size_t, _STLP_ALLOC_THREADS::_S_heap_size,=0);
#  ifdef _STLP_DO_CLEAN_NODE_ALLOC
__DECLARE_INSTANCE(_Node_alloc_obj *, _STLP_ALLOC_THREADS::_S_chunks,=0);
#  endif
__DECLARE_INSTANCE(_Node_alloc_obj * _STLP_VOLATILE,
                   _STLP_ALLOC_THREADS::_S_free_list[_STLP_NFREELISTS],
                   ={0});
#  if !defined (_STLP_USE_LOCK_FREE_IMPLEMENTATION)
__DECLARE_INSTANCE(_STLP_STATIC_MUTEX,
                   _STLP_ALLOC_NOTHREADS_LOCK::_S_lock,
                   _STLP_MUTEX_INITIALIZER);
__DECLARE_INSTANCE(_STLP_STATIC_MUTEX,
                   _STLP_ALLOC_THREADS_LOCK::_S_lock,
                   _STLP_MUTEX_INITIALIZER);

#    undef _STLP_ALLOC_THREADS_LOCK
#    undef _STLP_ALLOC_NOTHREADS_LOCK
#  endif

#  undef _STLP_ALLOC_THREADS
#  undef _STLP_ALLOC_NOTHREADS

#endif /* _STLP_STATIC_TEMPLATE_DATA */

_STLP_END_NAMESPACE

#undef _S_FREELIST_INDEX

#endif /* _STLP_EXPOSE_GLOBALS_IMPLEMENTATION */

#endif /*  _STLP_ALLOC_C */

// Local Variables:
// mode:C++
// End:
