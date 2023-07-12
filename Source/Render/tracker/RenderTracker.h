#ifndef PERIMETER_RENDERTRACKER_H 
#define PERIMETER_RENDERTRACKER_H

#ifdef PERIMETER_DEBUG
#define PERIMETER_RENDER_TRACKER
//#define PERIMETER_RENDER_TRACKER_PRINT
//#define PERIMETER_RENDER_TRACKER_COMMANDS
//#define PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
//#define PERIMETER_RENDER_TRACKER_RESOURCES
//#define PERIMETER_RENDER_TRACKER_LOCKS
#endif


#ifdef PERIMETER_RENDER_TRACKER

const int MAX_RENDER_EVENTS = 200; 

class RenderEvent {
public:
    enum RenderEventType {
        INIT,
        DONE,
        UPDATE_MODE,
        
        FILL,
        BEGIN_SCENE,
        END_SCENE,
        FLUSH_SCENE,
        
        //PTR: DrawBuffer
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
        GET_DRAW_BUFFER,
        SET_ACTIVE_DRAW_BUFFER,
        SUBMIT_DRAW_BUFFER,
        FINISH_ACTIVE_DRAW_BUFFER,
#endif
        
        //PTR: Texture
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        CREATE_TEXTURE,
        DELETE_TEXTURE,
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        LOCK_TEXTURE,
        LOCK_TEXTURE_RECT,
        UNLOCK_TEXTURE,
#endif
        
        //PTR: VertexBuffer
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        CREATE_VERTEXBUF,
        DELETE_VERTEXBUF,
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        LOCK_VERTEXBUF,
        UNLOCK_VERTEXBUF,
#endif
        
        //PTR: IndexBuffer
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        CREATE_INDEXBUF,
        DELETE_INDEXBUF,
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        LOCK_INDEXBUF,
        UNLOCK_INDEXBUF,
#endif
#ifdef PERIMETER_RENDER_TRACKER_MATRIX        
        SET_WORLD_MATRIX,
        SET_VIEWPROJ_MATRIX,
#endif
        
#ifdef PERIMETER_SOKOL
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
        //PTR: SokolCommand
        CREATE_COMMAND,
        PROCESS_COMMAND,
#endif
#endif
    } type = INIT;
    RenderEvent();
    ~RenderEvent();
    const char* call_file = "";
    unsigned int call_line = 0;
    std::string label = "";
    size_t count = 0;
    void* ptr = nullptr;
};

static const char* getRenderEventTypeStr(RenderEvent::RenderEventType type) {
    switch (type) {
        case RenderEvent::INIT: return "INIT";
        case RenderEvent::DONE: return "DONE";
        case RenderEvent::UPDATE_MODE: return "UPDATE_MODE";
        case RenderEvent::FILL: return "FILL";
        case RenderEvent::BEGIN_SCENE: return "BEGIN_SCENE";
        case RenderEvent::END_SCENE: return "END_SCENE";
        case RenderEvent::FLUSH_SCENE: return "FLUSH_SCENE";
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
        case RenderEvent::GET_DRAW_BUFFER: return "GET_DRAW_BUFFER";
        case RenderEvent::SET_ACTIVE_DRAW_BUFFER: return "SET_ACTIVE_DRAW_BUFFER";
        case RenderEvent::SUBMIT_DRAW_BUFFER: return "SUBMIT_DRAW_BUFFER";
#endif
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        case RenderEvent::CREATE_TEXTURE: return "CREATE_TEXTURE";
        case RenderEvent::DELETE_TEXTURE: return "DELETE_TEXTURE";
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        case RenderEvent::LOCK_TEXTURE: return "LOCK_TEXTURE";
        case RenderEvent::LOCK_TEXTURE_RECT: return "LOCK_TEXTURE_RECT";
        case RenderEvent::UNLOCK_TEXTURE: return "UNLOCK_TEXTURE";
#endif
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        case RenderEvent::CREATE_VERTEXBUF: return "CREATE_VERTEXBUF";
        case RenderEvent::DELETE_VERTEXBUF: return "DELETE_VERTEXBUF";
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        case RenderEvent::LOCK_VERTEXBUF: return "LOCK_VERTEXBUF";
        case RenderEvent::UNLOCK_VERTEXBUF: return "UNLOCK_VERTEXBUF";
#endif
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        case RenderEvent::CREATE_INDEXBUF: return "CREATE_INDEXBUF";
        case RenderEvent::DELETE_INDEXBUF: return "DELETE_INDEXBUF";
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
        case RenderEvent::LOCK_INDEXBUF: return "LOCK_INDEXBUF";
        case RenderEvent::UNLOCK_INDEXBUF: return "UNLOCK_INDEXBUF";
#endif
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
        case RenderEvent::SET_WORLD_MATRIX: return "SET_WORLD_MATRIX";
        case RenderEvent::SET_VIEWPROJ_MATRIX: return "SET_VIEWPROJ_MATRIX";
#endif
#ifdef PERIMETER_SOKOL
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
        case RenderEvent::CREATE_COMMAND: return "CREATE_COMMAND";
        case RenderEvent::PROCESS_COMMAND: return "PROCESS_COMMAND";
#endif
#endif
        default: return "?";
    }
}

