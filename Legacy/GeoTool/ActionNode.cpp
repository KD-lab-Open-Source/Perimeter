#include <stdafx.h>
#include "ActionNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const string ActionNode::EMPTY_TYPE = "Empty";
const string ActionNode::PAINT_TYPE = "Paint";
const string ActionNode::MIX_TYPE = "Mix";

ActionNode::ActionNode(const string& paramString) : Node(paramString) {
	string typeStr = tx3d::XMLUtils::extractPropertyFromTag
					(
						tx3d::XMLUtils::VALUE,
						tx3d::XMLUtils::extractTagFromXML(tx3d::XMLUtils::TYPE, 0, paramString)
					 );
	if (typeStr == EMPTY_TYPE) {
		type = EMPTY;
	} else if (typeStr == PAINT_TYPE) {
		type = PAINT;
	} else if (typeStr == MIX_TYPE) {
		type = MIX;
	}
}

