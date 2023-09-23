#ifndef PERIMETER_DRAWBUFFER_H
#define PERIMETER_DRAWBUFFER_H

class DrawBuffer {
private:
    bool LockSetup(size_t vertices, size_t indices);
public:
    ePrimitiveType primitive;
    VertexBuffer vb;
    IndexBuffer ib;
    size_t locked_vertices = 0;
    size_t locked_indices = 0;
    size_t lock_written_vertices = 0;
    size_t lock_written_indices = 0;
    size_t written_vertices = 0;
    size_t written_indices = 0;
    bool set_as_active = false;
    
    DrawBuffer() = default;
    ~DrawBuffer() {
        Destroy();
    }
    NO_COPY_CONSTRUCTOR(DrawBuffer)

    void Create(size_t vertices, bool dynamic_vertices, size_t indices, bool dynamic_indices, vertex_fmt_t fmt, ePrimitiveType primitive);
    void Recreate();
    void Backwind();
    void Destroy();
    void LockRaw(size_t vertices, size_t indices, void*& vertex_buf, indices_t*& indices_buf, bool increment_written);
    void Unlock();
    bool IsLocked();
    void AutoUnlock();
    void Draw();
    void EndTriangleStrip();
    void PostDraw();
    
    //Helpers for common operation

    template<class TVERTEX>
    void Lock(size_t vertices, size_t indices, TVERTEX*& vertex_buf, indices_t*& indices_buf, bool increment_written) {
        //Make sure we are not locking with wrong vertex format
        xassert(TVERTEX::fmt == vb.fmt);
        static void* vbuf;
        LockRaw(vertices, indices, vbuf, indices_buf, increment_written);
        vertex_buf = static_cast<TVERTEX*>(vbuf);
    }

    template<class TVERTEX>
    inline void LockAll(TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        Lock(vb.NumberVertex, ib.NumberIndices, vertex_buf, indices_buf, true);
    }

    template<class TVERTEX>
    inline TVERTEX* LockQuad(size_t quads) {
        TVERTEX* vptr;
        indices_t* iptr;
        Lock(quads * 4, quads * 6, vptr, iptr, false);
        TVERTEX* empty = nullptr;
        WriteQuad(quads, empty, iptr);
        return vptr;
    }

    template<class TVERTEX>
    inline TVERTEX* LockTriangleStripSteps(size_t steps) {
        TVERTEX* vptr;
        indices_t* iptr;
        Lock((steps + 1) * 2, (steps + 1) * 2, vptr, iptr, false);
        TVERTEX* empty = nullptr;
        WriteTriangleStripStep(steps, empty, iptr);
        return vptr;
    }

    template<class TVERTEX>
    inline TVERTEX* LockTriangleStripGrid(size_t quads, size_t rows = 1) {
        TVERTEX* vptr;
        indices_t* iptr;
        Lock(quads * 4, quads * 4, vptr, iptr, false);
        TVERTEX* empty = nullptr;
        WriteTriangleStripGrid(quads, rows, empty, iptr);
        return vptr;
    }

    template<class TVERTEX>
    void AutoLockTriangleStripStep(size_t locked_steps, size_t steps, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(locked_steps);
        if ((locked_vertices && lock_written_vertices + steps * 2 >= locked_vertices)
            || (locked_indices && lock_written_indices + steps * 2 + 2 >= locked_indices)) {
            Unlock();
        }
        TVERTEX* empty = nullptr;
        bool was_locked = IsLocked();
        if (!was_locked) {
            Lock(
                    std::min(static_cast<size_t>(vb.NumberVertex), locked_steps * 2),
                    std::min(static_cast<size_t>(ib.NumberIndices), locked_steps * 2),
                    vertex_buf, indices_buf, false);
        }
        WriteTriangleStripStep(steps, was_locked ? vertex_buf : empty, indices_buf);
    }

    template<class TVERTEX>
    void AutoLockQuad(size_t locked_quads, size_t quads, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(locked_quads);
        if ((locked_vertices && lock_written_vertices + quads * 4 >= locked_vertices)
        || (locked_indices && lock_written_indices + quads * 6 >= locked_indices)) {
            Unlock();
        }
        TVERTEX* empty = nullptr;
        bool was_locked = IsLocked();
        if (!was_locked) {
            Lock(
                    std::min(static_cast<size_t>(vb.NumberVertex), locked_quads * 4),
                    std::min(static_cast<size_t>(ib.NumberIndices), locked_quads * 6),
                    vertex_buf, indices_buf, false);
        }
        WriteQuad(quads, was_locked ? vertex_buf : empty, indices_buf);
    }

