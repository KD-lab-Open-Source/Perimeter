#pragma once

#ifndef _FRAME_HPP
#define _FRAME_HPP

#include "SharedPointer.hpp"
#include "World.hpp"
#include "Knowledge.hpp"

class Frame : public tx3d::SharedPointer {
	public:
		Frame(const std::string& name, World* parentWorld, int race, int colorIndex);
		virtual ~Frame();
		void addKnowledge(const Knowledge& newKnowledge);
		void goToWorld(World* destWorld);

		void destroy() {
			alive = false;
		}
		bool isAlive() const {
			return alive;
		}
		const Knowledge& getKnowledge() const {
			return knowledge;
		}
		const std::string& getName() const {
			return name;
		}
		World* getCurrentWorld() const {
			return getKnowledge().getPath().back();
		}
		Object3D* getFrame3D() {
			return frame3D;
		}
		void setRace(int newRace) {
			race = newRace;
		}
		int getRace() const {
			return race;
		}

		void setColorIndex(int newColorIndex) {
			colorIndex = newColorIndex;
		}
		int getColorIndex() const {
			return colorIndex;
		}

		void setFrame3D(Object3D* newFrame3D);
	protected:
		bool alive;
		Knowledge knowledge;
		std::string name;
		int race;
		int colorIndex;

		Object3D* frame3D;
};



#endif //_FRAME_HPP
