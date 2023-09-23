#include <string>
#include "StdAfxRD.h"
#include "Umath.h"

float HermitSpline(float t,float p0,float p1,float p2,float p3)
{
//	return (3.0f/2.0f*p1-3.0f/2.0f*p2-p0/2+p3/2)*t*t*t +
//			(2.0f*p2+p0-p3/2-5.0f/2.0f*p1)*t*t + (p2/2-p0/2)*t + p1;

	return (1.5f*(p1-p2)+(-p0+p3)*0.5f)*t*t*t +
		   (2.0f*p2+p0-p3*0.5f-2.5f*p1)*t*t + (p2-p0)*0.5f*t + p1;
}

float HermitSplineDerivation(float t,float p0,float p1,float p2,float p3)
{
	return (1.5f*(p1-p2)+(-p0+p3)*0.5f)*t*t*3 +
		   (2.0f*p2+p0-p3*0.5f-2.5f*p1)*t*2 + (p2-p0)*0.5f;
}

float HermitSplineDerivation2(float t,float p0,float p1,float p2,float p3)
{
	return (1.5f*(p1-p2)+(-p0+p3)*0.5f)*t*2*3 +
		   (2.0f*p2+p0-p3*0.5f-2.5f*p1)*2;
}

Vect3f HermitSpline(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3)
{
	Vect3f out;
	out.x=HermitSpline(t,p0.x,p1.x,p2.x,p3.x);
	out.y=HermitSpline(t,p0.y,p1.y,p2.y,p3.y);
	out.z=HermitSpline(t,p0.z,p1.z,p2.z,p3.z);
	return out;
}

Vect3f HermitSplineDerivation(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3)
{
	Vect3f out;
	out.x=HermitSplineDerivation(t,p0.x,p1.x,p2.x,p3.x);
	out.y=HermitSplineDerivation(t,p0.y,p1.y,p2.y,p3.y);
	out.z=HermitSplineDerivation(t,p0.z,p1.z,p2.z,p3.z);
	return out;
}

Vect3f HermitSplineDerivation2(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3)
{
	Vect3f out;
	out.x=HermitSplineDerivation2(t,p0.x,p1.x,p2.x,p3.x);
	out.y=HermitSplineDerivation2(t,p0.y,p1.y,p2.y,p3.y);
	out.z=HermitSplineDerivation2(t,p0.z,p1.z,p2.z,p3.z);
	return out;
}

void MatrixInterpolate(MatXf& out,const MatXf& a,const MatXf& b,float f)
{
	out.rot()[0][0]=LinearInterpolate(a.rot()[0][0],b.rot()[0][0],f);
	out.rot()[0][1]=LinearInterpolate(a.rot()[0][1],b.rot()[0][1],f);
	out.rot()[0][2]=LinearInterpolate(a.rot()[0][2],b.rot()[0][2],f);
	out.rot()[1][0]=LinearInterpolate(a.rot()[1][0],b.rot()[1][0],f);
	out.rot()[1][1]=LinearInterpolate(a.rot()[1][1],b.rot()[1][1],f);
	out.rot()[1][2]=LinearInterpolate(a.rot()[1][2],b.rot()[1][2],f);
	out.rot()[2][0]=LinearInterpolate(a.rot()[2][0],b.rot()[2][0],f);
	out.rot()[2][1]=LinearInterpolate(a.rot()[2][1],b.rot()[2][1],f);
	out.rot()[2][2]=LinearInterpolate(a.rot()[2][2],b.rot()[2][2],f);
	out.trans().x=LinearInterpolate(a.trans().x,b.trans().x,f);
	out.trans().y=LinearInterpolate(a.trans().y,b.trans().y,f);
	out.trans().z=LinearInterpolate(a.trans().z,b.trans().z,f);
}

