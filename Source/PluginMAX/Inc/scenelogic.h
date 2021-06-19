#ifndef __SCENE_LOGIC_H__
#define __SCENE_LOGIC_H__

class cLogicScene
{
public:
	unsigned int					ID;
	std::string						name;
	cChannelAnimationLibrary		ChannelLibrary;
	
	cLogicScene()							{ ID=1; }
	~cLogicScene()							{ Release(); }
	void Release()							{ name.clear(); ChannelLibrary.Release(); }
	
	int Read(cMeshFile &f);
	int Write(cMeshFile &f);
};

#endif //__SCENE_LOGIC_H__