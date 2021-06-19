#pragma once

#ifndef _CONTAINERS_HPP
#define _CONTAINERS_HPP

#include "World.hpp"
#include "Frame.hpp"

class Worlds {
	public:
		//reserve
		~Worlds();
		void addNewWorld(World* world);
		World* getWorldBySysName(const std::string& sysName);
		void clear();
//	protected:
		std::map <std::string, World*> worlds;
};

class Frames {
	public:
		//reserve
		~Frames();
		void addNewFrame(Frame* frame);
		Frame* getFrameByName(const std::string& name);
		void clear();
		int knowledgeArrivedToWorld(const Knowledge& knowledge, World* world);
//	protected:
		std::map <std::string, Frame*> frames;
};

#endif //_CONTAINERS_HPP
