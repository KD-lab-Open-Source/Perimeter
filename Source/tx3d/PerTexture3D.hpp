/**
 *	"Procedural 3D Texture Library"
 *	
 *	PerTexture3D implements a Texture3D, that is perturbed by another Texture3D.
 *
 *	@version $Id: PerTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_PERTEXTURE3D_H
#define _TX3D_PERTEXTURE3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class PerTexture3D : public Texture3D {

		public :
			PerTexture3D(const string& paramString);

			PerTexture3D(Texture3D *core = 0, Texture3D *perturbator = 0);

			~PerTexture3D();
			
			void getColor(Vector3D* destClr, const Vector3D &v) {
				perturbator->getColor(&perturbValue, v);
				perturbValue += v;
				core->getColor(destClr, perturbValue);
			}

			Texture3D* getCore() const {
				return core;
			}

			void setCore(Texture3D* t);

			Texture3D* getPerturbator() const {
				return perturbator;
			}

			void setPerturbator(Texture3D* t);

			const string& getType() const {
				return TYPE_NAME;
			}

			void zoom(float degree) {
				core->zoom(degree);
				perturbator->zoom(degree);
				perturbator->zoomColor(degree);
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string CORE;
			static const string PERTURBATOR;

		protected :
			Vector3D perturbValue;

			Texture3D *core;
			Texture3D *perturbator;
	};

}

#endif