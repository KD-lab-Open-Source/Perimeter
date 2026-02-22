#pragma once

struct AnimationChain
{
	string name;
	int begin_frame;
	int end_frame;
	float time;//Время, за которое должна проигрываться анимационная цепочка.
	bool cycled; //Нужно для корректной интерполяции
};

struct AnimationVisibleGroup
{
	string name;
	vector<IGameNode*> invisible_object;
};

struct AnimationGroup
{
	string name;
	vector<IGameNode*> groups;
};

class AnimationData
{
public:
	vector<AnimationChain> animation_chain;
	vector<AnimationVisibleGroup> animation_chain_group;

	vector<AnimationGroup> animation_group;
	void Save(CSaver& s);
	void Load(CLoadDirectory rd);
protected:
	void SaveGroup(CSaver& s);
	void SaveChain(CSaver& s);
	void SaveChainGroup(CSaver& s);
	void LoadGroup(CLoadDirectory rd);
	void LoadGroupOne(CLoadDirectory rd);

	void LoadChain(CLoadDirectory rd);
	void LoadChainGroup(CLoadDirectory rd);
};