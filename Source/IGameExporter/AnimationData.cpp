#include "StdAfx.h"
#include "AnimationData.h"

void AnimationData::Save(CSaver& s)
{
	s.push(C3DX_ANIMATION_HEAD);
	SaveGroup(s);
	SaveChain(s);
	SaveChainGroup(s);
	s.pop();
}

void AnimationData::Load(CLoadDirectory rd)
{
	animation_group.clear();
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_GROUPS:
		LoadGroup(ld);
		break;
	case C3DX_ANIMATION_CHAIN:
		LoadChain(ld);
		break;
	case C3DX_ANIMATION_CHAIN_GROUP:
		LoadChainGroup(ld);
		break;
	}
}

void AnimationData::SaveGroup(CSaver& s)
{
	s.push(C3DX_ANIMATION_GROUPS);

	for(int i=0;i<animation_group.size();i++)
	{
		AnimationGroup& ag=animation_group[i];
		s.push(C3DX_ANIMATION_GROUP);
			s.push(C3DX_AG_HEAD);
			s<<ag.name;
			s.pop();
			s.push(C3DX_AG_LINK);
			s<<(int)ag.groups.size();
			for(int j=0;j<ag.groups.size();j++)
			{
				IGameNode* node=ag.groups[j];
				s<<node->GetName();
//				int inode=pRootExport->FindNodeIndex(node);
//				xassert(inode>=0);
//				s<<inode;
			}
			s.pop();
		s.pop();
	}
	s.pop();
}

void AnimationData::LoadGroup(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_GROUP:
		{
			LoadGroupOne(ld);
		}
		break;
	}
}

void AnimationData::LoadGroupOne(CLoadDirectory rd)
{
	AnimationGroup ag;
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_AG_HEAD:
		{
			CLoadIterator li(ld);
			li>>ag.name;
		}
		break;
	case C3DX_AG_LINK:
		{
			CLoadIterator li(ld);
			int size=0;
			li>>size;
			for(int i=0;i<size;i++)
			{
				IGameNode* node=NULL;
				string node_name;
				li>>node_name;

				node=pRootExport->Find(node_name.c_str());

				if(node)
					ag.groups.push_back(node);
				else
					Msg("Не могу найти ноду %s. Она исчезнет из анимационной группы %s\n",node_name.c_str(),ag.name.c_str());
			}
		}
		break;
	}
	animation_group.push_back(ag);
}


void AnimationData::SaveChain(CSaver& s)
{
	if(animation_chain.empty())
	{
		Msg("Error: Нет ни одной анимационной цепочки.");
	}

	s.push(C3DX_ANIMATION_CHAIN);

	for(int ichain=0;ichain<animation_chain.size();ichain++)
	{
		AnimationChain& ac=animation_chain[ichain];
		s.push(C3DX_AC_ONE);
		s<<ac.name;
		s<<ac.begin_frame;
		s<<ac.end_frame;
		s<<ac.time;
		s<<ac.cycled;
		s.pop();
	}
	s.pop();
}

void AnimationData::SaveChainGroup(CSaver& s)
{
	s.push(C3DX_ANIMATION_CHAIN_GROUP);
	for(int igroup=0;igroup<animation_chain_group.size();igroup++)
	{
		AnimationVisibleGroup& ag=animation_chain_group[igroup];
		s.push(C3DX_ACG_ONE);
		s<<ag.name;
		s<<(int)ag.invisible_object.size();
		for(int i=0;i<ag.invisible_object.size();i++)
		{
			const char* name=ag.invisible_object[i]->GetName();
			s<<name;
		}
		s.pop();
	}
	s.pop();
}

void AnimationData::LoadChain(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_AC_ONE:
		{
			CLoadIterator li(ld);
			AnimationChain ac;
			ac.cycled=false;
			li>>ac.name;
			li>>ac.begin_frame;
			li>>ac.end_frame;
			li>>ac.time;
			li>>ac.cycled;
			animation_chain.push_back(ac);
		}
		break;
	}
}

void AnimationData::LoadChainGroup(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ACG_ONE:
		{
			CLoadIterator li(ld);
			AnimationVisibleGroup ag;
			li>>ag.name;
			int size=0;
			li>>size;
			for(int i=0;i<size;i++)
			{
				string node_name;
				li>>node_name;
				IGameNode* node=pRootExport->Find(node_name.c_str());
				if(node)
					ag.invisible_object.push_back(node);
				else
				{
					Msg("Не могу найти ноду %s. Она станет видимой в группе видимости %s.\n",node_name.c_str(),ag.name.c_str());
				}
			}
			animation_chain_group.push_back(ag);
		}
		break;
	}
}