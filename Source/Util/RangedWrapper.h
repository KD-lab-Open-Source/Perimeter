#ifndef __RANGED_WRAPPER_H_INCLUDED__
#define __RANGED_WRAPPER_H_INCLUDED__

#include "Range.h"

template<typename ScalarType = float>
class RangedWrapper
{
public:
    typedef Range<ScalarType> RangeType;
    typedef RangedWrapper<ScalarType> RangedWrapperType;

    RangedWrapper (ScalarType& _value, RangeType& _range, ScalarType _step = ScalarType (0)) :
        value_ (_value),
        range_ (_range),
        step_ (_step)
    {}

    RangedWrapper (ScalarType& _value, ScalarType _range_min, ScalarType _range_max, ScalarType _step = ScalarType (0)) :
        value_ (_value),
        range_ (RangeType (_range_min, _range_max)),
        step_ (_step)
    {}

    ~RangedWrapper ()
    {
        clip ();
    }
    inline ScalarType& value () 
    {
        return value_;
    }
    inline operator ScalarType () const
    {
        return value_;
    }
    inline RangedWrapperType& operator= (const ScalarType& _value)
    {
        value_ = _value;
		return *this;
    }
    inline RangeType range () const
    {
        return range_;
    }
    inline void clip ()
    {
        value_ = range_.clip (value_);
    }
    inline ScalarType step () const
    {
        return step_;
    }
private:
    RangeType range_;
    ScalarType step_;
    ScalarType& value_;
};

typedef RangedWrapper<float> RangedWrapperf;

#endif
