#include <list>
#include <string>
#include "RenderTracker.h"
#include "xerrhand.h"
#include "RenderMT.h"

#ifdef PERIMETER_RENDER_TRACKER

MTSection log_lock;
std::list<RenderEvent*> events;

bool isRenderEventTypeVerbose(RenderEvent::RenderEventType type) {
    switch (type) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        case RenderEvent::CREATE_TEXTURE:
        case RenderEvent::DELETE_TEXTURE:
        case RenderEvent::CREATE_VERTEXBUF:
        case RenderEvent::DELETE_VERTEXBUF:
        case RenderEvent::CREATE_INDEXBUF:
        case RenderEvent::DELETE_INDEXBUF:
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        case RenderEvent::LOCK_TEXTURE:
        case RenderEvent::UNLOCK_TEXTURE:
        case RenderEvent::LOCK_VERTEXBUF:
        case RenderEvent::UNLOCK_VERTEXBUF:
        case RenderEvent::LOCK_INDEXBUF:
        case RenderEvent::UNLOCK_INDEXBUF:
#endif
            return true;
        default:
            return false;
    }
}

MTSection* GetRenderEventsLock() {
    return &log_lock;
}

const std::list<RenderEvent*>& GetRenderEvents() {
    MTAuto mtenter(&log_lock);
    return events;
}

#if 0
void PrintEvent(const RenderEvent* ev) {
    if (!isRenderEventTypeVerbose(ev->type)) {
        std::string padding = "- ";
        for (int i = 0; i < getRenderEventTypeDepth(ev->type); ++i) {
            padding += "    ";
        }
        std::string printlabel = ev->label;
        printf("%s(%u) [%p] [%s] %s (%s:%d)\n", padding.c_str(), ev->count, ev->ptr, getRenderEventTypeStr(ev->type), printlabel.c_str(), ev->call_file, ev->call_line);
    }
}
#else
#define PrintEvent(...)
#endif

void RenderSubmitEventImpl(const char* file, unsigned int line, RenderEvent::RenderEventType type, const char* label, void* ptr) {
    
    std::string labelstr = label ? label : "";
    if (events.size() > MAX_RENDER_EVENTS * 1.5f) {
        log_lock.Lock();
        while (events.size() > MAX_RENDER_EVENTS) {
            RenderEvent* ev = events.front();
            events.pop_front();
            delete ev;
        }
        log_lock.Unlock();  
    }
    
    //Avoid duplicate
    if (!events.empty()) {
        RenderEvent* ev = events.back();
        if (ev->type == type && ev->label == labelstr && ev->ptr == ptr) {
            ev->count += 1;
            PrintEvent(ev);
            return;
        }
    }
    
    RenderEvent* ev = new RenderEvent();
    ev->type = type;
    ev->call_file = file;
    ev->call_line = line;
    ev->label = labelstr;
    ev->count = 1;
    ev->ptr = ptr;

    {
        log_lock.Lock();
        events.emplace_back(ev);
        log_lock.Unlock();
    }

    PrintEvent(events.back());
}

RenderEvent::RenderEvent() = default;

RenderEvent::~RenderEvent() = default;

#endif //PERIMETER_RENDER_TRACKER
