#include "StdAfx.h"
#include "xmath.h"

int RandomGenerator::operator()()
{
    // Generates random value [0..max_value], non-inline due to some bugs with optimization
    return rand() % (max_value + 1);
}

RandomGenerator xm_random_generator;

const Vect2i Vect2i::ID = Vect2i(1, 1);
const Vect2i Vect2i::ZERO = Vect2i(0, 0);
const Vect2f Vect2f::ZERO = Vect2f(0, 0);
const Vect3f Vect3f::I = Vect3f(1, 0, 0);
const Vect3f Vect3f::J = Vect3f(0, 1, 0);
const Vect3f Vect3f::K = Vect3f(0, 0, 1);
const Vect3f Vect3f::I_ = Vect3f(-1, 0, 0);
const Vect3f Vect3f::J_ = Vect3f(0, -1, 0);
const Vect3f Vect3f::K_ = Vect3f(0, 0, -1);
const Vect3f Vect3f::ID = Vect3f(1, 1, 1);
const Vect3f Vect3f::ZERO = Vect3f(0, 0, 0);
const QuatF QuatF::ID = QuatF(1, 0, 0, 0);
const Mat2f Mat2f::ID = Mat2f(1, 0, 0, 1);
const Mat3f Mat3f::ID = Mat3f(1, 0, 0, 0, 1, 0, 0, 0, 1);
const Mat3f Mat3f::ZERO = Mat3f(0, 0, 0, 0, 0, 0, 0, 0, 0);
const MatXf MatXf::ID = MatXf(Mat3f::ID, Vect3f::ID);
const Se3f Se3f::ID = Se3f(Mat3f::ID, Vect3f::ID);

Mat3f& Mat3f::mult(const Mat3f& M, const Mat3f& N)
{
    // M * N	   [!]

    this->xx = M.xx * N.xx + M.xy * N.yx + M.xz * N.zx;
    this->xy = M.xx * N.xy + M.xy * N.yy + M.xz * N.zy;
    this->xz = M.xx * N.xz + M.xy * N.yz + M.xz * N.zz;

    this->yx = M.yx * N.xx + M.yy * N.yx + M.yz * N.zx;
    this->yy = M.yx * N.xy + M.yy * N.yy + M.yz * N.zy;
    this->yz = M.yx * N.xz + M.yy * N.yz + M.yz * N.zz;

    this->zx = M.zx * N.xx + M.zy * N.yx + M.zz * N.zx;
    this->zy = M.zx * N.xy + M.zy * N.yy + M.zz * N.zy;
    this->zz = M.zx * N.xz + M.zy * N.yz + M.zz * N.zz;
    return *this;
}

Mat3f& Mat3f::premult(const Mat3f& M)
{
    // M * this  [!]
    *this = mult(M, *this);
    return *this;
}
Mat3f& Mat3f::postmult(const Mat3f& M)
{
    // this * M  [!]
    *this = mult(*this, M);
    return *this;
}

QuatF& QuatF::mult(const QuatF& p, const QuatF& q)
{
    // p * q	   [!]

    Vect3f vp{ p.x_, p.y_, p.z_ };
    Vect3f vq{ q.x_, q.y_, q.z_ };

    Vect3f c; c.cross(vp, vq);
    Vect3f v = vp.scale(q.s_) + vq.scale(p.s_) + c;

    s_ = p.s_ * q.s_ - vp.dot(vq);
    x_ = v.x;
    y_ = v.y;
    z_ = v.z;

    return *this;
}

Mat3f& Mat3f::set(const QuatF& q)
{
    // FIXME
    return *this;
}

Mat3f& Mat3f::set(const Vect3f& axis, float angle, int normalizeAxis /*= 1*/)
{
    // FIXME
    return *this;
}


Mat3f& Mat3f::set(const Vect3f& x_from, const Vect3f& y_from, const Vect3f& z_from,
    const Vect3f& x_to /*= Vect3f::I*/, const Vect3f& y_to /*= Vect3f::J*/, const Vect3f& z_to /*= Vect3f::K*/)
{
    // FIXME
    return *this;
}


int  Mat3f::invert()
{
    // this^-1, returns one if the matrix was not invertible, otherwise zero.
    // FIXME
    return 0;
}
int  Mat3f::invert(const Mat3f& M)
{
    // M^-1	   [!]
    // FIXME
    return 0;
}
// That is, works for non-scaled matrix !!!
MatXf& MatXf::invert(const MatXf& M)
{
    // M^-1	   [!]
    // FIXME
    return *this;
}

MatXf& MatXf::invert()
{
    // this^-1
    // FIXME
    return *this;
}

// Really inverts 3x3-matrix.
MatXf& MatXf::Invert(const MatXf& M)
{
    // M^-1	   [!]
    // FIXME
    return *this;
}
MatXf& MatXf::Invert()
{
    // this^-1
    // FIXME
    return *this;
}

