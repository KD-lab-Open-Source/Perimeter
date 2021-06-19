#pragma once

#ifndef _BGSCENE_H
#define _BGSCENE_H

class BGScene {

	public:

		BGScene();
		~BGScene();

		void init(cVisGeneric* visGeneric);
		void done();
		bool ready() const;

		void quant(float dt);
		void preDraw();
		void draw();
		void postDraw();

		void reset();

		void setProgress(float progress);

		bool isPlaying() const;
		void play();
		void markToPlay(const char* objName, const char* chainName, bool forward);
		void markAllToPlay(bool forward);
		void unmarkToPlay(const char* objName, const char* chainName, bool forward);

		void onResolutionChanged();

		void setEnabled(bool state) {
			enabled = state;
		}

		void setSkinColor(const sColor4f& color = sColor4f(1, 1, 1, 1)) {
			if ((!(color == skinColor)) && bgObj) {
				skinColor = color;
				bgObj->SetSkinColor(color);
			}
		}

		bool inited() const {
			return (scene != 0);
		}

	protected:

		cUnkLight* light;
		cScene* scene;
		cCamera* camera;

		cObjectNodeRoot	*bgObj;

		float timer;
		float liveTimer;

		bool enabled;

		struct SubObject {
			cObjectNode* node;
			cObjectNode* liveGroupNode;
			bool stopped;
			bool forwardDirection;
			std::string chainName;
		};

		std::vector<SubObject> subObjects;

		sColor4f skinColor;
};

#endif //_BGSCENE_H