void CheckMatrix(const MatXf& Matrix)
{
    /*
	const float good_trans=1e6f;
	const float good_rot=1e5f;
	VISASSERT(Matrix.trans().x>-good_trans && Matrix.trans().x<good_trans);
	VISASSERT(Matrix.trans().y>-good_trans && Matrix.trans().y<good_trans);
    //TODO Investigate why this fails on INFORB-3 ET map
	VISASSERT(Matrix.trans().z>-good_trans && Matrix.trans().z<good_trans);

	VISASSERT(Matrix.rot().xrow().x>-good_rot && Matrix.rot().xrow().x<good_rot);
	VISASSERT(Matrix.rot().xrow().y>-good_rot && Matrix.rot().xrow().y<good_rot);
	VISASSERT(Matrix.rot().xrow().z>-good_rot && Matrix.rot().xrow().z<good_rot);

	VISASSERT(Matrix.rot().yrow().x>-good_rot && Matrix.rot().yrow().x<good_rot);
	VISASSERT(Matrix.rot().yrow().y>-good_rot && Matrix.rot().yrow().y<good_rot);
	VISASSERT(Matrix.rot().yrow().z>-good_rot && Matrix.rot().yrow().z<good_rot);

	VISASSERT(Matrix.rot().zrow().x>-good_rot && Matrix.rot().zrow().x<good_rot);
	VISASSERT(Matrix.rot().zrow().y>-good_rot && Matrix.rot().zrow().y<good_rot);
	VISASSERT(Matrix.rot().zrow().z>-good_rot && Matrix.rot().zrow().z<good_rot);
    */
}

void SetOrthographic(Mat4f& out, int width, int height, int znear, int zfar) {
    out.xx = 2.0f / static_cast<float>(width);
    out.yy = 2.0f / static_cast<float>(height);
    out.zz = 2.0f / static_cast<float>(znear - zfar);
    out.ww = 1.0f;
    out.wx =  width < 0 ? 1.0f : -1.0f;
    out.wy = height < 0 ? 1.0f : -1.0f;
    out.wz = static_cast<float>(zfar + znear) / static_cast<float>(znear - zfar);
}

void Vect3fNormalize(Vect3f* out, const Vect3f* pv) {
    float norm = pv->norm();
    if (norm == 0) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    } else {
        out->x = pv->x / norm;
        out->y = pv->y / norm;
        out->z = pv->z / norm;
    }
}

void Vect3fTransformNormal(Vect3f* out, const Vect3f* pv, const Mat4f* pm) {
    const Mat4f m = *pm;
    const Vect3f v = *pv;

    out->x = m.xx * v.x + m.yx * v.y + m.zx * v.z;
    out->y = m.xy * v.x + m.yy * v.y + m.zy * v.z;
    out->z = m.xz * v.x + m.yz * v.y + m.zz * v.z;
}

