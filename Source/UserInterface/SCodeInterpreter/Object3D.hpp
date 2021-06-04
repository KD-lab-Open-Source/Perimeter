#pragma once

#ifndef _OBJECT3D_HPP
#define _OBJECT3D_HPP

#include "SharedPointer.hpp"

class Object3D : public tx3d::SharedPointer {
	public:
		virtual void quant(float dt) = 0;
		virtual int getState() const = 0;
		virtual bool setState(int newState) = 0;
		virtual void updateColor(const Vect3f& cameraPos) = 0;
};

#endif //_OBJECT3D_HPP