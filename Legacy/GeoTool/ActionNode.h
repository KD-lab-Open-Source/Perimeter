#ifndef _ACTION_NODE_H
#define _ACTION_NODE_H

#include <XMLUtils.hpp>
#include "Node.h"

class ActionNode : public Node {

	public :
		ActionNode(const string& paramString);

		typedef enum {
			EMPTY,
			PAINT,
			MIX
		} ActionType;

		ActionType getActionType() const {
			return type;
		}

		
	protected :

		ActionType type;

		static const string EMPTY_TYPE;
		static const string PAINT_TYPE;
		static const string MIX_TYPE;

};

#endif // _ACTION_NODE_H