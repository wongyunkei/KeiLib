/*
 * Bundle.h
 *
 *  Created on: 2016¦~6¤ë10¤é
 *      Author: wongy
 */

#ifndef BUNDLE_H_
#define BUNDLE_H_

#include <Program.h>

namespace System{
	class BlinkObj;
	class PWMObj;
};

namespace Time{
	class Task;
	class TaskObj;
};

using namespace Time;

namespace System{

	class Bundle{
		public:
			Bundle();
			BlinkObj** mBlinkObj;
			PWMObj** mPWMObj;
			int BlinkObjNum;
			int PWMObjNum;
			TaskObj* mTaskObj;
			int Cmd;
			float Data;
	};

};

#endif /* BUNDLE_H_ */
