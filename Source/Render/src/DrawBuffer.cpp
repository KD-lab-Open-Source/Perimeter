#include "StdAfxRD.h"
#include "DrawBuffer.h"

void DrawBuffer::Create(size_t vertices, bool dynamic_vertices, size_t indices, bool dynamic_indices, vertex_fmt_t fmt, ePrimitiveType _primitive) {
    Destroy();
    this->primitive = _primitive;
    gb_RenderDevice->CreateVertexBuffer(vb, vertices, fmt, dynamic_vertices);
    gb_RenderDevice->CreateIndexBuffer(ib, indices, dynamic_indices);
    set_as_active = dynamic_vertices || dynamic_indices;
    Backwind();
}

void DrawBuffer::Recreate() {
    if (!vb.data) {
        gb_RenderDevice->CreateVertexBuffer(vb, vb.NumberVertex, vb.fmt, vb.dynamic);
    }
    if (!ib.data) {
        gb_RenderDevice->CreateIndexBuffer(ib, ib.NumberIndices, ib.dynamic);
    }
    Backwind();
}

void DrawBuffer::Backwind() {
    if (IsLocked()) {
        xassert(0);
        Unlock();
    }
    if (vb.dynamic) {
        locked_vertices = 0;
        lock_written_vertices = 0;
        written_vertices = 0;
    }
    if (ib.dynamic) {
        locked_indices = 0;
        lock_written_indices = 0;
        written_indices = 0;
    }
}

void DrawBuffer::Destroy() {
    vb.Destroy();
    ib.Destroy();
}

void DrawBuffer::LockRaw(size_t vertices, size_t indices, void*& vertex_buf, indices_t*& indices_buf, bool increment_written) {
    //Make sure we are not locking with wrong vertex format
    xassert(!locked_vertices && !locked_indices);
    if (!LockSetup(vertices, indices)) {
        vertex_buf = nullptr;
        indices_buf = nullptr;
        return;
    }
    //Set as null if no amount requested, lock whole if all was requested, else lock region
    if (vertices == 0) {
        vertex_buf = nullptr;
    } else if (written_vertices == 0 && vertices == vb.NumberVertex) {
        vertex_buf = gb_RenderDevice->LockVertexBuffer(vb);
    } else {
        vertex_buf = gb_RenderDevice->LockVertexBufferRange(vb, written_vertices, vertices);
    }
    if (indices == 0) {
        indices_buf = nullptr;
    } else if (written_indices == 0 && indices == ib.NumberIndices) {
        indices_buf = gb_RenderDevice->LockIndexBuffer(ib);
    } else {
        indices_buf = gb_RenderDevice->LockIndexBufferRange(ib, written_indices, indices);
    }
    if (vertex_buf) locked_vertices = vertices;
    if (indices_buf) locked_indices = indices;
    if (increment_written) {
        lock_written_vertices = locked_vertices;
        lock_written_indices = locked_indices;
    }
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
    
    gb_RenderDevice->SubmitDrawBuffer(this, nullptr);
}

void DrawBuffer::EndTriangleStrip() {
    xassert(primitive == PT_TRIANGLESTRIP);
#if 0
    if (4 <= written_vertices && 4 <= written_indices) {
        indices_t* iptr = gb_RenderDevice->LockIndexBuffer(ib, written_indices, 2);
        iptr[0] = written_vertices - 1; iptr[1] = written_vertices - 1;
        gb_RenderDevice->UnlockIndexBuffer(ib);
    }
#else
    Draw();
#endif
}

void DrawBuffer::PostDraw() {
    if (vb.dynamic || ib.dynamic) {
        if (vb.data && ib.data) {
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
    if (vertices + written_vertices > vb.NumberVertex) {
        if (vb.dynamic) {
            //Buffers are full, submit this and start from 0
            Draw();
        } else {
            //Uhh, this is not dynamic buffer, so we shouldn't get here
            xassert(0);
            return false;
        }
    }
    if (indices + written_indices > ib.NumberIndices) {
        if (ib.dynamic) {
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
    if (set_as_active) {
        gb_RenderDevice->SetActiveDrawBuffer(this);
    }
    
    //Locking can go
    return true;
}
