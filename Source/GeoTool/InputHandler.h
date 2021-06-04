#ifndef _INPUTHANDLER_H
#define _INPUTHANDLER_H

#include "Operator.h"

class InputHandler {
	public:
		virtual ~InputHandler() {
		}
		virtual void handleInput(Operator* op, float deltaT) = 0;
		virtual void inputDisabled(Operator* op) = 0;
};

#endif // _INPUTHANDLER_H