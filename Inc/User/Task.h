/*
 * Task.h
 *
 *  Created on: 2014¦~8¤ë6¤é
 *      Author: YunKei
 */

#ifndef TASK_H_
#define TASK_H_

#include <Program.h>

using namespace std;
using namespace System;

namespace System{
	class Bundle;
	class GPIO;
};

namespace Time{

	class Ticks;

	class TaskObj{
		public:
			typedef void (*pTask)(Bundle* bundle);
			TaskObj(float period, float offset, pTask fn, string fnName, bool isPeriodic, int BreakCout = -1);
			int duration[2];
			pTask mTask;
			uint32_t TaskPeriod;
			uint32_t TaskOffset;
			string TaskName;
			bool IsPeriodic;
			int _BreakCout;
			uint32_t hangCount;
			int executedCount;
	};

	class Task{
		public:
			typedef void (*pTask)(Bundle* bundle);
			Task();
			void Attach(float period, pTask fn, string fnName, int BreakCout = -1, bool keepLoopping = true);
			void Attach(float period, float offset, pTask fn, string fnName, int BreakCout = -1, bool keepLoopping = true);
			void DeAttach(string fnName);
			void Run(bool isPrintTaskNum = false);
			void PrintTasksInfo();
			void PrintTasksInfo(int start, int lengh);
			void PrintDeration();
			static Bundle* mBundle;
			bool IsPrintTaskNum;
			int currentTaskNum;
			TaskObj* mTaskObj[1024];

		private:
			bool KeepLoopping;
			bool OnWatchDog;
			int Count;
			uint32_t TasksNum;
	};
};

#endif /* TASK_H_ */
