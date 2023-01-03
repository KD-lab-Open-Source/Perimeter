#include "StdAfxRD.h"
#include "DrawBuffer.h"

void DrawBuffer::Create(size_t vertices, size_t indices, vertex_fmt_t fmt, bool _dynamic, ePrimitiveType _primitive) {
    Destroy();
    this->dynamic = _dynamic;
    this->primitive = _primitive;
    locked_vertices = 0;
    locked_indices = 0;
    written_vertices = 0;
    written_indices = 0;
    gb_RenderDevice->CreateVertexBuffer(vb, vertices, fmt, dynamic);
    gb_RenderDevice->CreateIndexBuffer(ib, indices, dynamic);
}

void DrawBuffer::Recreate() {
    if (!vb.buf) {
        gb_RenderDevice->CreateVertexBuffer(vb, vb.NumberVertex, vb.fmt, dynamic);
    }
    if (!ib.buf) {
        gb_RenderDevice->CreateIndexBuffer(ib, ib.NumberIndices, dynamic);
    }
    written_vertices = 0;
    written_indices = 0;
}

void DrawBuffer::Destroy() {
    vb.Destroy();
    ib.Destroy();
}

void DrawBuffer::Unlock() {
    if (!locked_vertices && !locked_indices) {
        xassert(0);
        return;
    }
    if (locked_vertices) gb_RenderDevice->UnlockVertexBuffer(vb);
    if (locked_indices) gb_RenderDevice->UnlockIndexBuffer(ib);
    written_vertices += lock_written_vertices;
    written_indices += lock_written_indices;
    lock_written_vertices = 0;
    lock_written_indices = 0;
    locked_vertices = 0;
    locked_indices = 0;
}

void DrawBuffer::Draw() {
    if (locked_vertices || locked_indices) {
        xassert(0);
        Unlock();
    }
    if (!written_vertices) {
        return;
    }
    gb_RenderDevice->SubmitDrawBuffer(this);
}

void DrawBuffer::PostDraw() {
    if (dynamic) {
        if (vb.buf && ib.buf) {
            written_vertices = 0;
            written_indices = 0;
        } else {
            Recreate();
        }
    }
}

bool DrawBuffer::LockSetup(size_t vertices, size_t indices) {
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
