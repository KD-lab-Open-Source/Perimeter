#pragma once

#include "TriggerExport.h"

class AddLink
{
public:
	AddLink(TriggerChain & chain,
		int linkOwnerIndex,
		int linkChildIndex,
		CPoint const& szChildOffset,
		CPoint const& szParentOffset,
		int type,
		bool isAutoRestarted
		);
	~AddLink(void);
	static bool run(TriggerChain & chain,
					int linkOwnerIndex,
					int linkChildIndex,
					CPoint const& szChildOffset,
					CPoint const& szParentOffset,
					int type,
					bool isAutoRestarted
					);
	bool operator()();
private:
	TriggerChain & chain_;
	int				linkOwnerIndex_;
	int				linkChildIndex_;
	CPoint			childOffset_;
	CPoint			parentOffset_;
	int				type_;
	bool			isAutoRestarted_;
};
