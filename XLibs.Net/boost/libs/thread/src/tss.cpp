// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#include <boost/thread/tss.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/exceptions.hpp>
#include <vector>
#include <stdexcept>
#include <cassert>

#if defined(BOOST_HAS_WINTHREADS)
#   include <windows.h>
#   include <boost/thread/detail/threadmon.hpp>
#endif

namespace {

typedef std::vector<void*> tss_slots;

struct tss_data_t
{
    boost::mutex mutex;
    std::vector<boost::function1<void, void*>*> cleanup_handlers;
#if defined(BOOST_HAS_WINTHREADS)
    DWORD native_key;
#elif defined(BOOST_HAS_PTHREADS)
    pthread_key_t native_key;
#elif defined(BOOST_HAS_MPTASKS)
    TaskStorageIndex native_key;
#endif
};

tss_data_t* tss_data = 0;
boost::once_flag tss_data_once = BOOST_ONCE_INIT;

extern "C" void cleanup_slots(void* p)
{
    tss_slots* slots = static_cast<tss_slots*>(p);
    for (tss_slots::size_type i = 0; i < slots->size(); ++i)
    {
        boost::mutex::scoped_lock lock(tss_data->mutex);
        (*tss_data->cleanup_handlers[i])((*slots)[i]);
        (*slots)[i] = 0;
    }
}

void init_tss_data()
{
    std::auto_ptr<tss_data_t> temp(new tss_data_t);

#if defined(BOOST_HAS_WINTHREADS)
    temp->native_key = TlsAlloc();
    if (temp->native_key == 0xFFFFFFFF)
        return;
#elif defined(BOOST_HAS_PTHREADS)
    int res = pthread_key_create(&temp->native_key, &cleanup_slots);
    if (res != 0)
        return;
#elif defined(BOOST_HAS_MPTASKS)
    OSStatus status = MPAllocateTaskStorageIndex(&temp->native_key);
    if (status != noErr)
        return;
#endif

    // Intentional memory "leak"
    // This is the only way to ensure the mutex in the global data
    // structure is available when cleanup handlers are run, since the
    // execution order of cleanup handlers is unspecified on any platform
    // with regards to C++ destructor ordering rules.
    tss_data = temp.release();
}

#if defined(BOOST_HAS_WINTHREADS)
tss_slots* get_slots(bool alloc);

void __cdecl tss_thread_exit()
{
    tss_slots* slots = get_slots(false);
    if (slots)
        cleanup_slots(slots);
}
#endif

tss_slots* get_slots(bool alloc)
{
    tss_slots* slots = 0;

#if defined(BOOST_HAS_WINTHREADS)
    slots = static_cast<tss_slots*>(
        TlsGetValue(tss_data->native_key));
#elif defined(BOOST_HAS_PTHREADS)
    slots = static_cast<tss_slots*>(
        pthread_getspecific(tss_data->native_key));
#elif defined(BOOST_HAS_MPTASKS)
    slots = static_cast<tss_slots*>(
        MPGetTaskStorageValue(tss_data->native_key));
#endif

    if (slots == 0 && alloc)
    {
        std::auto_ptr<tss_slots> temp(new tss_slots);

#if defined(BOOST_HAS_WINTHREADS)
        if (on_thread_exit(&tss_thread_exit) == -1)
            return 0;
        if (!TlsSetValue(tss_data->native_key, temp.get()))
            return 0;
#elif defined(BOOST_HAS_PTHREADS)
        if (pthread_setspecific(tss_data->native_key, temp.get()) != 0)
            return 0;
#elif defined(BOOST_HAS_MPTASKS)
        if (MPSetTaskStorageValue(tss_data->native_key, temp.get()) != noErr)
            return 0;
#endif

        slots = temp.release();
    }

    return slots;
}

} // namespace

namespace boost {

namespace detail {
void tss::init(boost::function1<void, void*>* pcleanup)
{
    boost::call_once(&init_tss_data, tss_data_once);
    if (tss_data == 0)
        throw thread_resource_error();
    boost::mutex::scoped_lock lock(tss_data->mutex);
    try
    {
        tss_data->cleanup_handlers.push_back(pcleanup);
        m_slot = tss_data->cleanup_handlers.size() - 1;
    }
    catch (...)
    {
        throw thread_resource_error();
    }
}

void* tss::get() const
{
    tss_slots* slots = get_slots(false);

    if (!slots)
        return 0;

    if (m_slot >= slots->size())
        return 0;

    return (*slots)[m_slot];
}

void tss::set(void* value)
{
    tss_slots* slots = get_slots(true);

    if (!slots)
        throw boost::thread_resource_error();

    if (m_slot >= slots->size())
    {
        try
        {
            slots->resize(m_slot + 1);
        }
        catch (...)
        {
            throw boost::thread_resource_error();
        }
    }

    (*slots)[m_slot] = value;
}

void tss::cleanup(void* value)
{
    boost::mutex::scoped_lock lock(tss_data->mutex);
    (*tss_data->cleanup_handlers[m_slot])(value);
}

} // namespace detail
} // namespace boost

// Change Log:
//   6 Jun 01  
//      WEKEMPF Initial version.
//  30 May 02  WEKEMPF 
//      Added interface to set specific cleanup handlers.
//      Removed TLS slot limits from most implementations.
//  22 Mar 04 GlassfordM for WEKEMPF
//      Fixed: thread_specific_ptr::reset() doesn't check error returned
//          by tss::set(); tss::set() now throws if it fails.
//      Fixed: calling thread_specific_ptr::reset() or 
//          thread_specific_ptr::release() causes double-delete: once on
//          reset()/release() and once on ~thread_specific_ptr().