    template<class TVERTEX>
    void AutoLockTriangle(size_t locked_triangles, size_t triangles, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(TVERTEX::fmt == vb.fmt);
        xassert(primitive == PT_TRIANGLES);
        xassert(locked_triangles);
        if (!triangles) return;
        if ((locked_vertices && lock_written_vertices + triangles * 3 >= locked_vertices)
        ||  (locked_indices && lock_written_indices + triangles * 3 >= locked_indices)) {
            Unlock();
        }
        bool was_locked = IsLocked();
        if (!was_locked) {
            Lock(
                    std::min(static_cast<size_t>(vb.NumberVertex), locked_triangles * 3),
                    std::min(static_cast<size_t>(ib.NumberIndices), locked_triangles * 3),
                    vertex_buf, indices_buf, false);
        }
        xassert(lock_written_vertices + triangles * 3 <= locked_vertices);
        xassert(lock_written_indices + triangles * 3 <= locked_indices);
        for (int i = 0; i < triangles; ++i) {
            size_t b = written_vertices + lock_written_vertices;
            indices_buf[0] = b;
            indices_buf[1] = b + 1;
            indices_buf[2] = b + 2;
            indices_buf += 3;
            lock_written_indices += 3;
            lock_written_vertices += 3;
        }
        if (was_locked) vertex_buf += triangles * 3;
    }
    
    //Write* functions prewrite indices and advance vertex buffer automatically

    template<class TVERTEX>
    void WriteTriangleStripStep(size_t steps, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(TVERTEX::fmt == vb.fmt);
        xassert(primitive == PT_TRIANGLESTRIP);
        if (!steps) return;
        xassert(lock_written_vertices + steps * 2 <= locked_vertices);
        xassert(lock_written_indices + steps * 2 <= locked_indices);
        size_t b = written_vertices + lock_written_vertices;
        size_t points = steps * 2;
        for (int i = 0; i < points; ++i) {
            indices_buf[i] = b + i;
        }
        indices_buf += points;
        lock_written_indices += points;
        lock_written_vertices += points;
        if (vertex_buf) vertex_buf += points;
    }

    template<class TVERTEX>
    void WriteQuad(size_t quads, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(TVERTEX::fmt == vb.fmt);
        xassert(primitive == PT_TRIANGLES);
        if (!quads) return;
        xassert(lock_written_vertices + quads * 4 <= locked_vertices);
        xassert(lock_written_indices + quads * 6 <= locked_indices);
        for (int i = 0; i < quads; ++i) {
            size_t b = written_vertices + lock_written_vertices;
            indices_buf[0] = b;
            indices_buf[1] = indices_buf[4] = b + 1;
            indices_buf[2] = indices_buf[3] = b + 2;
            indices_buf[5] = b + 3;
            indices_buf += 6;
            lock_written_indices += 6;
            lock_written_vertices += 4;
        }
        if (vertex_buf) vertex_buf += quads * 4;
    }

    template<class TVERTEX>
    void WriteTriangleStripGrid(size_t quads, size_t rows, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        xassert(TVERTEX::fmt == vb.fmt);
        xassert(primitive == PT_TRIANGLESTRIP);
        if (!quads) return;
        size_t line_points = quads + 1;
        size_t points = line_points * (rows + 1);
        if (vertex_buf) xassert(lock_written_vertices + points <= locked_vertices);
        int vi = 0;
        xassert(lock_written_indices + points <= locked_indices);
        for (int y = 0; y < rows; ++y) {
            size_t b = written_vertices + vi;
            //Start degenerate triangle
            indices_buf[0] = b;
            indices_buf += 1;
            for (int x = 0; x < line_points; ++x) {
                indices_buf[0] = b + x;
                indices_buf[1] = b + x + line_points;
                indices_buf += 2;
            }
            //End degenerate triangle
            indices_buf[0] = b + line_points * 2 - 1;
            indices_buf += 1;
            lock_written_indices += line_points * 2 + 2;
            vi += line_points;
        }
        vi += line_points;
        xassert(vi == points);
        if (vertex_buf) {
            vertex_buf += points;
            lock_written_vertices += points;
        }
    }
};

#endif //PERIMETER_DRAWBUFFER_H
