#ifndef PERIMETER_DRAWBUFFER_H
#define PERIMETER_DRAWBUFFER_H

class DrawBuffer {
private:
    bool LockSetup(size_t vertices, size_t indices);
public:
    VertexBuffer vb;
    IndexBuffer ib;
    bool dynamic = false;
    size_t locked_vertices = 0;
    size_t locked_indices = 0;
    size_t written_vertices = 0;
    size_t written_indices = 0;
    
    DrawBuffer() = default;
    ~DrawBuffer() {
        Destroy();
    }
    NO_COPY_CONSTRUCTOR(DrawBuffer)

    void Create(size_t vertices, size_t indices, vertex_fmt_t fmt, bool dynamic);
    void Recreate();
    void Destroy();
    void Unlock();
    void Draw();
    void PostDraw();

    template<class TVERTEX>
    void Lock(size_t vertices, size_t indices, TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        //Make sure we are not locking with wrong vertex format
        xassert(TVERTEX::fmt == vb.fmt);
        xassert(!locked_vertices && !locked_indices);
        if (!LockSetup(vertices, indices)) {
            vertex_buf = nullptr;
            indices_buf = nullptr;
            return;
        }
        vertex_buf = static_cast<TVERTEX*>(gb_RenderDevice->LockVertexBuffer(vb, written_vertices, vertices));
        if (vertex_buf) locked_vertices = vertices;
        indices_buf = gb_RenderDevice->LockIndexBuffer(ib, written_indices, indices);
        if (indices_buf) locked_indices = indices;
    }

    template<class TVERTEX>
    TVERTEX* LockQuad(size_t quads) {
        TVERTEX* vptr;
        indices_t* iptr;
        Lock(quads * 4, quads * 6, vptr, iptr);
        for (int i = 0; i < quads; ++i) {
            size_t b = written_vertices + i * 4;
            iptr[0] = b;
            iptr[1] = iptr[4] = b + 1;
            iptr[2] = iptr[3] = b + 2;
            iptr[5] = b + 3;
            iptr += 6;
        }
        return vptr;
    }

    template<class TVERTEX>
    void LockAll(TVERTEX*& vertex_buf, indices_t*& indices_buf) {
        Lock(vb.NumberVertex, ib.NumberIndices, vertex_buf, indices_buf);
    }
};

#endif //PERIMETER_DRAWBUFFER_H
