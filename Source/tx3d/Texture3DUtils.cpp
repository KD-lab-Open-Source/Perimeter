/**
 *	"Procedural 3D Texture Library"
 *	
 *	Class contains general utility methods.
 *
 *	@version $Id: Texture3DUtils.h,v 1.8 2002/11/05 17:55:27 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Texture3DUtils.hpp"

using namespace tx3d;

Vector3D Texture3DUtils::convertHSBtoRGB(const Vector3D& hsbColor) {
	Vector3D rgbColor(0.0, 0.0, 0.0);

	if (hsbColor.y == 0) {
		rgbColor.x = rgbColor.y = rgbColor.z = hsbColor.z;
	} else {
		float h = hsbColor.x * 6.0f;
//		float f = Texture3DUtils::reminder(h - 0.5f);
		float f = h - Texture3DUtils::floor(h);
		float p = hsbColor.z * (1.0f - hsbColor.y);
		float q = hsbColor.z * (1.0f - hsbColor.y * f);
		float t = hsbColor.z * (1.0f - (hsbColor.y * (1.0f - f)));
		switch (Texture3DUtils::floor(h)) {
			case 0:
				rgbColor.x = hsbColor.z; 
				rgbColor.y = t; 
				rgbColor.z = p; 
				break;
			case 1:
				rgbColor.x = q; 
				rgbColor.y = hsbColor.z; 
				rgbColor.z = p; 
				break;
			case 2:
				rgbColor.x = p; 
				rgbColor.y = hsbColor.z; 
				rgbColor.z = t; 
				break;
			case 3:
				rgbColor.x = p; 
				rgbColor.y = q; 
				rgbColor.z = hsbColor.z; 
				break;
			case 4:
				rgbColor.x = t; 
				rgbColor.y = p; 
				rgbColor.z = hsbColor.z; 
				break;
			case 5:
				rgbColor.x = hsbColor.z; 
				rgbColor.y = p; 
				rgbColor.z = q; 
				break;
		}

	}

/*
	if (hsbColor.z == 0) {
		// safety short circuit again
		return Vector3D(0, 0, 0);
	}

	if (hsbColor.y == 0) {
		return Vector3D(hsbColor.z, hsbColor.z, hsbColor.z);
	}

	float domainOffset;         // hue mod 1/6

	if (hsbColor.x < 1 / 6) {
		// red domain; green ascends
		domainOffset = hsbColor.x;
		rgbColor.x = hsbColor.z;
		rgbColor.z = hsbColor.z * (1 - hsbColor.y);
		rgbColor.y = rgbColor.z + (hsbColor.z - rgbColor.z) * domainOffset * 6;
	} else if (hsbColor.x < 2 / 6) {
		// yellow domain; red descends
		domainOffset = hsbColor.x - 1 / 6;
		rgbColor.y = hsbColor.z;
		rgbColor.z = hsbColor.z * (1 - hsbColor.y);
		rgbColor.x = rgbColor.y + (hsbColor.z - rgbColor.z) * domainOffset * 6;
	} else if (hsbColor.x < 3 / 6) {
		// green domain; blue ascends
		domainOffset = hsbColor.x - 2 / 6;
		rgbColor.y = hsbColor.z;
		rgbColor.x = hsbColor.z * (1 - hsbColor.y);
		rgbColor.z = rgbColor.x + (hsbColor.z - rgbColor.x) * domainOffset * 6;
	} else if (hsbColor.x < 4 / 6) {
		// cyan domain; green descends
		domainOffset = hsbColor.x - 3 / 6;
		rgbColor.z = hsbColor.z;
		rgbColor.x = hsbColor.z * (1 - hsbColor.y);
		rgbColor.y = rgbColor.z + (hsbColor.z - rgbColor.x) * domainOffset * 6;
	} else if (hsbColor.x < 5 / 6) {
		// blue domain; red ascends
		domainOffset = hsbColor.x - 4 / 6;
		rgbColor.z = hsbColor.z;
		rgbColor.y = hsbColor.z * (1 - hsbColor.y);
		rgbColor.x = rgbColor.y + (hsbColor.z - rgbColor.y) * domainOffset * 6;
	} else {
		// magenta domain; blue descends
		domainOffset = hsbColor.x - 5 / 6;
		rgbColor.x = hsbColor.z;
		rgbColor.y = hsbColor.z * (1 - hsbColor.y);
		rgbColor.z = rgbColor.x + (hsbColor.z - rgbColor.y) * domainOffset * 6;
	}
*/
	return rgbColor;
}

