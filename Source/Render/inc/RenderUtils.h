#ifndef PERIMETER_RENDERUTILS_H
#define PERIMETER_RENDERUTILS_H

void copy_indices_list(indices_t* dst, const indices_t* src, size_t len, size_t written_vertices) {
    if (written_vertices == 0) {
        //No indices shifting required, copy as is
        memcpy(dst, src, len * sizeof(indices_t));
    } else {
        size_t i = 0;
        //8 unrolled
        while (i + 7 < len) {
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
        }
        //3 unrolled
        while (i + 2 < len) {
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
        }
        //Leftovers
        while (i < len) {
            dst[i] = src[i] + written_vertices; i++;
        }
    }
}

#endif //PERIMETER_RENDERUTILS_H