MatXf& MatXf::mult(const MatXf& M, const MatXf& N)
{
    // M * N	   [!]
        // FIXME
    return *this;
}
MatXf& MatXf::premult(const MatXf& M)
{
    // M * this  [!]
        // FIXME
    return *this;
}
MatXf& MatXf::postmult(const MatXf& M)
{
    // this * M  [!]
        // FIXME
    return *this;
}
QuatF& QuatF::premult(const QuatF& q)
{
    // q * this  [!]

    *this = mult(q, *this);
    return *this;
}

QuatF& QuatF::postmult(const QuatF& q)
{
    // this * q  [!]


    *this = mult(*this, q);
    return *this;
}

Vect3f& QuatF::xform(const Vect3f& u, Vect3f& v) const
{
    // this (v 0) this^-1 => xv
    // FIXME
    return Vect3f(0, 0, 0);
}
Vect3f& QuatF::xform(Vect3f& v) const
{
    // this (v 0) this^-1 => v
    // FIXME
    return Vect3f(0, 0, 0);
}

// set a quaternion to a rotation of 'angle' radians about 'axis'
  // the axis passed is automatically normalized unless normalizeAxis = 0
QuatF& QuatF::set(float angle, const Vect3f& axis, int normalizeAxis /*= 1*/)
{
    //FIXME
    return *this;
}

QuatF& QuatF::set(const Mat3f& R)
{
    //FIXME
    return *this;
}

XStream& operator<= (XStream& s, const Vect2f& v)
{
    // FIXME
    return s;
}
XStream& operator>= (XStream& s, Vect2f& v)
{
    // FIXME
    return s;
}
XStream& operator< (XStream& s, const Vect2f& v)
{
    // FIXME
    return s;
}
XStream& operator> (XStream& s, Vect2f& v)
{
    // FIXME
    return s;
}

XBuffer& operator<= (XBuffer& b, const Vect2f& v)
{
    // FIXME
    return b;
}
XBuffer& operator>= (XBuffer& b, Vect2f& v)
{
    // FIXME
    return b;
}
XBuffer& operator< (XBuffer& b, const Vect2f& v)
{
    // FIXME
    return b;
}
XBuffer& operator> (XBuffer& b, Vect2f& v)
{
    // FIXME
    return b;
}

XStream& operator<= (XStream& s, const Vect2i& v)
{
    // FIXME
    return s;
}
XStream& operator>= (XStream& s, Vect2i& v)
{
    // FIXME
    return s;
}
XStream& operator< (XStream& s, const Vect2i& v)
{
    // FIXME
    return s;
}
XStream& operator> (XStream& s, Vect2i& v)
{
    // FIXME
    return s;
}

XBuffer& operator<= (XBuffer& b, const Vect2i& v)
{
    // FIXME
    return b;
}
XBuffer& operator>= (XBuffer& b, Vect2i& v)
{
    // FIXME
    return b;
}
XBuffer& operator< (XBuffer& b, const Vect2i& v)
{
    // FIXME
    return b;
}
XBuffer& operator> (XBuffer& b, Vect2i& v)
{
    // FIXME
    return b;
}


XStream& operator<= (XStream& s, const Vect3f& v)
{
    // FIXME
    return s;
}
XStream& operator>= (XStream& s, Vect3f& v)
{
    // FIXME
    return s;
}
XStream& operator< (XStream& s, const Vect3f& v)
{
    // FIXME
    return s;
}
XStream& operator> (XStream& s, Vect3f& v)
{
    // FIXME
    return s;
}

XBuffer& operator<= (XBuffer& b, const Vect3f& v)
{
    // FIXME
    return b;
}
XBuffer& operator>= (XBuffer& b, Vect3f& v)
{
    // FIXME
    return b;
}
XBuffer& operator< (XBuffer& b, const Vect3f& v)
{
    // FIXME
    return b;
}
XBuffer& operator> (XBuffer& b, Vect3f& v)
{
    // FIXME
    return b;
}


XStream& operator<= (XStream& s, const QuatF& q)
{
    // FIXME
    return s;
}
XStream& operator>= (XStream& s, QuatF& q)
{
    // FIXME
    return s;
}
XStream& operator< (XStream& s, const QuatF& q)
{
    // FIXME
    return s;
}
XStream& operator> (XStream& s, QuatF& q)
{
    // FIXME
    return s;
}

XBuffer& operator<= (XBuffer& b, const QuatF& q)
{
    // FIXME
    return b;
}
XBuffer& operator>= (XBuffer& b, QuatF& q)
{
    // FIXME
    return b;
}
XBuffer& operator< (XBuffer& b, const QuatF& q)
{
    // FIXME
    return b;
}
XBuffer& operator> (XBuffer& b, QuatF& q)
{
    // FIXME
    return b;
}