static int getRenderEventTypeDepth(RenderEvent::RenderEventType type) {
    switch (type) {
        case RenderEvent::INIT:
        case RenderEvent::DONE:
        case RenderEvent::UPDATE_MODE:
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
        case RenderEvent::CREATE_TEXTURE:
        case RenderEvent::DELETE_TEXTURE:
        case RenderEvent::CREATE_VERTEXBUF:
        case RenderEvent::DELETE_VERTEXBUF:
        case RenderEvent::CREATE_INDEXBUF:
        case RenderEvent::DELETE_INDEXBUF:
#endif
        default:
            return 0;
        case RenderEvent::FILL:
        case RenderEvent::BEGIN_SCENE:
        case RenderEvent::END_SCENE:
        case RenderEvent::FLUSH_SCENE:
#ifdef PERIMETER_SOKOL
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
        case RenderEvent::CREATE_COMMAND:
        case RenderEvent::PROCESS_COMMAND:
#endif
#endif
            return 1;
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
        case RenderEvent::GET_DRAW_BUFFER:
        case RenderEvent::SET_ACTIVE_DRAW_BUFFER:
        case RenderEvent::SUBMIT_DRAW_BUFFER:
#endif
#ifdef PERIMETER_RENDER_TRACKER_LOCKS 
        case RenderEvent::LOCK_TEXTURE:
        case RenderEvent::LOCK_TEXTURE_RECT:
        case RenderEvent::UNLOCK_TEXTURE:
        case RenderEvent::LOCK_VERTEXBUF:
        case RenderEvent::UNLOCK_VERTEXBUF:
        case RenderEvent::LOCK_INDEXBUF:
        case RenderEvent::UNLOCK_INDEXBUF:
#endif
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
        case RenderEvent::SET_WORLD_MATRIX:
        case RenderEvent::SET_VIEWPROJ_MATRIX:
#endif
            return 2;
    }
}

bool isRenderEventTypeVerbose(RenderEvent::RenderEventType type);
//const std::list<RenderEvent*>& GetRenderEvents();
void RenderSubmitEventImpl(const char *file, unsigned int line, RenderEvent::RenderEventType type, const char* label = nullptr, void* ptr = nullptr);
#define RenderSubmitEvent(...) RenderSubmitEventImpl(__FILE__, __LINE__, __VA_ARGS__)

#else //PERIMETER_DEBUG

#define RenderSubmitEvent(...)

#endif //PERIMETER_DEBUG
#endif //PERIMETER_RENDERTRACKER_H