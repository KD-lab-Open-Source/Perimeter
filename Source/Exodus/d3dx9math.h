#ifndef PERIMETER_D3DX9MATH_H
#define PERIMETER_D3DX9MATH_H

//Attempt to wrap legacy D3DX9 stuff with what we got

#include <d3d9types.h>
#include "xmath.h"
#include "../Render/inc/Umath.h"

//Type mapping

typedef Vect3f D3DXVECTOR3;
typedef Vect4f D3DXVECTOR4;
typedef CMatrix D3DXMATRIX;

D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* pv);
D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* out, const D3DXVECTOR3* pv, const D3DXMATRIX* pm);
D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* out, const D3DXVECTOR3* pv, const D3DXMATRIX* pm);
D3DXVECTOR4* D3DXVec4Transform(D3DXVECTOR4 *pout, const D3DXVECTOR4 *pv, const D3DXMATRIX *pm);
float D3DXVec4Length(D3DXVECTOR4* v);

void D3DXMatrixIdentity(D3DXMATRIX* mat);
float D3DXMatrixDeterminant(const D3DXMATRIX *m);
D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* m1, const D3DXMATRIX* m2);
D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* out, float* determinant, const D3DXMATRIX* mp);
D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* out, const D3DXMATRIX *pm);
D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX *out, const D3DXVECTOR3 *eye, const D3DXVECTOR3 *at, const D3DXVECTOR3 *up);
D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pout, float fovy, float aspect, float zn, float zf);

#endif //PERIMETER_D3DX9MATH_H
