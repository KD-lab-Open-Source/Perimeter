/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: PerTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_FLATMODELXTEXTURE3D_H
#define _TX3D_FLATMODELXTEXTURE3D_H

#include "FlatModelTexture3D.hpp"

namespace tx3d {

	class FlatModelXTexture3D : public FlatModelTexture3D {

		public :
			FlatModelXTexture3D(const string& paramString) : FlatModelTexture3D(paramString) {
			}

			FlatModelXTexture3D	(	const Vector3D& shift,
									const Vector3D& scale,
									const string& src	)
								: FlatModelTexture3D (
									shift,
									scale,
									src	) {
			}

			FlatModelXTexture3D(const FlatModelTexture3D* origin)
								 : FlatModelTexture3D(origin) {
			}

			FlatModelXTexture3D	(	const Vector3D& shift,
									const Vector3D& scale,
									Vector3D* flatModel,
									int modelWidth,
									int modelHeight	)
								: FlatModelTexture3D (
									shift,
									scale,
									flatModel,
									modelWidth,
									modelHeight	) {
			}

			void getColor(Vector3D* destClr, const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

}

#endif