Mat4f* Mat4fInverse(Mat4f *out, float *pdeterminant, const Mat4f *pm) {
    //Implementation copied from WINE project d3dx9_36/math.c

    float det, t[3], v[16];
    t[0] = pm->zz * pm->ww - pm->zw * pm->wz;
    t[1] = pm->yz * pm->ww - pm->yw * pm->wz;
    t[2] = pm->yz * pm->zw - pm->yw * pm->zz;
    v[0] = pm->yy * t[0] - pm->zy * t[1] + pm->wy * t[2];
    v[4] = -pm->yx * t[0] + pm->zx * t[1] - pm->wx * t[2];

    t[0] = pm->yx * pm->zy - pm->zx * pm->yy;
    t[1] = pm->yx * pm->wy - pm->wx * pm->yy;
    t[2] = pm->zx * pm->wy - pm->wx * pm->zy;
    v[8] = pm->ww * t[0] - pm->zw * t[1] + pm->yw * t[2];
    v[12] = -pm->wz * t[0] + pm->zz * t[1] - pm->yz * t[2];

    det = pm->xx * v[0] + pm->xy * v[4] +
          pm->xz * v[8] + pm->xw * v[12];

    if (det == 0.0f)
        return nullptr;
    if (pdeterminant)
        *pdeterminant = det;

    t[0] = pm->zz * pm->ww - pm->zw * pm->wz;
    t[1] = pm->xz * pm->ww - pm->xw * pm->wz;
    t[2] = pm->xz * pm->zw - pm->xw * pm->zz;
    v[1] = -pm->xy * t[0] + pm->zy * t[1] - pm->wy * t[2];
    v[5] = pm->xx * t[0] - pm->zx * t[1] + pm->wx * t[2];

    t[0] = pm->xx * pm->zy - pm->zx * pm->xy;
    t[1] = pm->wx * pm->xy - pm->xx * pm->wy;
    t[2] = pm->zx * pm->wy - pm->wx * pm->zy;
    v[9] = -pm->ww * t[0] - pm->zw * t[1]- pm->xw * t[2];
    v[13] = pm->wz * t[0] + pm->zz * t[1] + pm->xz * t[2];

    t[0] = pm->yz * pm->ww - pm->yw * pm->wz;
    t[1] = pm->xz * pm->ww - pm->xw * pm->wz;
    t[2] = pm->xz * pm->yw - pm->xw * pm->yz;
    v[2] = pm->xy * t[0] - pm->yy * t[1] + pm->wy * t[2];
    v[6] = -pm->xx * t[0] + pm->yx * t[1] - pm->wx * t[2];

    t[0] = pm->xx * pm->yy - pm->yx * pm->xy;
    t[1] = pm->wx * pm->xy - pm->xx * pm->wy;
    t[2] = pm->yx * pm->wy - pm->wx * pm->yy;
    v[10] = pm->ww * t[0] + pm->yw * t[1] + pm->xw * t[2];
    v[14] = -pm->wz * t[0] - pm->yz * t[1] - pm->xz * t[2];

    t[0] = pm->yz * pm->zw - pm->yw * pm->zz;
    t[1] = pm->xz * pm->zw - pm->xw * pm->zz;
    t[2] = pm->xz * pm->yw - pm->xw * pm->yz;
    v[3] = -pm->xy * t[0] + pm->yy * t[1] - pm->zy * t[2];
    v[7] = pm->xx * t[0] - pm->yx * t[1] + pm->zx * t[2];

    v[11] = -pm->xx * (pm->yy * pm->zw - pm->yw * pm->zy) +
            pm->yx * (pm->xy * pm->zw - pm->xw * pm->zy) -
            pm->zx * (pm->xy * pm->yw - pm->xw * pm->yy);

    v[15] = pm->xx * (pm->yy * pm->zz - pm->yz * pm->zy) -
            pm->yx * (pm->xy * pm->zz - pm->xz * pm->zy) +
            pm->zx * (pm->xy * pm->yz - pm->xz * pm->yy);

    det = 1.0f / det;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*out)[i][j] = v[4 * i + j] * det;
        }
    }

    return out;
}

Mat4f * Mat4fLookAtLH(Mat4f* out, const Vect3f* eye, const Vect3f* at, const Vect3f* up) {
    //Implementation copied from WINE project d3dx9_36/math.c
    Vect3f right, upn, vec;

    vec = *at - *eye;
    Vect3fNormalize(&vec, &vec);
    right.cross(*up, vec);
    upn.cross(vec, right);
    Vect3fNormalize(&right, &right);
    Vect3fNormalize(&upn, &upn);
    out->xx = right.x;
    out->yx = right.y;
    out->zx = right.z;
    out->wx = -right.dot(*eye);
    out->xy = upn.x;
    out->yy = upn.y;
    out->zy = upn.z;
    out->wy = -upn.dot(*eye);
    out->xz = vec.x;
    out->yz = vec.y;
    out->zz = vec.z;
    out->wz = -vec.dot(*eye);
    out->xw = 0.0f;
    out->yw = 0.0f;
    out->zw = 0.0f;
    out->ww = 1.0f;

    return out;
}

void Mat4fConvert(const Mat4f& m, const Vect3f& in,Vect3f& pv,Vect3f& pe) {
    pv.x   =m.xx*in.x + m.yx*in.y + m.zx*in.z + m.wx;
    pv.y   =m.xy*in.x + m.yy*in.y + m.zy*in.z + m.wy;
    pv.z   =m.xz*in.x + m.yz*in.y + m.zz*in.z + m.wz;
    float d=m.xw*in.x + m.yw*in.y + m.zw*in.z + m.ww;
    if(d==0) d=1e30f; else d=1/d;
    pe.x=pv.x*d;
    pe.y=pv.y*d;
    pe.z=pv.z*d;
}

std::string toColorCode(const sColor4f& color) {
    std::string result;
    char buff[7];
    snprintf(buff, 7, "%x", color.GetRGB());
    buff[6] = 0;
    result += buff;
    while (result.length() < 6) {
        result = "0" + result;
    }
    return result;
};
