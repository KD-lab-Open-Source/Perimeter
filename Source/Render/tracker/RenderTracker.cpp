#ifdef PERIMETER_DEBUG

#include <list>
#include <string>
#include "RenderTracker.h"

std::list<RenderEvent*> events;

bool isRenderEventTypeVerbose(RenderEvent::RenderEventType type) {
    switch (type) {
        case RenderEvent::CREATE_TEXTURE:
        case RenderEvent::DELETE_TEXTURE:

        /*
        case RenderEvent::CREATE_VERTEXBUF:
        case RenderEvent::DELETE_VERTEXBUF:
        case RenderEvent::CREATE_INDEXBUF:
        case RenderEvent::DELETE_INDEXBUF:
        /*/
            
        /*
        case RenderEvent::LOCK_TEXTURE:
        case RenderEvent::UNLOCK_TEXTURE:
        case RenderEvent::LOCK_VERTEXBUF:
        case RenderEvent::UNLOCK_VERTEXBUF:
        case RenderEvent::LOCK_INDEXBUF:
        case RenderEvent::UNLOCK_INDEXBUF:
        /*/
            return true;
        default:
            return false;
    }
}

const std::list<RenderEvent*>& GetRenderEvents() {
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
        printf("%s(%u) [%p] [%s] %s (%s, %d)\n", padding.c_str(), ev->count, ev->ptr, getRenderEventTypeStr(ev->type), printlabel.c_str(), ev->call_file, ev->call_line);
    }
}
#else
#define PrintEvent(...)
#endif

void RenderSubmitEventImpl(const char* file, unsigned int line, RenderEvent::RenderEventType type, const std::string& label, void* ptr) {
    while (events.size() > MAX_RENDER_EVENTS) {
        RenderEvent* ev = events.front();
        events.pop_front();
        delete ev;
    }
    
    //Avoid duplicate
    if (!events.empty()) {
        RenderEvent* ev = events.back();
        if (ev->type == type && ev->label == label && ev->ptr == ptr) {
            ev->count += 1;
            PrintEvent(ev);
            return;
        }
    }
    
    RenderEvent* ev = new RenderEvent {
            type, file, line, std::string(label), 1, ptr
    };
    
    events.emplace_back(ev);

    PrintEvent(events.back());
}

#endif //PERIMETER_DEBUG