#include "xerrhand.h"
#include "Unknown.h"

cUnknownClass::~cUnknownClass() {
#ifdef PERIMETER_DEBUG_ASSERT
    int64_t r = GetRef();
    xassert(r == 0 || r == 1);
#endif

    //Remove link that handle has to this object if handle exists
    cUnknownHandle* handle = currentHandle;
#ifdef UNKNOWNCLASS_USE_ATOMIC
    if (handle) {
        if (currentHandle.compare_exchange_strong(handle, nullptr)) {
            handle->ptr = nullptr;
            handle->Release();
        } else {
            xassert(0);
        }
    }
#else
    currentHandle = nullptr;
    if (handle) {
        handle->ptr = nullptr;
        handle->Release();
    }
#endif
}

int64_t cUnknownClass:: Release() {
    int64_t r = DecRef();
    if (0 < r) {
        return r;
    }
    xassert(r == 0);
    delete this;
    return 0;
}

int64_t cUnknownClass::CountHandles() const {
    cUnknownHandle* handle = currentHandle;
    return handle ? handle->GetRef() : 0;
}

cUnknownHandle* cUnknownClass::AcquireHandle() {
    cUnknownHandle* handle = currentHandle;
    //Ceate handle for this obj
    if (handle == nullptr) {
        cUnknownHandle* newHandle = new cUnknownHandle(this);
#ifdef UNKNOWNCLASS_USE_ATOMIC
        if (currentHandle.compare_exchange_strong(handle, newHandle)) {
            handle = newHandle;
        } else {
            //Another thread created a handle, 'handle' contains the current handle
            newHandle->Release();
        }
#else
        currentHandle = handle = newHandle;
#endif
    }
    xassert(handle);
    //Auto increment ref count when giving handle
    if (handle) {
        handle->IncRef();
    }
    return handle;
}

void cUnknownClass::FreeHandle(cUnknownHandle* old_handle) {
    xassert(currentHandle == old_handle);
    cUnknownHandle* handle = old_handle;
#ifdef UNKNOWNCLASS_USE_ATOMIC
    if (currentHandle.compare_exchange_strong(handle, nullptr)) {
        if (handle) {
            handle->DecRef();
        }
    }
#else
    if (handle && currentHandle == handle) {
        currentHandle = nullptr;
        handle->DecRef();
    }
#endif
    xassert(handle == old_handle);
}

cUnknownHandle::cUnknownHandle(cUnknownClass* _ptr): ptr(_ptr) {
}

cUnknownHandle::~cUnknownHandle() {
    if (ptr) {
        ptr->FreeHandle(this);
    }
}
