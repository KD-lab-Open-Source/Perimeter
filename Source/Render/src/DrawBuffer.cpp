#include "StdAfxRD.h"
#include "DrawBuffer.h"

void DrawBuffer::Create(size_t vertices, size_t indices, vertex_fmt_t fmt, bool dynamic) {
    Destroy();
    locked_vertices = 0;
    locked_indices = 0;
    written_vertices = 0;
    written_indices = 0;
    gb_RenderDevice->CreateVertexBuffer(vb, vertices, fmt, dynamic);
    gb_RenderDevice->CreateIndexBuffer(ib, indices);
}

void DrawBuffer::Recreate() {
    if (!vb.buf) {
        gb_RenderDevice->CreateVertexBuffer(vb, vb.NumberVertex, vb.fmt, vb.dynamic);
    }
    if (!ib.buf) {
        gb_RenderDevice->CreateIndexBuffer(ib, ib.NumberIndices);
    }
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
    if (!locked_vertices) {
        xassert(0);
        return;
    }
    gb_RenderDevice->UnlockVertexBuffer(vb);
    gb_RenderDevice->UnlockIndexBuffer(ib);
    written_vertices += locked_vertices;
    written_indices += locked_indices;
    locked_vertices = 0;
    locked_indices = 0;
}

void DrawBuffer::Draw() {
    if (locked_vertices) {
        xassert(0);
        Unlock();
    }
    if (!written_vertices) {
        return;
    }
    gb_RenderDevice->SubmitDrawBuffer(this);
}