Vector3D Texture3DUtils::convertRGBtoHSB(const Vector3D& rgbColor) {
	Vector3D hsbColor(0.0, 0.0, 0.0);

	hsbColor.z = (rgbColor.x > rgbColor.y) ? rgbColor.x : rgbColor.y;
	if (rgbColor.z > hsbColor.z) {
		hsbColor.z = rgbColor.z;
	}
	float cmin = (rgbColor.x < rgbColor.y) ? rgbColor.x : rgbColor.y;
	if (rgbColor.z < cmin) {
		cmin = rgbColor.z;
	}

	if (hsbColor.z != 0.0f) {
	    hsbColor.y = (hsbColor.z - cmin) / hsbColor.z;
	} else {
	    hsbColor.y = 0.0f;
	}

	if (hsbColor.y == 0.0f) {
	    hsbColor.x = 0.0f;
	} else {
	    float redc = (hsbColor.z - rgbColor.x) / (hsbColor.z - cmin);
	    float greenc = (hsbColor.z - rgbColor.y) / (hsbColor.z - cmin);
	    float bluec = (hsbColor.z - rgbColor.z) / (hsbColor.z - cmin);

		if (rgbColor.x == hsbColor.z) {
			hsbColor.x = bluec - greenc;
		} else if (rgbColor.y == hsbColor.z) {
			hsbColor.x = 2.0f + redc - bluec;
		} else {
			hsbColor.x = 4.0f + greenc - redc;
		}

		hsbColor.x = hsbColor.x / 6.0f;

		if (hsbColor.x < 0.0f) {
			hsbColor.x = hsbColor.x + 1.0f;
		}
	}

/*
	hsbColor.z = (rgbColor.x > rgbColor.y) ? rgbColor.x : rgbColor.y;
	if (rgbColor.z > hsbColor.z) {
		hsbColor.z = rgbColor.z;
	}

	if (hsbColor.z == 0) {
		// short-circuit now and avoid division by zero problems later
		return Vector3D(0, 0, 0);
	}

	float desaturator = (rgbColor.x < rgbColor.y) ? rgbColor.x : rgbColor.y;
	if (rgbColor.z < desaturator) {
		desaturator = rgbColor.z;
	}

	if (hsbColor.z == desaturator) {
		// we're grey (and still have division by zero issues to bypass)
		hsbColor.x = 0;
		hsbColor.y = 0;
		return hsbColor;
	}

	hsbColor.y = (hsbColor.z - desaturator) / hsbColor.z;

	float midigator

    int midigator = mid(red, green, blue);

    // "domains" are 60 degrees of red, yellow, green, cyan, blue, or magenta
    // compute how far we are from a domain base
    float domainBase;
    float oneSixth = 1.0f / 6.0f;
    float domainOffset = (midigator - desaturator) /
                         (float)(bri - desaturator) / 6.0;
    if (red == bri)
    {
        if (midigator == green)
	{   // green is ascending
            domainBase = 0 / 6.0;       // red domain
	}
          else
	  { // blue is descending
            domainBase = 5 / 6.0;       // magenta domain
	    domainOffset = oneSixth - domainOffset;
	  }
    }
      else if (grn == bri)
      {
        if (midigator == blue)
	{   // blue is ascending
            domainBase = 2 / 6.0;       // green domain
	}
          else
	  { // red is descending
            domainBase = 1 / 6.0;       // yellow domain
	    domainOffset = oneSixth - domainOffset;
	  }
      }
      else
      {
        if (midigator == red)
	{   // red is ascending
            domainBase = 4 / 6.0;       // blue domain
	}
          else
	  { // green is descending
            domainBase = 3 / 6.0;       // cyan domain
	    domainOffset = oneSixth - domainOffset;
	  }
      }

    hue = domainBase + domainOffset;
*/
	return hsbColor;
}
