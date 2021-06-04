#ifndef _RECT4I_H
#define _RECT4I_H

#include <xmath.h>

struct Rect4i {
	Vect2i	min;
	Vect2i	max;
	
	Rect4i() {
	}

	Rect4i(int xMin, int yMin, int xMax, int yMax) {
		set(xMin, yMin, xMax, yMax);
	}

	Rect4i(const Rect4i &rect) {
		min = rect.min;
		max = rect.max;
	}

	inline void set(int xMin, int yMin, int xMax, int yMax) {
		min.x = xMin;
		min.y = yMin;
		max.x = xMax;
		max.y = yMax;
	}

	inline int xMin() const {
		return min.x;
	}
	inline int yMin() const {
		return min.y;
	}
	inline int xMax() const {
		return max.x;
	}
	inline int yMax() const {
		return max.y;
	}
	inline int& xMin() {
		return min.x;
	}
	inline int& yMin() {
		return min.y;
	}
	inline int& xMax() {
		return max.x;
	}
	inline int& yMax() {
		return max.y;
	}

	inline int getWidth() const {
		return max.x - min.x;
	}

	inline int getHeight() const {
		return max.y - min.y;
	}

	inline void unZoom(int degree) {
/*
		min.x = unZoom(min.x, degree);
		min.y = unZoom(min.y, degree);
		max.x = unZoom(max.x, degree);
		max.y = unZoom(max.y, degree);
*/

		min.x = round( ((float)min.x / (float)degree) + 0.5f );
		min.y = round( ((float)min.y / (float)degree) + 0.5f );
		max.x = unZoom(max.x, degree);
		max.y = unZoom(max.y, degree);

	}

	inline bool inside(int x, int y) const {
		return 		   
			   x > min.x
			&& y > min.y
			&& x < max.x
			&& y < max.y;
	}

	inline void shift(int x, int y) {
		min.x += x;
		min.y += y;
		max.x += x;
		max.y += y;
	}

	inline void shift(const Vect2i& dist) {
		min += dist;
		max += dist;
	}

	inline void intersect(const Rect4i& rect) {
		min.x = ::max(rect.xMin(), min.x);
		min.y = ::max(rect.yMin(), min.y);
		max.x = ::min(rect.xMax(), max.x);
		max.y = ::min(rect.yMax(), max.y);
	}

	inline Rect4i getIntersection(const Rect4i& rect) const {
		return Rect4i(	::max(rect.xMin(), min.x),
						::max(rect.yMin(), min.y),
						::min(rect.xMax(), max.x),
						::min(rect.yMax(), max.y)	);
	}

	static int unZoom(int src, int degree) {
		return round( ((float)src / (float)degree) - 0.5f );
	}
};

#endif // _RECT4I_H