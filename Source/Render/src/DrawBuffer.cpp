#include "StdAfxRD.h"
#include "DrawBuffer.h"

void DrawBuffer::Create(size_t vertices, size_t indices, vertex_fmt_t fmt, bool _dynamic, ePrimitiveType _primitive) {
    Destroy();
    this->dynamic = _dynamic;
    this->primitive = _primitive;
    gb_RenderDevice->CreateVertexBuffer(vb, vertices, fmt, dynamic);
    gb_RenderDevice->CreateIndexBuffer(ib, indices, dynamic);
    Backwind();
}

void DrawBuffer::Recreate() {
    if (!vb.buf) {
        gb_RenderDevice->CreateVertexBuffer(vb, vb.NumberVertex, vb.fmt, dynamic);
    }
    if (!ib.buf) {
        gb_RenderDevice->CreateIndexBuffer(ib, ib.NumberIndices, dynamic);
    }
    Backwind();
}

void DrawBuffer::Backwind() {
    if (IsLocked()) {
        xassert(0);
        Unlock();
    }
    locked_vertices = 0;
    locked_indices = 0;
    locked_vertices = 0;
    locked_indices = 0;
    written_vertices = 0;
    written_indices = 0;
}

void DrawBuffer::Destroy() {
    vb.Destroy();
    ib.Destroy();
}

void DrawBuffer::Unlock() {
    if (!IsLocked()) {
        xassert(0);
        return;
    }
    xassert(lock_written_vertices <= locked_vertices);
    xassert(lock_written_indices <= locked_indices);
    if (locked_vertices) gb_RenderDevice->UnlockVertexBuffer(vb);
    if (locked_indices) gb_RenderDevice->UnlockIndexBuffer(ib);
    written_vertices += lock_written_vertices;
    written_indices += lock_written_indices;
    lock_written_vertices = 0;
    lock_written_indices = 0;
    locked_vertices = 0;
    locked_indices = 0;
}

bool DrawBuffer::IsLocked() {
    return locked_vertices || locked_indices;
}

void DrawBuffer::AutoUnlock() {
    if (IsLocked()) Unlock();
}

void DrawBuffer::Draw() {
    if (IsLocked()) {
        xassert(0);
        Unlock();
    }
    
    if (primitive == PT_TRIANGLESTRIP) {
        if (written_vertices < 4) {
            Backwind();
            return;
        }        
    } else {
        if (!written_vertices) {
            return;
        }
    }
    gb_RenderDevice->SubmitDrawBuffer(this);
}

void DrawBuffer::EndTriangleStrip() {
    xassert(primitive == PT_TRIANGLESTRIP);
#if 1
    if (4 <= written_vertices && 4 <= written_indices) {
        indices_t* iptr = gb_RenderDevice->LockIndexBuffer(ib, written_indices, 2);
        iptr[0] = written_vertices - 1; iptr[1] = written_vertices - 1;
        gb_RenderDevice->UnlockIndexBuffer(ib);
    }
#endif
}

void DrawBuffer::PostDraw() {
    if (dynamic) {
        if (vb.buf && ib.buf) {
            Backwind();
        } else {
            Recreate();
        }
    }
}

bool DrawBuffer::LockSetup(size_t vertices, size_t indices) {
    //If nothing to lock then just skip
    if (!vertices && !indices) {
        return false;
    }
    
    //Check if there is enough available to lock
    if (vertices + written_vertices > vb.NumberVertex
    || indices + written_indices > ib.NumberIndices) {
        if (dynamic) {
            //Buffers are full, submit this and start from 0
            Draw();
        } else {
            //Uhh, this is not dynamic buffer, so we shouldn't get here
            xassert(0);
            return false;
        }
    }
    
    //If is more than buffer can provide then just not lock
    if (vertices > vb.NumberVertex || indices > ib.NumberIndices) {
        xassert(0);
        return false;
    }
    
    //Tell renderer this is current active DB
    if (dynamic) {
        gb_RenderDevice->SetActiveDrawBuffer(this);
    }
    
    //Locking can go
    return true;
}
