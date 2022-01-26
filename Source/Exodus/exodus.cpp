#include "exodus.h"
#include "tweaks.h"
#include <cctype>
#include <thread>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Sleep(uint32_t millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

//According to MSDN: Both __iscsym and __iswcsym return a nonzero value if c is a letter, underscore, or digit. 
int __iscsym(int c) {
    if (c == '_') return 1;
    return isalnum(c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HANDLE CreateEvent(int, bool manualReset, bool initialState, int) {
    return neosmart::CreateEvent(manualReset, initialState);
}

void DestroyEvent(HANDLE event) {
    neosmart::DestroyEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

void SetEvent(HANDLE event) {
    neosmart::SetEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

void ResetEvent(HANDLE event) {
    neosmart::ResetEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

uint32_t WaitForSingleObject(HANDLE event, uint64_t milliseconds) {
    return neosmart::WaitForEvent(reinterpret_cast<neosmart::neosmart_event_t>(event), milliseconds);
}

uint32_t WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds) {
    return neosmart::WaitForMultipleEvents(reinterpret_cast<neosmart::neosmart_event_t*>(events), count, waitAll, milliseconds);
}
