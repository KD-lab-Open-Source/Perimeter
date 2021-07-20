#include <cstdio>
#include <windows.h>
#include <d3d9.h>
#include "d3dx9math.h"

//This file contains parts of WINE project source code for d3dx9math licensed under GPLv2

//Implementation copied from WINE project d3dx9_36/math.c
D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* pv) {
    FLOAT norm = pv->norm();
    if (!norm) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    } else {
        out->x = pv->x / norm;
        out->y = pv->y / norm;
        out->z = pv->z / norm;
    }

    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* out, const D3DXVECTOR3* pv, const D3DXMATRIX* pm) {
    D3DXVECTOR4 result;

    result.x = pm->m[0][0] * pv->x + pm->m[1][0] * pv->y + pm->m[2][0] * pv->z + pm->m[3][0];
    result.y = pm->m[0][1] * pv->x + pm->m[1][1] * pv->y + pm->m[2][1] * pv->z + pm->m[3][1];
    result.z = pm->m[0][2] * pv->x + pm->m[1][2] * pv->y + pm->m[2][2] * pv->z + pm->m[3][2];
    result.w = pm->m[0][3] * pv->x + pm->m[1][3] * pv->y + pm->m[2][3] * pv->z + pm->m[3][3];
    
    *out = result;
    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* out, const D3DXVECTOR3* pv, const D3DXMATRIX* pm) {
    const D3DXVECTOR3 v = *pv;

    out->x = pm->m[0][0] * v.x + pm->m[1][0] * v.y + pm->m[2][0] * v.z;
    out->y = pm->m[0][1] * v.x + pm->m[1][1] * v.y + pm->m[2][1] * v.z;
    out->z = pm->m[0][2] * v.x + pm->m[1][2] * v.y + pm->m[2][2] * v.z;

    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXVECTOR4* D3DXVec4Transform(D3DXVECTOR4 *out, const D3DXVECTOR4 *pv, const D3DXMATRIX *pm) {
    D3DXVECTOR4 result;

    result.x = pm->m[0][0] * pv->x + pm->m[1][0] * pv->y + pm->m[2][0] * pv->z + pm->m[3][0] * pv->w;
    result.y = pm->m[0][1] * pv->x + pm->m[1][1] * pv->y + pm->m[2][1] * pv->z + pm->m[3][1] * pv->w;
    result.z = pm->m[0][2] * pv->x + pm->m[1][2] * pv->y + pm->m[2][2] * pv->z + pm->m[3][2] * pv->w;
    result.w = pm->m[0][3] * pv->x + pm->m[1][3] * pv->y + pm->m[2][3] * pv->z + pm->m[3][3] * pv->w;

    *out = result;
    return out;
}

float D3DXVec4Length(D3DXVECTOR4* v) {
    return (float) sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}


void D3DXMatrixIdentity(D3DXMATRIX* mat) {
    mat->identity();
}

//Implementation copied from WINE project d3dx9_36/math.c
float D3DXMatrixDeterminant(const D3DXMATRIX *pm) {
    FLOAT t[3], v[4];

    t[0] = pm->m[2][2] * pm->m[3][3] - pm->m[2][3] * pm->m[3][2];
    t[1] = pm->m[1][2] * pm->m[3][3] - pm->m[1][3] * pm->m[3][2];
    t[2] = pm->m[1][2] * pm->m[2][3] - pm->m[1][3] * pm->m[2][2];
    v[0] = pm->m[1][1] * t[0] - pm->m[2][1] * t[1] + pm->m[3][1] * t[2];
    v[1] = -pm->m[1][0] * t[0] + pm->m[2][0] * t[1] - pm->m[3][0] * t[2];

    t[0] = pm->m[1][0] * pm->m[2][1] - pm->m[2][0] * pm->m[1][1];
    t[1] = pm->m[1][0] * pm->m[3][1] - pm->m[3][0] * pm->m[1][1];
    t[2] = pm->m[2][0] * pm->m[3][1] - pm->m[3][0] * pm->m[2][1];
    v[2] = pm->m[3][3] * t[0] - pm->m[2][3] * t[1] + pm->m[1][3] * t[2];
    v[3] = -pm->m[3][2] * t[0] + pm->m[2][2] * t[1] - pm->m[1][2] * t[2];

    return pm->m[0][0] * v[0] + pm->m[0][1] * v[1] +
           pm->m[0][2] * v[2] + pm->m[0][3] * v[3];
}

D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX *out, const D3DXMATRIX *m1, const D3DXMATRIX *m2) {
    *out = *m1 * *m2;
    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX *out, FLOAT *pdeterminant, const D3DXMATRIX *pm) {
    FLOAT det, t[3], v[16];
    UINT i, j;

    t[0] = pm->m[2][2] * pm->m[3][3] - pm->m[2][3] * pm->m[3][2];
    t[1] = pm->m[1][2] * pm->m[3][3] - pm->m[1][3] * pm->m[3][2];
    t[2] = pm->m[1][2] * pm->m[2][3] - pm->m[1][3] * pm->m[2][2];
    v[0] = pm->m[1][1] * t[0] - pm->m[2][1] * t[1] + pm->m[3][1] * t[2];
    v[4] = -pm->m[1][0] * t[0] + pm->m[2][0] * t[1] - pm->m[3][0] * t[2];

    t[0] = pm->m[1][0] * pm->m[2][1] - pm->m[2][0] * pm->m[1][1];
    t[1] = pm->m[1][0] * pm->m[3][1] - pm->m[3][0] * pm->m[1][1];
    t[2] = pm->m[2][0] * pm->m[3][1] - pm->m[3][0] * pm->m[2][1];
    v[8] = pm->m[3][3] * t[0] - pm->m[2][3] * t[1] + pm->m[1][3] * t[2];
    v[12] = -pm->m[3][2] * t[0] + pm->m[2][2] * t[1] - pm->m[1][2] * t[2];

    det = pm->m[0][0] * v[0] + pm->m[0][1] * v[4] +
          pm->m[0][2] * v[8] + pm->m[0][3] * v[12];

    if (det == 0.0f)
        return nullptr;
    if (pdeterminant)
        *pdeterminant = det;

    t[0] = pm->m[2][2] * pm->m[3][3] - pm->m[2][3] * pm->m[3][2];
    t[1] = pm->m[0][2] * pm->m[3][3] - pm->m[0][3] * pm->m[3][2];
    t[2] = pm->m[0][2] * pm->m[2][3] - pm->m[0][3] * pm->m[2][2];
    v[1] = -pm->m[0][1] * t[0] + pm->m[2][1] * t[1] - pm->m[3][1] * t[2];
    v[5] = pm->m[0][0] * t[0] - pm->m[2][0] * t[1] + pm->m[3][0] * t[2];

    t[0] = pm->m[0][0] * pm->m[2][1] - pm->m[2][0] * pm->m[0][1];
    t[1] = pm->m[3][0] * pm->m[0][1] - pm->m[0][0] * pm->m[3][1];
    t[2] = pm->m[2][0] * pm->m[3][1] - pm->m[3][0] * pm->m[2][1];
    v[9] = -pm->m[3][3] * t[0] - pm->m[2][3] * t[1]- pm->m[0][3] * t[2];
    v[13] = pm->m[3][2] * t[0] + pm->m[2][2] * t[1] + pm->m[0][2] * t[2];

    t[0] = pm->m[1][2] * pm->m[3][3] - pm->m[1][3] * pm->m[3][2];
    t[1] = pm->m[0][2] * pm->m[3][3] - pm->m[0][3] * pm->m[3][2];
    t[2] = pm->m[0][2] * pm->m[1][3] - pm->m[0][3] * pm->m[1][2];
    v[2] = pm->m[0][1] * t[0] - pm->m[1][1] * t[1] + pm->m[3][1] * t[2];
    v[6] = -pm->m[0][0] * t[0] + pm->m[1][0] * t[1] - pm->m[3][0] * t[2];

    t[0] = pm->m[0][0] * pm->m[1][1] - pm->m[1][0] * pm->m[0][1];
    t[1] = pm->m[3][0] * pm->m[0][1] - pm->m[0][0] * pm->m[3][1];
    t[2] = pm->m[1][0] * pm->m[3][1] - pm->m[3][0] * pm->m[1][1];
    v[10] = pm->m[3][3] * t[0] + pm->m[1][3] * t[1] + pm->m[0][3] * t[2];
    v[14] = -pm->m[3][2] * t[0] - pm->m[1][2] * t[1] - pm->m[0][2] * t[2];

    t[0] = pm->m[1][2] * pm->m[2][3] - pm->m[1][3] * pm->m[2][2];
    t[1] = pm->m[0][2] * pm->m[2][3] - pm->m[0][3] * pm->m[2][2];
    t[2] = pm->m[0][2] * pm->m[1][3] - pm->m[0][3] * pm->m[1][2];
    v[3] = -pm->m[0][1] * t[0] + pm->m[1][1] * t[1] - pm->m[2][1] * t[2];
    v[7] = pm->m[0][0] * t[0] - pm->m[1][0] * t[1] + pm->m[2][0] * t[2];

    v[11] = -pm->m[0][0] * (pm->m[1][1] * pm->m[2][3] - pm->m[1][3] * pm->m[2][1]) +
            pm->m[1][0] * (pm->m[0][1] * pm->m[2][3] - pm->m[0][3] * pm->m[2][1]) -
            pm->m[2][0] * (pm->m[0][1] * pm->m[1][3] - pm->m[0][3] * pm->m[1][1]);

    v[15] = pm->m[0][0] * (pm->m[1][1] * pm->m[2][2] - pm->m[1][2] * pm->m[2][1]) -
            pm->m[1][0] * (pm->m[0][1] * pm->m[2][2] - pm->m[0][2] * pm->m[2][1]) +
            pm->m[2][0] * (pm->m[0][1] * pm->m[1][2] - pm->m[0][2] * pm->m[1][1]);

    det = 1.0f / det;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            out->m[i][j] = v[4 * i + j] * det;

    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* out, const D3DXMATRIX* pm) {
    const D3DXMATRIX m = *pm;
    int i,j;

    for (i=0; i<4; i++)
        for (j=0; j<4; j++) out->m[i][j] = m.m[j][i];

    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXMATRIX * D3DXMatrixLookAtLH(D3DXMATRIX *out, const D3DXVECTOR3 *eye, const D3DXVECTOR3 *at, const D3DXVECTOR3 *up) {
    D3DXVECTOR3 right, upn, vec;

    vec = *at - *eye;
    D3DXVec3Normalize(&vec, &vec);
    right.cross(*up, vec);
    upn.cross(vec, right);
    D3DXVec3Normalize(&right, &right);
    D3DXVec3Normalize(&upn, &upn);
    out->m[0][0] = right.x;
    out->m[1][0] = right.y;
    out->m[2][0] = right.z;
    out->m[3][0] = -right.dot(*eye);
    out->m[0][1] = upn.x;
    out->m[1][1] = upn.y;
    out->m[2][1] = upn.z;
    out->m[3][1] = -upn.dot(*eye);
    out->m[0][2] = vec.x;
    out->m[1][2] = vec.y;
    out->m[2][2] = vec.z;
    out->m[3][2] = -vec.dot(*eye);
    out->m[0][3] = 0.0f;
    out->m[1][3] = 0.0f;
    out->m[2][3] = 0.0f;
    out->m[3][3] = 1.0f;

    return out;
}

//Implementation copied from WINE project d3dx9_36/math.c
D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pout, float fovy, float aspect, float zn, float zf) {
    D3DXMatrixIdentity(pout);
    pout->m[0][0] = 1.0f / (aspect * tanf(fovy/2.0f));
    pout->m[1][1] = 1.0f / tanf(fovy/2.0f);
    pout->m[2][2] = zf / (zf - zn);
    pout->m[2][3] = 1.0f;
    pout->m[3][2] = (zf * zn) / (zn - zf);
    pout->m[3][3] = 0.0f;
    return pout;
}
