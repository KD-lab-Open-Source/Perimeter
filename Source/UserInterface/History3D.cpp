#include "StdAfx.h"
#include "History3D.h"
#include "GameShellSq.h"

void WorldSphere::quant(float dt) {
	timeEvolvingElapsed += dt;
	if (timeEvolvingElapsed > EVOLVE_TIME) {
		timeEvolvingElapsed = 0;
		elasticSphere->evolve();
	}
	timeElapsed += dt;
	timeTestamentRemaining -= dt;
	if (timeTestamentRemaining < 0) {
		timeTestamentRemaining = 0;
	}
	if (timeDissolveRemaining > 0) {
		timeDissolveRemaining -= dt;
		if (timeDissolveRemaining < 0) {
			timeDissolveRemaining = 0;
		}
	}
}

void WorldSphere::startTestamentEffect(int level) {
	timeTestamentRemaining = TESTAMENT_EFFECT_PERIOD + (level - 1) * TESTAMENT_EFFECT_DELAY;
}

void LinkTubule::quant(float dt) {
	timeEvolvingElapsed += dt;
	if (timeEvolvingElapsed > EVOLVE_TIME) {
		timeEvolvingElapsed = 0;
		elasticLink->evolve();
	}
	timeElapsed += dt;
}

bool WorldSphere::setState(int newState) {
	if (state == newState) {
		return false;
	}
	state = newState;
	return true;
}

bool LinkTubule::setState(int newState) {
	if (state == newState) {
		return false;
	}
	state = newState;
	return true;
}

void WorldSphere::updateColor(const Vect3f& cameraPos) {
	float distance = elasticSphere->position().distance(-cameraPos);
	if (timeDissolveRemaining == 0 || distance < 0 || distance >= SPHERE_VISIBLE_DISTANCE) {
		elasticSphere->SetAttr(ATTRUNKOBJ_IGNORE);
	} else {
		if (timeDissolveRemaining > 0) {
			elasticSphere->radius = radius * (timeDissolveRemaining / DISSOLVE_TIME);
		}
		elasticSphere->ClearAttr(ATTRUNKOBJ_IGNORE);
		float f = 1.0f - distance / SPHERE_VISIBLE_DISTANCE;
		sColor4f newColor(sphereColor);
		newColor.a *= f;
/*		if (timeTestamentRemaining != 0 && timeTestamentRemaining <= TESTAMENT_EFFECT_PERIOD) {
			float ra = 2 * timeTestamentRemaining / TESTAMENT_EFFECT_PERIOD;
			if (ra > 1) {
				ra -= 1.0f;
			}
			newColor.g *= ra;
			newColor.b *= ra;
//			newColor.g *= 0;
//			newColor.b *= 0;
		}
*/		elasticSphere->SetColor( 0, &newColor );
	}
}
void LinkTubule::updateColor(const Vect3f& cameraPos) {
	sColor4f d;
	sColor4f d1;
	sColor4f d2;
	elasticLink->sphere1->GetColor( 0, &d1 );
	elasticLink->sphere2->GetColor( 0, &d2 );
	d = (d1 + d2) * 0.5f;
	if (state == BLINKING) {
		float f = 2 * fmod(timeElapsed, BUILDING_LINK_BLINKING_PERIOD) / BUILDING_LINK_BLINKING_PERIOD;
		d.a *= (f > 1) ? (2.0f - f) : f;
	}
	elasticLink->SetColor( 0, &d );
}
void Frame3D::updateColor(const Vect3f& cameraPos) {
	float distance = frameObj->GetGlobalMatrix().trans().distance(-cameraPos);
	if (!alive || distance < 0 || distance >= SPHERE_VISIBLE_DISTANCE) {
		frameObj->SetAttr(ATTRUNKOBJ_IGNORE);
	} else {
		frameObj->ClearAttr(ATTRUNKOBJ_IGNORE);
		float f = 1.0f - distance / SPHERE_VISIBLE_DISTANCE;
        sColor4f color(1, 1, 1, f);
		frameObj->SetColor( 0, &color );
	}
}

void WorldSphere::setupFrame3D(Frame3D* frame3D) {
	frameCount++;
	MatXf mat = getElasticSphere()->GetGlobalMatrix();
//	Rotate(mat, Vect3f(0, (frameCount - 1), 0));
	float shift = getElasticSphere()->radius * 9.0f / 15.0f;
	switch (frameCount) {
		case 2:
			Translate(mat, Vect3f(0, shift, 0));
			break;
		case 3:
			Translate(mat, Vect3f(0, -shift, 0));
			break;
		case 4:
			Translate(mat, Vect3f(shift, 0, 0));
			break;
		case 5:
			Translate(mat, Vect3f(-shift, 0, 0));
			break;
		case 6:
			Translate(mat, Vect3f(0, 0, shift));
			break;
		case 7:
			Translate(mat, Vect3f(0, 0, -shift));
			break;
	}
	frame3D->getFrameObj()->SetPosition( mat );
}

void Frame3D::quant(float dt) {
	if (flareEffectTime) {
		flareEffectTime -= dt;
		if (flareEffectTime <= 0) {
			flareEffectTime = 0;
			frameObj->SetChannel("main", true);
		} else {
			frameObj->SetPhase(1.0f - flareEffectTime / JUMP_FLARE_PERIOD, true);
		}
	}
}

void Frame3D::tryFlareEffect() {
	setPlumeEffect(false);
	if (shouldStartFlareEffect) {
		flareEffectTime = JUMP_FLARE_PERIOD;
		frameObj->SetChannel("flare", true);
	}
}
void Frame3D::setPlumeEffect(bool on) {
	if (on && !hasPlumeEffect) {
		flareEffectTime = 0;
		frameObj->SetChannel("effect", true);
	} else if (!on && hasPlumeEffect) {
		frameObj->SetChannel("main", true);
	}
	hasPlumeEffect = on;
}

void Frame3D::createObj(cScene* scene) {
	if (frameObj) {
		frameObj->Release();
		frameObj = 0;
	}
	string pathName;
	switch (race) {
		case 0:
			//empire
			pathName = "RESOURCE\\MODELS\\MENU\\intrf_frame_E.M3D";
			break;
		case 1:
			//exodus
			pathName = "RESOURCE\\MODELS\\MENU\\intrf_frame_EX.M3D";
			break;
		default:
			//hark
			pathName = "RESOURCE\\MODELS\\MENU\\intrf_frame_H.M3D";
	}
	
	frameObj = scene->CreateObject(pathName.c_str(), "RESOURCE\\MODELS\\MAIN\\TEXTURES\\");

	frameObj->SetAttr(ATTRUNKOBJ_NOLIGHT);
	frameObj->SetScale(Vect3f(HISTORY_FRAME_MODEL_SCALE, HISTORY_FRAME_MODEL_SCALE, HISTORY_FRAME_MODEL_SCALE));
}

void Frame3D::setRace(cScene* scene, int newRace) {
	if (race != newRace) {
		race = newRace;
		createObj(scene);
	}
}

void WorldSphere::setDead() {
	if (timeDissolveRemaining < 0) {
		timeDissolveRemaining = DISSOLVE_TIME;
		radius = elasticSphere->radius;
	}
}
