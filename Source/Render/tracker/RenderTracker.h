#ifndef PERIMETER_RENDERTRACKER_H 
#define PERIMETER_RENDERTRACKER_H
#ifdef PERIMETER_DEBUG

const int MAX_RENDER_EVENTS = 50; 

struct RenderEvent {
    enum RenderEventType {
        INIT,
        DONE,
        UPDATE_MODE,
        
        FILL,
        BEGIN_SCENE,
        END_SCENE,
        FLUSH_SCENE,
        
        //PTR: DrawBuffer
        SET_ACTIVE_DRAW_BUFFER,
        SUBMIT_DRAW_BUFFER,
        
        //PTR: Texture
        CREATE_TEXTURE,
        DELETE_TEXTURE,
        LOCK_TEXTURE,
        UNLOCK_TEXTURE,
        
        //PTR: VertexBuffer
        CREATE_VERTEXBUF,
        DELETE_VERTEXBUF,
        LOCK_VERTEXBUF,
        UNLOCK_VERTEXBUF,
        
        //PTR: IndexBuffer
        CREATE_INDEXBUF,
        DELETE_INDEXBUF,
        LOCK_INDEXBUF,
        UNLOCK_INDEXBUF,
        
        SET_WORLD_MATRIX,
        SET_VIEWPROJ_MATRIX,
        
#ifdef PERIMETER_SOKOL
        //PTR: SokolCommand
        FINISH_COMMAND,
        PROCESS_COMMAND,
#endif
    } type;
    const char* call_file;
    unsigned int call_line;
    std::string label;
    size_t count;
    void* ptr;
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
        case RenderEvent::SET_ACTIVE_DRAW_BUFFER: return "SET_ACTIVE_DRAW_BUFFER";
        case RenderEvent::SUBMIT_DRAW_BUFFER: return "SUBMIT_DRAW_BUFFER";
        case RenderEvent::CREATE_TEXTURE: return "CREATE_TEXTURE";
        case RenderEvent::DELETE_TEXTURE: return "DELETE_TEXTURE";
        case RenderEvent::LOCK_TEXTURE: return "LOCK_TEXTURE";
        case RenderEvent::UNLOCK_TEXTURE: return "UNLOCK_TEXTURE";
        case RenderEvent::CREATE_VERTEXBUF: return "CREATE_VERTEXBUF";
        case RenderEvent::DELETE_VERTEXBUF: return "DELETE_VERTEXBUF";
        case RenderEvent::LOCK_VERTEXBUF: return "LOCK_VERTEXBUF";
        case RenderEvent::UNLOCK_VERTEXBUF: return "UNLOCK_VERTEXBUF";
        case RenderEvent::CREATE_INDEXBUF: return "CREATE_INDEXBUF";
        case RenderEvent::DELETE_INDEXBUF: return "DELETE_INDEXBUF";
        case RenderEvent::LOCK_INDEXBUF: return "LOCK_INDEXBUF";
        case RenderEvent::UNLOCK_INDEXBUF: return "UNLOCK_INDEXBUF";
        case RenderEvent::SET_WORLD_MATRIX: return "SET_WORLD_MATRIX";
        case RenderEvent::SET_VIEWPROJ_MATRIX: return "SET_VIEWPROJ_MATRIX";
#ifdef PERIMETER_SOKOL
        case RenderEvent::FINISH_COMMAND: return "FINISH_COMMAND";
        case RenderEvent::PROCESS_COMMAND: return "PROCESS_COMMAND";
#endif
        default: return "?";
    }
}

static int getRenderEventTypeDepth(RenderEvent::RenderEventType type) {
    switch (type) {
        case RenderEvent::INIT:
        case RenderEvent::DONE:
        case RenderEvent::UPDATE_MODE:
        case RenderEvent::FILL:
        case RenderEvent::BEGIN_SCENE:
        case RenderEvent::END_SCENE:
        case RenderEvent::FLUSH_SCENE:
        case RenderEvent::CREATE_TEXTURE:
        case RenderEvent::DELETE_TEXTURE:
        case RenderEvent::CREATE_VERTEXBUF:
        case RenderEvent::DELETE_VERTEXBUF:
        case RenderEvent::CREATE_INDEXBUF:
        case RenderEvent::DELETE_INDEXBUF:
        default:
            return 0;
#ifdef PERIMETER_SOKOL
        case RenderEvent::FINISH_COMMAND:
        case RenderEvent::PROCESS_COMMAND:
#endif
            return 1;
        case RenderEvent::SET_ACTIVE_DRAW_BUFFER:
        case RenderEvent::SUBMIT_DRAW_BUFFER:
        case RenderEvent::LOCK_TEXTURE:
        case RenderEvent::UNLOCK_TEXTURE:
        case RenderEvent::LOCK_VERTEXBUF:
        case RenderEvent::UNLOCK_VERTEXBUF:
        case RenderEvent::LOCK_INDEXBUF:
        case RenderEvent::UNLOCK_INDEXBUF:
        case RenderEvent::SET_WORLD_MATRIX:
        case RenderEvent::SET_VIEWPROJ_MATRIX:
            return 2;
    }
}

bool isRenderEventTypeVerbose(RenderEvent::RenderEventType type);
const std::list<RenderEvent*>& GetRenderEvents();
void RenderSubmitEventImpl(const char *file, unsigned int line, RenderEvent::RenderEventType type, const std::string& label = "", void* ptr = nullptr);
#define RenderSubmitEvent(args...) RenderSubmitEventImpl(__FILE__, __LINE__, args)

#else //PERIMETER_DEBUG

#define RenderSubmitEvent(args...)

#endif //PERIMETER_DEBUG
#endif //PERIMETER_RENDERTRACKER_H